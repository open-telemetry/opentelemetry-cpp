#include "opentelemetry/ext/zpages/tracez_processor.h"

#include <benchmark/benchmark.h>
#include <thread>

#include "opentelemetry/context/threadlocal_context.h"

using namespace opentelemetry::sdk::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{

/////////////////////////////// BENCHMARK HELPER FUNCTIONS //////////////////////////////

/*
 * Helper function that creates num_spans threadsafe span data, adding them to the spans
 * vector during setup to benchmark OnStart and OnEnd times for the processor.
 */
void CreateRecordables(std::vector<std::unique_ptr<ThreadsafeSpanData>> &spans,
                       unsigned int num_spans)
{
  for (unsigned int i = 0; i < num_spans; i++)
    spans.push_back(std::unique_ptr<ThreadsafeSpanData>(new ThreadsafeSpanData()));
}

/*
 * Helper function calls GetSpanSnapshot() num_snapshots times, does nothing otherwise.
 * Snapshots are significant and contribute to performance differences because
 * completed spans are cleared from the processor memory. This function
 * simulates an aggregator querying the processor many times, but doesn't
 * aggregate or store spans.
 */
void GetManySnapshots(std::shared_ptr<TracezSpanProcessor> &processor, unsigned int num_snapshots)
{
  for (unsigned int i = 0; i < num_snapshots; i++)
    processor->GetSpanSnapshot();
}

/////////////////////////// PROCESSOR PEER //////////////////////////////

/*
 * Friend class allows us to access processor private variables for
 * benchmarking. It also reduces allows isolation of processor functions
 * to minimize benchmarking noise, since we don't do the extra computation
 * of setting variables the tracer would do with StartSpan and EndSpan
 */

class TracezProcessorPeer
{
public:
  TracezProcessorPeer(std::shared_ptr<TracezSpanProcessor> &processor) { processor_ = processor; }

  /*
   * Calls TracezProcessor::OnStart on each span in spans.
   */
  void StartAllSpans(std::vector<std::unique_ptr<ThreadsafeSpanData>> &spans)
  {
    for (auto &span : spans)
      processor_->OnStart(*(span.get()));
  }

  /*
   * Calls TracezProcessor::OnEnd on each span in spans, which gives ownership
   * of those spans to the processor. Clears the spans vector since everything in
   * the spans vector is now garbage.
   */
  void EndAllSpans(std::vector<std::unique_ptr<ThreadsafeSpanData>> &spans)
  {
    for (auto &span : spans)
      processor_->OnEnd(std::move(span));
    spans.clear();
  }

  /*
   * Clears running span pointers in processor memory, which is used in between
   * iterations so that processor->spans_.running doesn't hold nullptr
   */
  void ClearRunning() { processor_->spans_.running.clear(); }

private:
  std::shared_ptr<TracezSpanProcessor> processor_;
};

////////////////////////  FIXTURE FOR SHARED SETUP CODE ///////////////////

class TracezProcessor : public benchmark::Fixture
{
protected:
  void SetUp(const ::benchmark::State &state)
  {
    processor_      = std::shared_ptr<TracezSpanProcessor>(new TracezSpanProcessor());
    processor_peer_ = std::unique_ptr<TracezProcessorPeer>(new TracezProcessorPeer(processor_));
  }
  std::vector<std::unique_ptr<ThreadsafeSpanData>> spans_;
  std::unique_ptr<TracezProcessorPeer> processor_peer_;
  std::shared_ptr<TracezSpanProcessor> processor_;
};

//////////////////////////// BENCHMARK DEFINITIONS /////////////////////////////////

/*
 * Make many empty spans. This checks the scenario where the processor holds many
 * running spans but never gets queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_MakeRunning)(benchmark::State &state)
{
  const unsigned int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    // Clear span vector if EndAllSpans for spans isn't called to avoid double
    // starting spans
    spans_.clear();
    // Clear processor memory, which ensures no nullptrs are in running spans are
    // set from previous iterations when EndAllSpans isn't called on span/spans2
    processor_peer_->ClearRunning();
    CreateRecordables(spans_, num_spans);
    state.ResumeTiming();

    processor_peer_->StartAllSpans(spans_);
  }
}

/*
 * End many empty spans. This checks the scenario where the processor holds many
 * completed spans but never gets queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_MakeComplete)(benchmark::State &state)
{
  const unsigned int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    CreateRecordables(spans_, num_spans);
    processor_peer_->StartAllSpans(spans_);
    state.ResumeTiming();

    processor_peer_->EndAllSpans(spans_);
  }
}

/*
 * Make many snapshots. This checks the scenario where the processor holds no spans
 * but gets queried many times.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_GetSpans)(benchmark::State &state)
{
  const unsigned int num_snapshots = state.range(0);
  for (auto _ : state)
    GetManySnapshots(processor_, num_snapshots);
}

/*
 * Make and end many empty spans. This checks the scenario where the processor holds
 * many running and completed spans but never gets queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_MakeRunningMakeComplete)(benchmark::State &state)
{
  const unsigned int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    processor_peer_->ClearRunning();
    std::vector<std::unique_ptr<ThreadsafeSpanData>> spans2;
    CreateRecordables(spans_, num_spans);
    CreateRecordables(spans2, num_spans);
    processor_peer_->StartAllSpans(spans_);
    state.ResumeTiming();

    std::thread start(&TracezProcessorPeer::StartAllSpans, processor_peer_.get(), std::ref(spans2));
    processor_peer_->EndAllSpans(spans_);

    start.join();
  }
}

/*
 * Make many empty spans while snapshots grabbed. This checks the scenario where the
 * processor holds many running spans and gets queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_MakeRunningGetSpans)(benchmark::State &state)
{
  const unsigned int num_spans_snaps = state.range(0);  // number of spans and snapshots
  for (auto _ : state)
  {
    state.PauseTiming();
    spans_.clear();
    processor_peer_->ClearRunning();
    CreateRecordables(spans_, num_spans_snaps);
    state.ResumeTiming();

    std::thread start(&TracezProcessorPeer::StartAllSpans, processor_peer_.get(), std::ref(spans_));
    GetManySnapshots(processor_, num_spans_snaps);

    start.join();
  }
}

/*
 * Make many empty spans end while snapshots are being grabbed. This checks the scenario
 * where the processor doesn't make new spans, but existing spans complete while they're
 * queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_GetSpansMakeComplete)(benchmark::State &state)
{
  const unsigned int num_spans_snaps = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    CreateRecordables(spans_, num_spans_snaps);
    processor_peer_->StartAllSpans(spans_);
    state.ResumeTiming();

    std::thread end(&TracezProcessorPeer::EndAllSpans, processor_peer_.get(), std::ref(spans_));
    GetManySnapshots(processor_, num_spans_snaps);

    end.join();
  }
}

/*
 * Make many empty spans and end some, all while snapshots are being grabbed. This
 * checks the scenario where the processor makes new spans, other spans complete,
 * and all spans are queried. This is the case most similar to real situations.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_MakeRunningGetSpansMakeComplete)(benchmark::State &state)
{
  const unsigned int num_spans_snaps = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    processor_peer_->ClearRunning();
    std::vector<std::unique_ptr<ThreadsafeSpanData>> spans2;
    CreateRecordables(spans_, num_spans_snaps);
    CreateRecordables(spans2, num_spans_snaps);
    processor_peer_->StartAllSpans(spans_);
    state.ResumeTiming();

    std::thread end(&TracezProcessorPeer::EndAllSpans, processor_peer_.get(), std::ref(spans_));
    std::thread start(&TracezProcessorPeer::StartAllSpans, processor_peer_.get(), std::ref(spans2));
    GetManySnapshots(processor_, num_spans_snaps);

    start.join();
    end.join();
  }
}

/////////////////////// RUN BENCHMARKS ///////////////////////////

// Arg is the number of spans created for each iteration
BENCHMARK_REGISTER_F(TracezProcessor, BM_MakeRunning)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezProcessor, BM_MakeComplete)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezProcessor, BM_GetSpans)->Arg(10)->Arg(1000);

// These use multiple threads, so that CPU usage and thread works needs to be measured
BENCHMARK_REGISTER_F(TracezProcessor, BM_MakeRunningMakeComplete)
    ->Arg(10)
    ->Arg(1000)
    ->MeasureProcessCPUTime()
    ->UseRealTime();
BENCHMARK_REGISTER_F(TracezProcessor, BM_MakeRunningGetSpans)
    ->Arg(10)
    ->Arg(1000)
    ->MeasureProcessCPUTime()
    ->UseRealTime();
BENCHMARK_REGISTER_F(TracezProcessor, BM_GetSpansMakeComplete)
    ->Arg(10)
    ->Arg(1000)
    ->MeasureProcessCPUTime()
    ->UseRealTime();
BENCHMARK_REGISTER_F(TracezProcessor, BM_MakeRunningGetSpansMakeComplete)
    ->Arg(10)
    ->Arg(1000)
    ->MeasureProcessCPUTime()
    ->UseRealTime();

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE

BENCHMARK_MAIN();
