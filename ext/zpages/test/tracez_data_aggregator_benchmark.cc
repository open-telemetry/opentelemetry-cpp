#include "opentelemetry/context/threadlocal_context.h"
#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"

#include <benchmark/benchmark.h>
#include <chrono>

#include "opentelemetry/sdk/trace/tracer.h"

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;
using opentelemetry::core::SteadyTimestamp;

/////////////////////////////// BENCHMARK HELPER FUNCTIONS //////////////////////////////

/*
 * Helper function that creates and ends i spans instantly
 */
void StartEndSpans(
    std::shared_ptr<opentelemetry::trace::Tracer> tracer,
    int i,
    bool isUnique = false)
{
  for (; i > 0; i--)
    tracer->StartSpan(isUnique ? std::to_string(i) : "")->End();
}

/*
 * Helper function that creates and ends spans instantly, while simulating
 * different latencies.
 */
void StartEndSpansLatency(
    std::shared_ptr<opentelemetry::trace::Tracer> tracer,
    int i,
    bool isUnique = false)
{
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(nanoseconds(0));
  for (; i > 0; i--)
  {
    // Latency bucket depends on the index
    auto latency_band = kLatencyBoundaries[i % kLatencyBoundaries.size()];
    opentelemetry::trace::EndSpanOptions end;
    end.end_steady_time = SteadyTimestamp(latency_band);

    tracer->StartSpan(isUnique ? std::to_string(i) : "", start)->End(end);
  }
}
/*
 * Helper function that creates and ends spans instantly. while simulating
 * error codes.
 */
void StartEndSpansError(
    std::shared_ptr<opentelemetry::trace::Tracer> tracer,
    int i,
    bool isUnique = false)
{
  for (; i > 0; i--)
    tracer->StartSpan(isUnique ? std::to_string(i) : "")
        ->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED, "");
}

/*
 * Helper function that creates i spans that always run.
 */
void StartSpans(
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> &spans,
    std::shared_ptr<opentelemetry::trace::Tracer> tracer,
    int i,
    bool isUnique = false)
{
  for (; i > 0; i--)
    spans.push_back(tracer->StartSpan(isUnique ? std::to_string(i) : ""));
}

////////////////////////  FIXTURE FOR SHARED SETUP CODE ///////////////////

class TracezAggregator : public benchmark::Fixture
{
protected:
  void SetUp(const ::benchmark::State& state)
  {
    std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
    tracer                 = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
    tracez_data_aggregator = std::unique_ptr<TracezDataAggregator>(
        new TracezDataAggregator(processor, milliseconds(10)));
  }

  std::unique_ptr<TracezDataAggregator> tracez_data_aggregator;
  std::shared_ptr<opentelemetry::trace::Tracer> tracer;
};

//////////////////////////// BENCHMARK DEFINITIONS /////////////////////////////////

/*
 * Aggregator handing many spans with the same name, who end instantly.
 */

BENCHMARK_DEFINE_F(TracezAggregator, BM_InstantSame)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    StartEndSpans(tracer, numSpans);
  }
}

/*
 * Aggregator handing many spans with unique names, who end instantly.
 */

BENCHMARK_DEFINE_F(TracezAggregator, BM_InstantUnique)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    StartEndSpans(tracer, numSpans, true);
  }
}

/*
 * Aggregator handing many spans with the same name, who end instantly.
 */

BENCHMARK_DEFINE_F(TracezAggregator, BM_BucketsSame)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> running_spans;
    StartSpans(running_spans, tracer, numSpans / 3);
    StartEndSpansError(tracer, numSpans / 3);
    StartEndSpansLatency(tracer, numSpans / 3);
  }
}

/*
 * Aggregator handing many spans with unique names, who end instantly.
 */

BENCHMARK_DEFINE_F(TracezAggregator, BM_BucketsUnique)(benchmark::State &state)
{
  const int numSpans = state.range(0);
  for (auto _ : state)
  {
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> running_spans;
    StartSpans(running_spans, tracer, numSpans / 3, true);
    StartEndSpansError(tracer, numSpans / 3, true);
    StartEndSpansLatency(tracer, numSpans / 3, true);
  }
}
/////////////////////// RUN BENCHMARKS ///////////////////////////

BENCHMARK_REGISTER_F(TracezAggregator, BM_InstantSame)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_InstantUnique)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_BucketsSame)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_BucketsUnique)->Arg(10)->Arg(1000);

BENCHMARK_MAIN();

