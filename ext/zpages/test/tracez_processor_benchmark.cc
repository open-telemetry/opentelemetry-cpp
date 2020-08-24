#include "opentelemetry/context/threadlocal_context.h"
#include "opentelemetry/ext/zpages/tracez_processor.h"

#include <benchmark/benchmark.h>
#include <thread>

#include "opentelemetry/sdk/trace/tracer.h"

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;

/////////////////////////////// BENCHMARK HELPER FUNCTIONS //////////////////////////////

/*
 * Helper function that creates and add i spans into the passed in vector
 */
void StartManySpans(
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> &spans,
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    int i)
{
  for (; i > 0; i--)
    spans.push_back(tracer->StartSpan(""));
}

/*
 * Helper function that ends all spans in the passed in span vector.
 */
void EndAllSpans(std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> &spans)
{
  while(!spans.empty())
  {
    auto span = spans.back();
    span->End();
    spans.pop_back();
  }
}

/*
 * Helper function calls GetSpanSnapshot() i times, does nothing otherwise.
 * Snapshots are significant and contribute to performance differences because
 * completed spans are cleared from the processor memory.
 */
void GetManySnapshots(std::shared_ptr<TracezSpanProcessor> &processor, int i)
{
  for (; i > 0; i--)
    processor->GetSpanSnapshot();
}

////////////////////////  FIXTURE FOR SHARED SETUP CODE ///////////////////

class TracezProcessor : public benchmark::Fixture
{
protected:
  void SetUp(const ::benchmark::State& state)
  {
    processor  = std::shared_ptr<TracezSpanProcessor>(new TracezSpanProcessor());
    tracer     = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
  }

  std::shared_ptr<TracezSpanProcessor> processor;
  std::shared_ptr<opentelemetry::trace::Tracer> tracer;

  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans;
};

//////////////////////////// BENCHMARK DEFINITIONS /////////////////////////////////

/*
 * Make many empty spans. This checks the scenario where the processor holds
 * many running spans but never gets queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_Run)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    StartManySpans(spans, std::ref(tracer), numSpans);
  }
}

/*
 * Make many snapshots. This checks the scenario where the processor holds
 * no spans but gets queried many times.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_Snap)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    GetManySnapshots(processor, numSpans);
  }
}

/*
 * Make and end many empty spans. This checks the scenario where the processor holds
 * many running and completed spans but never gets queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_RunComplete)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans2;
    StartManySpans(spans2, std::ref(tracer), numSpans);

    std::thread start(StartManySpans, std::ref(spans), std::ref(tracer), numSpans);
    EndAllSpans(spans2);

    start.join();

    EndAllSpans(spans);
  }
}

/*
 * Make many empty spans while spapshots grabbed. This checks the scenario where the
 * processor holds many running spans and gets queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_RunSnap)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    std::thread snapshots(GetManySnapshots, std::ref(processor), numSpans);
    StartManySpans(spans, std::ref(tracer), numSpans);

    snapshots.join();
    EndAllSpans(spans);
  }
}

/*
 * Make many empty spans end while snapshots are being grabbed. This checks the scenario
 * where the processor doesn't make new spans, but existing spans complete while they're
 * queried.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_SnapComplete)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    StartManySpans(spans, std::ref(tracer), numSpans);

    std::thread snapshots(GetManySnapshots, std::ref(processor), numSpans);
    EndAllSpans(spans);

    snapshots.join();
  }

}

/*
 * Make many empty spans and end some, all while snapshots are being grabbed. This
 * checks the scenario where the processor makes new spans, other spans complete,
 * and all spans are queried. This is the case most similar to real situations.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_RunSnapComplete)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans2;

    StartManySpans(spans, std::ref(tracer), numSpans);

    std::thread start(StartManySpans, std::ref(spans2), std::ref(tracer), numSpans);
    std::thread snapshots(GetManySnapshots, std::ref(processor), numSpans);
    EndAllSpans(spans);

    start.join();
    snapshots.join();

    EndAllSpans(spans2);
  }
}

/////////////////////// RUN BENCHMARKS ///////////////////////////

BENCHMARK_REGISTER_F(TracezProcessor, BM_Run)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezProcessor, BM_Snap)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezProcessor, BM_RunComplete)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezProcessor, BM_RunSnap)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezProcessor, BM_SnapComplete)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezProcessor, BM_RunSnapComplete)->Arg(10)->Arg(1000);

BENCHMARK_MAIN();

