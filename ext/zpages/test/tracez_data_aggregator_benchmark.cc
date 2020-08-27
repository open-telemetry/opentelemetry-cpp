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
 * Helper function that creates and ends num_spans spans instantly. If is_unique is
 * true, then all spans will have different names.
 */
void StartEndSpans(
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    int num_spans,
    bool is_unique = false)
{
  opentelemetry::trace::StartSpanOptions start;
  opentelemetry::trace::EndSpanOptions end;
  start.start_steady_time = SteadyTimestamp(nanoseconds(0));
  end.end_steady_time = SteadyTimestamp(nanoseconds(1));
  for (; num_spans > 0; num_spans--)
    tracer->StartSpan(is_unique ? std::to_string(num_spans) : "", start)->End(end);
}

/*
 * Helper function that creates and ends num_spans spans instantly, while evenly
 * spreading spans across all latency bands. If is_unique is true, then all spans
 * will have different names.
 */
void StartEndSpansLatency(
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    int num_spans,
    bool is_unique = false)
{
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(nanoseconds(0));
  for (; num_spans > 0; num_spans--)
  {
    // Latency bucket depends on the index
    nanoseconds latency_band = kLatencyBoundaries[num_spans % kLatencyBoundaries.size()];
    opentelemetry::trace::EndSpanOptions end;
    end.end_steady_time = SteadyTimestamp(latency_band);

    tracer->StartSpan(is_unique ? std::to_string(num_spans) : "", start)->End(end);
  }
}

/*
 * Helper function that creates and ends num_spans spans instantly, while
 * simulating error codes. If is_unique is true, then all spans will have
 * different names.
 */
void StartEndSpansError(
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    int num_spans,
    bool is_unique = false)
{
  for (; num_spans > 0; num_spans--)
    tracer->StartSpan(is_unique ? std::to_string(num_spans) : "")
        ->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED, "");
}

/*
 * Helper function that creates num_spans spans that always run. To remain in scope and running,
 * spans are referenced in the passed in vector. If is_unique is true, then all spans
 * will have different names.
 */
void StartSpans(
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> &spans,
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    int num_spans,
    bool is_unique = false)
{
  for (; num_spans > 0; num_spans--)
    spans.push_back(tracer->StartSpan(is_unique ? std::to_string(num_spans) : ""));
}

/*
 * Helper function that creates about num_spans spans, evenly split between running, latency (which
 * is further split by bucket), and error. If is_unique is true, then all spans will have
 * different names. The running spans vector is returned so that they remain running.
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
    aggregator_ = std::unique_ptr<TracezDataAggregator>(
        new TracezDataAggregator(processor));

    // Disable the aggregetor's periodic background thread aggregation work, which
    // it normally does during production. Disabling it allows us to isolate
    // aggregation work for benchmarking
    if (aggregator_->execute_.load(std::memory_order_acquire))
    {
      aggregator_->execute_.store(false, std::memory_order_release);
      aggregator_->cv_.notify_one();
      aggregator_->aggregate_spans_thread_.join();
    }
  }

  /*
   * Join and end periodic query loop in background if it's running.
   */
  ~TracezDataAggregatorPeer()
  {
    if (run_.load(std::memory_order_acquire))
    {
      run_.store(false, std::memory_order_release);
      query_thread_.join();
    }
  }

  /*
   * Calling the aggregation work function, locking as needed.
   */
  void Aggregate()
  {
    std::lock_guard<std::mutex> lock(mtx_);
    aggregator_->AggregateSpans();
  }

  /*
   * Starts a background thread to query for aggregated data every
   * query_interval nanoseconds. This should affect performance for the
   * aggregator, since aggregation work cannot be done while aggegation
   * data is being requested. This simulates a user visiting or refreshing
   * the Tracez webpage many times, except this function does and holds
   * nothing in memory.
   */
  void StartPeriodicQueryThread(nanoseconds query_interval = nanoseconds(1))
  {
    run_.store(true, std::memory_order_release);
    query_thread_ = std::thread([this, query_interval]() {
      while (run_.load(std::memory_order_acquire))
      {
        std::unique_lock<std::mutex> lock(mtx_);
        auto aggregations = aggregator_->GetAggregatedTracezData();
        // Continue if query interval passed
        cont_.wait_for(lock, query_interval);
      }
    });
  }

private:
  std::unique_ptr<TracezDataAggregator> aggregator_;
  // Keep queries running while peer is in memory
  std::thread query_thread_;
  // Ensure queries and aggregations don't happen simultaneously
  std::mutex mtx_;
  std::atomic<bool> run_;
  std::condition_variable cont_;
  
};

////////////////////////  FIXTURE FOR SHARED SETUP CODE ///////////////////

/*
 * This is a regular fixture that initializes the shared needed components
 * to create spans and do the aggregation work for benchmarking.
 */
class TracezAggregator : public benchmark::Fixture
{
protected:
  void SetUp(const ::benchmark::State& state)
  {
    std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
    tracer_ = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
    aggregator_peer_ = std::unique_ptr<TracezDataAggregatorPeer>(
        new TracezDataAggregatorPeer(processor));
  }

  std::unique_ptr<TracezDataAggregatorPeer> aggregator_peer_;
  // Tracer for creating spans
  std::shared_ptr<opentelemetry::trace::Tracer> tracer_;
};

////////////////////////// BENCHMARK HELPERS ///////////////////////////////

/*
 * Aggregator handling many spans with the same name, who end instantly. This
 * checks the scenario where there's only one Tracez name and minimal sorting
 * of spans into different latency bands is required, as all spans should be
 * sorted in the same bucket under the same span name.
 */
void SingleBucketSingleName(
    benchmark::State &state,
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    std::unique_ptr<TracezDataAggregatorPeer> const &aggregator_peer)
{
  for (auto _ : state)
  {
    // Do not time span creation, as we're only benchmarking aggregation work
    state.PauseTiming();
    StartEndSpans(tracer, state.range(0));
    state.ResumeTiming();
    aggregator_peer->Aggregate();
  }
}

/*
 * Aggregator handling many spans with unique names, who end instantly. This
 * checks the scenario where there's many Tracez groups but minimal sorting
 * of spans into different latency bands is required. Spans are sorted in
 * different groups but always in the same bucket.
 */
void SingleBucketManyNames(
    benchmark::State &state,
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    std::unique_ptr<TracezDataAggregatorPeer> const &aggregator_peer)
{
  for (auto _ : state)
  {
    state.PauseTiming();
    StartEndSpans(tracer, state.range(0), true);
    state.ResumeTiming();
    aggregator_peer->Aggregate();
  }
}

/*
 * Aggregator handling many spans with the same name, who may fall under error,
 * running, and any latency group. This checks the scenario where there's only
 * one Tracez groups but there also needs to be sorting of spans into their
 * respective buckets (running, error, latency [including within latency bands]).
 * Spans are in the same group but sorted to different buckets.
 */
void ManyBucketsSingleName(
    benchmark::State &state,
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    std::unique_ptr<TracezDataAggregatorPeer> const &aggregator_peer)
{
  for (auto _ : state)
  {
    state.PauseTiming();
    auto running_spans = MakeManySpans(tracer, state.range(0));
    state.ResumeTiming();
    aggregator_peer->Aggregate();
  }
}

/*
 * Aggregator handling many spans with unique names, who may fall under error,
 * running, and any latency group. This checks the scenario where there's many
 * Tracez groups and there also needs to be sorting of spans into their
 * respective buckets (running, error, latency [including within latency bands]).
 * Spans are in the same group but sorted to different buckets.
 */
void ManyBucketsManyNames(
    benchmark::State &state,
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    std::unique_ptr<TracezDataAggregatorPeer> const &aggregator_peer)
{
  for (auto _ : state)
  {
    state.PauseTiming();
    auto running_spans = MakeManySpans(tracer, state.range(0));
    state.ResumeTiming();
    aggregator_peer->Aggregate();
  }
}

//////////////////////// USER NEVER VISITS WEBPAGE //////////////////////////////

BENCHMARK_DEFINE_F(TracezAggregator, BM_SingleBucketSingleName)(benchmark::State &state)
{
  SingleBucketSingleName(state, tracer_, aggregator_peer_);
}

BENCHMARK_DEFINE_F(TracezAggregator, BM_SingleBucketManyNames)(benchmark::State &state)
{
  SingleBucketManyNames(state, tracer_, aggregator_peer_);
}

BENCHMARK_DEFINE_F(TracezAggregator, BM_ManyBucketsSingleName)(benchmark::State &state)
{
  ManyBucketsSingleName(state, tracer_, aggregator_peer_);
}

BENCHMARK_DEFINE_F(TracezAggregator, BM_ManyBucketsManyNames)(benchmark::State &state)
{
  ManyBucketsManyNames(state, tracer_, aggregator_peer_);
}

///////////// SAME BENCHMARKS, BUT AGGREGATIONS REQUESTED PERIODICALLY /////////////////

BENCHMARK_DEFINE_F(TracezAggregator, BM_SingleBucketSingleNameFetch)(benchmark::State &state)
{
  aggregator_peer_->StartPeriodicQueryThread();
  SingleBucketSingleName(state, tracer_, aggregator_peer_);
}

BENCHMARK_DEFINE_F(TracezAggregator, BM_SingleBucketManyNamesFetch)(benchmark::State &state)
{
  aggregator_peer_->StartPeriodicQueryThread();
  SingleBucketManyNames(state, tracer_, aggregator_peer_);
}

BENCHMARK_DEFINE_F(TracezAggregator, BM_ManyBucketsSingleNameFetch)(benchmark::State &state)
{
  aggregator_peer_->StartPeriodicQueryThread();
  ManyBucketsSingleName(state, tracer_, aggregator_peer_);
}

BENCHMARK_DEFINE_F(TracezAggregator, BM_ManyBucketsManyNamesFetch)(benchmark::State &state)
{
  aggregator_peer_->StartPeriodicQueryThread();
  ManyBucketsManyNames(state, tracer_, aggregator_peer_);
}

//////////////////////////// RUN BENCHMARKS ///////////////////////////////

// Arg is the number of spans created for each iteration
BENCHMARK_REGISTER_F(TracezAggregator, BM_SingleBucketSingleName)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_SingleBucketSingleNameFetch)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_SingleBucketManyNames)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_SingleBucketManyNamesFetch)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_ManyBucketsSingleName)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_ManyBucketsSingleNameFetch)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_ManyBucketsManyNames)->Arg(10)->Arg(1000);
BENCHMARK_REGISTER_F(TracezAggregator, BM_ManyBucketsManyNamesFetch)->Arg(10)->Arg(1000);

} // namespace zpages
} // namespace ext
OPENTELEMETRY_END_NAMESPACE

BENCHMARK_MAIN();

