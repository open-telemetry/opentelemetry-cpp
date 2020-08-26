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
void CreateRecordables(
    std::vector<std::unique_ptr<ThreadsafeSpanData>> &spans,
    int num_spans)
{
  for (; num_spans > 0; num_spans--)
    spans.push_back(std::unique_ptr<ThreadsafeSpanData>(new ThreadsafeSpanData()));
}

/*
 * Helper function calls GetSpanSnapshot() num_snapshots times, does nothing otherwise.
 * Snapshots are significant and contribute to performance differences because
 * completed spans are cleared from the processor memory. This function
 * simulates an aggregator querying the processor many times, but doesn't
 * aggregate or store spans.
 */
void GetManySnapshots(std::shared_ptr<TracezSpanProcessor> &processor, int num_snapshots)
{
  for (; num_snapshots > 0; num_snapshots--)
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
  TracezProcessorPeer(std::shared_ptr<TracezSpanProcessor> &processor_in)
  {
    processor  = processor_in;
  }

  /*
   * Calls TracezProcessor::OnStart on each span in spans.
   */
  void StartAllSpans(std::vector<std::unique_ptr<ThreadsafeSpanData>> &spans)
  {
    for(auto &span : spans)
      processor->OnStart(*(span.get()));
  }

  /*
   * Calls TracezProcessor::OnEnd on each span in spans, which gives ownership
   * of those spans to the processor. Clears the spans vector since everything in
   * the spans vector is now garbage.
   */
  void EndAllSpans(std::vector<std::unique_ptr<ThreadsafeSpanData>> &spans)
  {
    for(auto &span : spans)
      processor->OnEnd(std::move(span));
    spans.clear();
  }

  /*
   * Clears running span pointers in processor memory, which is used in between
   * iterations so that spans_.running doesn't hold nullptr
   */
  void ClearRunning()
  {
    processor->spans_.running.clear();
  }

private:
  std::shared_ptr<TracezSpanProcessor> processor;
};

////////////////////////  FIXTURE FOR SHARED SETUP CODE ///////////////////

class TracezProcessor : public benchmark::Fixture
{
protected:
  void SetUp(const ::benchmark::State& state)
  {
    processor  = std::shared_ptr<TracezSpanProcessor>(new TracezSpanProcessor());
    processor_peer = std::unique_ptr<TracezProcessorPeer>(new TracezProcessorPeer(processor));
  }
  std::vector<std::unique_ptr<ThreadsafeSpanData>> spans;
  std::unique_ptr<TracezProcessorPeer> processor_peer;
  std::shared_ptr<TracezSpanProcessor> processor;

};

//////////////////////////// BENCHMARK DEFINITIONS /////////////////////////////////

/*
 * Make many empty spans. This checks the scenario where the processor holds many
 * running spans but never gets queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_MakeRunning)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    // Clear span vector if EndAllSpans for spans isn't called
    spans.clear();
    // Clear processor memory, ensure no nullptrs are in running span set from
    // previous iterations from either spans or spans2 if EndAllSpans isn't called
    processor_peer->ClearRunning();
    CreateRecordables(spans, num_spans);
    state.ResumeTiming();

    processor_peer->StartAllSpans(spans);
  }
}

/*
 * End many empty spans. This checks the scenario where the processor holds many
 * completed spans but never gets queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_MakeComplete)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    CreateRecordables(spans, num_spans);
    processor_peer->StartAllSpans(spans);
    state.ResumeTiming();

    processor_peer->EndAllSpans(spans);
  }
}

/*
 * Make many snapshots. This checks the scenario where the processor holds no spans
 * but gets queried many times.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_GetSpans)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
    GetManySnapshots(processor, num_spans);
}

/*
 * Make and end many empty spans. This checks the scenario where the processor holds
 * many running and completed spans but never gets queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_MakeRunningMakeComplete)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    processor_peer->ClearRunning();
    std::vector<std::unique_ptr<ThreadsafeSpanData>> spans2;
    CreateRecordables(spans, num_spans);
    CreateRecordables(spans2, num_spans);
    processor_peer->StartAllSpans(spans);
    state.ResumeTiming();

    std::thread start(&TracezProcessorPeer::StartAllSpans, processor_peer.get(), std::ref(spans2));
    processor_peer->EndAllSpans(spans);

    start.join();
  }
}

/*
 * Make many empty spans while spapshots grabbed. This checks the scenario where the
 * processor holds many running spans and gets queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_MakeRunningGetSpans)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    spans.clear();
    processor_peer->ClearRunning();
    CreateRecordables(spans, num_spans);
    state.ResumeTiming();

    std::thread snapshots(GetManySnapshots, std::ref(processor), num_spans);
    processor_peer->StartAllSpans(spans);

    snapshots.join();
  }
}

/*
 * Make many empty spans end while snapshots are being grabbed. This checks the scenario
 * where the processor doesn't make new spans, but existing spans complete while they're
 * queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_GetSpansMakeComplete)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    CreateRecordables(spans, num_spans);
    processor_peer->StartAllSpans(spans);
    state.ResumeTiming();

    std::thread snapshots(GetManySnapshots, std::ref(processor), num_spans);
    processor_peer->EndAllSpans(spans);

    snapshots.join();
  }
}

/*
 * Make many empty spans and end some, all while snapshots are being grabbed. This
 * checks the scenario where the processor makes new spans, other spans complete,
 * and all spans are queried. This is the case most similar to real situations.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_MakeRunningGetSpansMakeComplete)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    processor_peer->ClearRunning();
    std::vector<std::unique_ptr<ThreadsafeSpanData>> spans2;
    CreateRecordables(spans, num_spans);
    CreateRecordables(spans2, num_spans);
    processor_peer->StartAllSpans(spans);
    state.ResumeTiming();

    std::thread snapshots(GetManySnapshots, std::ref(processor), num_spans);
    std::thread start(&TracezProcessorPeer::StartAllSpans, processor_peer.get(), std::ref(spans2));
    processor_peer->EndAllSpans(spans);

    start.join();
    snapshots.join();
  }
}

/////////////////////// RUN BENCHMARKS ///////////////////////////

// Arg is the number of spans created for each iteration
BENCHMARK_REGISTER_F(TracezProcessor, BM_MakeRunning)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezProcessor, BM_GetSpans)->Arg(10)->Arg(1000);
// These use multiple threads, so that CPU usage needs to be measured as well
BENCHMARK_REGISTER_F(TracezProcessor, BM_MakeRunningMakeComplete)->Arg(10)->Arg(1000)
	->MeasureProcessCPUTime()->UseRealTime();
BENCHMARK_REGISTER_F(TracezProcessor, BM_MakeRunningGetSpans)->Arg(10)->Arg(1000)
	->MeasureProcessCPUTime()->UseRealTime();
BENCHMARK_REGISTER_F(TracezProcessor, BM_GetSpansMakeComplete)->Arg(10)->Arg(1000)
	->MeasureProcessCPUTime()->UseRealTime();
BENCHMARK_REGISTER_F(TracezProcessor, BM_MakeRunningGetSpansMakeComplete)->Arg(10)->Arg(1000)
	->MeasureProcessCPUTime()->UseRealTime();

} // namespace zpages
} // namespace ext
OPENTELEMETRY_END_NAMESPACE

BENCHMARK_MAIN();

