#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"

#include <benchmark/benchmark.h>
#include <chrono>

#include "opentelemetry/context/threadlocal_context.h"
#include "opentelemetry/sdk/trace/tracer.h"

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;
using opentelemetry::core::SteadyTimestamp;

/////////////////////////////// BENCHMARK HELPER FUNCTIONS //////////////////////////////

/*
 * Helper function that creates and ends i spans instantly. If is_unique is
 * true, then all spans will have different names.
 */
void StartEndSpans(
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    int i,
    bool is_unique = false)
{
  opentelemetry::trace::StartSpanOptions start_end;
  start_end.start_steady_time = SteadyTimestamp(nanoseconds(0));
  for (; i > 0; i--)
    tracer->StartSpan(isUnique ? std::to_string(i) : "", start_end)
        ->End(start_end);
}

/*
 * Helper function that creates and ends spans instantly, while simulating
 * different latencies. If is_unique is true, then all spans will have
 * different names.

 */
void StartEndSpansLatency(
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    int i,
    bool is_unique = false)
{
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(nanoseconds(0));
  for (; i > 0; i--)
  {
    // Latency bucket depends on the index
    auto latency_band = kLatencyBoundaries[i % kLatencyBoundaries.size()];
    opentelemetry::trace::EndSpanOptions end;
    end.end_steady_time = SteadyTimestamp(latency_band);

    tracer->StartSpan(is_unique ? std::to_string(i) : "", start)->End(end);
  }
}

/*
 * Helper function that creates and ends spans instantly, while simulating
 * error codes. If is_unique is true, then all spans will have different names.
 */
void StartEndSpansError(
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    int i,
    bool is_unique = false)
{
  for (; i > 0; i--)
    tracer->StartSpan(is_unique ? std::to_string(i) : "")
        ->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED, "");
}

/*
 * Helper function that creates i spans that always run. If is_unique is true,
 * then all spans will have different names.
 */
void StartSpans(
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> &spans,
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    int i,
    bool is_unique = false)
{
  for (; i > 0; i--)
    spans.push_back(tracer->StartSpan(is_unique ? std::to_string(i) : ""));
}

/*
 * Helper function that queries for Tracez data. This simulates a user visiting
 * or refreshing the Tracez webpage many times, except this function does and
 * holds nothing in memory. While this runs, aggregation work and performance is
 * expected is be affected negatively.
 */

void GetManyAggregations(std::unique_ptr<TracezDataAggregator> &aggregator, int i)
{
  for (; i > 0; i--)
    aggregator->GetAggregatedTracezData();
}

////////////////////////  FIXTURE FOR SHARED SETUP CODE ///////////////////

class TracezAggregator : public benchmark::Fixture
{
protected:
  void SetUp(const ::benchmark::State& state)
  {
    std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
    tracer     = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
    aggregator = std::unique_ptr<TracezDataAggregator>(
        new TracezDataAggregator(processor, milliseconds(10)));
  }

  std::unique_ptr<TracezDataAggregator> aggregator;
  std::shared_ptr<opentelemetry::trace::Tracer> tracer;
};

//////////////////////////// BENCHMARK DEFINITIONS /////////////////////////////////

//////////////////////// USER NEVER VISITS WEBPAGE //////////////////////////////

/*
 * Aggregator handing many spans with the same name, who end instantly. This
 * checks the scenario where there's only one Tracez name and minimal sorting
 * of latencies is required, as all spans should be sorted in the same bucket
 * under the same span name.
 */
BENCHMARK_DEFINE_F(TracezAggregator, BM_SingleBucketSingleName)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    StartEndSpans(tracer, num_spans);
  }
}

/*
 * Aggregator handing many spans with unique names, who end instantly. This
 * checks the scenario where there's many Tracez groups but minimal sorting
 * of latencies is required. Spans are sorted in different groups but always
 * in the same bucket.
 */
BENCHMARK_DEFINE_F(TracezAggregator, BM_SingleBucketManyNames)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    StartEndSpans(tracer, num_spans, true);
  }
}

/*
 * Aggregator handing many spans with the same name, who may fall under error,
 * running, and any latency group. This checks the scenario where there's only
 * one Tracez groups but there also needs to be sorting of spans into their
 * respective buckets. Spans are in the same group but sorted to different
 * buckets.
 */
BENCHMARK_DEFINE_F(TracezAggregator, BM_ManyBucketsSingleName)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> running_spans;
    std::thread run(StartSpans, std::ref(running_spans), std::ref(tracer), num_spans / 3, false);
    std::thread err(StartEndSpansError, std::ref(tracer), num_spans / 3, false);
    StartEndSpansLatency(tracer, num_spans / 3);
    run.join();
    err.join();
  }
}

/*
 * Aggregator handing many spans with unique names, who may fall under error,
 * running, and any latency group. This checks the scenario where there's many
 * Tracez groups and there needs to be sorting of spans into their
 * respective buckets. Spans are in the different groups and buckets, similar to
 * likely real use cases.
 */
BENCHMARK_DEFINE_F(TracezAggregator, BM_ManyBucketsManyNames)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> running_spans;
    std::thread run(StartSpans, std::ref(running_spans), std::ref(tracer), num_spans / 3, true);
    std::thread err(StartEndSpansError, std::ref(tracer), num_spans / 3, true);
    StartEndSpansLatency(tracer, num_spans / 3, true);
    run.join();
    err.join();
  }
}

//////////////////// SAME BENCHMARKS, BUT USER VISITS WEBPAGE ////////////////////////

BENCHMARK_DEFINE_F(TracezAggregator, BM_SingleBucketSingleNameGet)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    std::thread spans(StartEndSpans, std::ref(tracer), num_spans, false);
    GetManyAggregations(std::ref(aggregator), num_spans);
    spans.join();
  }
}

BENCHMARK_DEFINE_F(TracezAggregator, BM_SingleBucketManyNamesGet)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    std::thread spans(StartEndSpans, std::ref(tracer), num_spans, true);
    GetManyAggregations(std::ref(aggregator), num_spans);
    spans.join();
  }
}

BENCHMARK_DEFINE_F(TracezAggregator, BM_ManyBucketsSingleNameGet)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> running_spans;
    std::thread run(StartSpans, std::ref(running_spans), std::ref(tracer), num_spans / 3, false);
    std::thread err(StartEndSpansError, std::ref(tracer), num_spans / 3, false);
    std::thread lat(StartEndSpansLatency, std::ref(tracer), num_spans / 3, false);
    GetManyAggregations(std::ref(aggregator), num_spans / 3);
    run.join();
    err.join();
    lat.join();
  }
}

BENCHMARK_DEFINE_F(TracezAggregator, BM_ManyBucketsManyNamesGet)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> running_spans;
    std::thread run(StartSpans, std::ref(running_spans), std::ref(tracer), num_spans / 3, true);
    std::thread err(StartEndSpansError, std::ref(tracer), num_spans / 3, true);
    std::thread lat(StartEndSpansLatency, std::ref(tracer), num_spans / 3, true);
    GetManyAggregations(std::ref(aggregator), num_spans / 3);
    run.join();
    err.join();
    lat.join();
  }
}

//////////////////////////// RUN BENCHMARKS ///////////////////////////////

// Arg is the number of spans created for each iteration
BENCHMARK_REGISTER_F(TracezAggregator, BM_SingleBucketSingleName)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_SingleBucketSingleNameGet)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_SingleBucketManyNames)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_SingleBucketManyNamesGet)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_ManyBucketsSingleName)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_ManyBucketsSingleNameGet)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_ManyBucketsManyNames)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_ManyBucketsManyNamesGet)->Arg(10)->Arg(1000);

BENCHMARK_MAIN();

