#include "opentelemetry/context/threadlocal_context.h"
#include "opentelemetry/ext/zpages/tracez_processor.h"

#include <benchmark/benchmark.h>
#include <thread>

#include "opentelemetry/sdk/trace/tracer.h"

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;

/////////////////////////////// BENCHMARK HELPER FUNCTIONS //////////////////////////////

/*
 * Helper function that creates i spans, which are added into the passed
 * in vector. Used for testing thread safety
 */
void StartManySpans(
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> &spans,
    std::shared_ptr<opentelemetry::trace::Tracer> tracer,
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
  for (auto &span : spans)
    span->End();
}

/*
 * Helper function calls GetSpanSnapshot() i times, does nothing otherwise
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
 * Make and end many empty spans.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_Run)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    StartManySpans(spans, tracer, numSpans);
  }
}

/*
 * Make and end many empty spans.
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
 * Make and end many empty spans.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_RunComplete)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans2;
    StartManySpans(spans2, tracer, numSpans);

    std::thread start(StartManySpans, std::ref(spans), tracer, numSpans);
    EndAllSpans(spans2);

    start.join();

    EndAllSpans(spans);
  }
}

/*
 * Make many empty spans while spapshots grabbed.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_RunSnap)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    std::thread snapshots(GetManySnapshots, std::ref(processor), numSpans);
    StartManySpans(spans, tracer, numSpans);

    snapshots.join();
    EndAllSpans(spans);
  }
}

/*
 * Make many empty spans end while snapshots are being grabbed.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_SnapComplete)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    StartManySpans(spans, tracer, numSpans);

    std::thread snapshots(GetManySnapshots, std::ref(processor), numSpans);
    EndAllSpans(spans);

    snapshots.join();
  }

}

/*
 * Make many empty spans and end them, all while snapshots are being grabbed.
 */
BENCHMARK_DEFINE_F(TracezProcessor, BM_RunSnapComplete)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans2;

    StartManySpans(spans, tracer, numSpans);

    std::thread start(StartManySpans, std::ref(spans2), tracer, numSpans);
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

