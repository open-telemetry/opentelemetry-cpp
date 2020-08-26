#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"

#include <benchmark/benchmark.h>
#include <chrono>
#include <iostream>

#include "opentelemetry/context/threadlocal_context.h"
#include "opentelemetry/sdk/trace/tracer.h"

using namespace opentelemetry::sdk::trace;
using opentelemetry::core::SteadyTimestamp;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{

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
  opentelemetry::trace::StartSpanOptions start;
  opentelemetry::trace::EndSpanOptions end;
  start.start_steady_time = SteadyTimestamp(nanoseconds(0));
  end.end_steady_time = SteadyTimestamp(nanoseconds(1));
  for (; i > 0; i--)
    tracer->StartSpan(is_unique ? std::to_string(i) : "", start)->End(end);
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
 * Helper function that creates i spans that always run. To remain in scope and running,
 * spans are referenced in the passed in vector. If is_unique is true, then all spans
 * will have different names.
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
 * Helper function that creates about i spans, evenly split between running, latency (which
 * is further split by bucket), and error. If is_unique is true, then all spans will have
 * different names.
 */
std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> MakeManySpans(
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    int num_spans,
    bool is_unique = false)
{
  // Running spans must be stored in a vector in order to stay running
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> running_spans;
  // Use threads to speed up the work
  std::thread run(StartSpans, std::ref(running_spans), std::ref(tracer), num_spans / 3, is_unique);
  std::thread err(StartEndSpansError, std::ref(tracer), num_spans / 3, is_unique);
  StartEndSpansLatency(tracer, num_spans / 3, is_unique);
  run.join();
  err.join();
  return running_spans;
}


/////////////////////////// AGGREGATOR PEER //////////////////////////////

/*
 * Friend class allows us to access aggregator private variables for
 * benchmarking and isolation of different functions
 */
class TracezDataAggregatorPeer
{
public:
  TracezDataAggregatorPeer(std::shared_ptr<TracezSpanProcessor> processor)
  {
    // Set up the aggregator
    aggregator = std::unique_ptr<TracezDataAggregator>(
        new TracezDataAggregator(processor));

    // Disable the aggregetor's periodic background thread aggregation work, which
    // it normally does during production. Disabling it allows us to isolate
    // aggregation work for benchmarking
    if (aggregator->execute_.load(std::memory_order_acquire))
    {
      aggregator->execute_.store(false, std::memory_order_release);
      aggregator->cv_.notify_one();
      aggregator->aggregate_spans_thread_.join();
    }
  }

  /*
   * Join and end periodic query loop in background if it's running.
   */
  ~TracezDataAggregatorPeer()
  {
    if (run.load(std::memory_order_acquire))
    {
      run.store(false, std::memory_order_release);
      query_thread.join();
    }
  }

  /*
   * Calling the aggregation work function, locking as needed.
   */
  void Aggregate()
  {
    std::lock_guard<std::mutex> lock(mtx);
    aggregator->AggregateSpans();
  }

  /*
   * Starts a background thread to query for aggregated data every i
   * nanoseconds. This should affect performance for the aggregator,
   * since aggregation work cannot be done while aggegation data is being
   * requested. This simulates a user visiting or refreshing the Tracez
   * webpage many times, except this function does and holds nothing in
   * memory.
   */
  void StartPeriodicQuery(int i = 1)
  {
    run.store(true, std::memory_order_release);
    nanoseconds query_interval = nanoseconds(i);
    query_thread = std::thread([this, query_interval]() {
      while (run.load(std::memory_order_acquire))
      {
        std::unique_lock<std::mutex> lock(mtx);
        aggregator->GetAggregatedTracezData();
        // Continue if mutex is free and query interval passed
        cont.wait_for(lock, query_interval);
      }
    });
  }

private:
  std::unique_ptr<TracezDataAggregator> aggregator;
  // Keep queries running while peer is in memory
  std::thread query_thread;
  // Ensure queries and aggregations don't happen simultaneously
  std::mutex mtx;
  std::atomic<bool> run;
  std::condition_variable cont;
  
};

////////////////////////  FIXTURE FOR SHARED SETUP CODE ///////////////////

/*
 * This is a regular fixture that initializes the shared needed components
 * to create spans and do the aggregation work for benchmarking.
 */
class TracezAggregator : public benchmark::Fixture
{
protected:
  void SetTracerAggregatorPeer()
  {
    std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
    tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
    aggregator_peer = std::unique_ptr<TracezDataAggregatorPeer>(
        new TracezDataAggregatorPeer(processor));
  }

  void SetUp(const ::benchmark::State& state)
  {
    SetTracerAggregatorPeer();
  }

  // Aggregator peer for accessing private aggregator functions
  std::unique_ptr<TracezDataAggregatorPeer> aggregator_peer;
  // Tracer for creating spans
  std::shared_ptr<opentelemetry::trace::Tracer> tracer;
};

/*
 * This extend the previous fixture, adding in the periodic query work in
 * the background to simulate users requesting the data on the frontend
 */
class TracezAggregatorFetch : public TracezAggregator
{
protected:
  void SetUp(const ::benchmark::State& state) override
  {
    SetTracerAggregatorPeer();
    aggregator_peer->StartPeriodicQuery();
  }
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
    // Do not time span creation, as we're only benchmarking aggregation work
    state.PauseTiming();
    StartEndSpans(tracer, num_spans);
    state.ResumeTiming();
    aggregator_peer->Aggregate();
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
    state.PauseTiming();
    StartEndSpans(tracer, num_spans, false);
    state.ResumeTiming();
    aggregator_peer->Aggregate();
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
    state.PauseTiming();
    auto running_spans = MakeManySpans(tracer, num_spans, false);
    state.ResumeTiming();
    aggregator_peer->Aggregate();
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
    state.PauseTiming();
    auto running_spans = MakeManySpans(tracer, num_spans, true);
    state.ResumeTiming();
    aggregator_peer->Aggregate();
  }
}

////////////////// SAME BENCHMARKS, BUT USER VISITS WEBPAGE ///////////////////////////

BENCHMARK_DEFINE_F(TracezAggregatorFetch, BM_SingleBucketSingleName)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    StartEndSpans(tracer, num_spans);
    state.ResumeTiming();
    aggregator_peer->Aggregate();
  }
}

BENCHMARK_DEFINE_F(TracezAggregatorFetch, BM_SingleBucketManyNames)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    StartEndSpans(tracer, num_spans, false);
    state.ResumeTiming();
    aggregator_peer->Aggregate();
  }
}

BENCHMARK_DEFINE_F(TracezAggregatorFetch, BM_ManyBucketsSingleName)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    auto running_spans = MakeManySpans(tracer, num_spans);
    state.ResumeTiming();
    aggregator_peer->Aggregate();
  }
}

BENCHMARK_DEFINE_F(TracezAggregatorFetch, BM_ManyBucketsManyNames)(benchmark::State &state)
{
  const int num_spans = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    auto running_spans = MakeManySpans(tracer, num_spans, true);
    state.ResumeTiming();
    aggregator_peer->Aggregate();
  }
}

//////////////////////////// RUN BENCHMARKS ///////////////////////////////

// Arg is the number of spans created for each iteration
BENCHMARK_REGISTER_F(TracezAggregator, BM_SingleBucketSingleName)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregatorFetch, BM_SingleBucketSingleName)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_SingleBucketManyNames)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregatorFetch, BM_SingleBucketManyNames)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_ManyBucketsSingleName)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregatorFetch, BM_ManyBucketsSingleName)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_ManyBucketsManyNames)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregatorFetch, BM_ManyBucketsManyNames)->Arg(10)->Arg(1000);

} // namespace zpages
} // namespace ext
OPENTELEMETRY_END_NAMESPACE

BENCHMARK_MAIN();

