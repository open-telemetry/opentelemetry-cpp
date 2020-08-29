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
void StartEndSpans(std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
                   unsigned int num_spans,
                   bool is_unique)
{
  opentelemetry::trace::StartSpanOptions start;
  opentelemetry::trace::EndSpanOptions end;
  start.start_steady_time = SteadyTimestamp(nanoseconds(0));
  end.end_steady_time     = SteadyTimestamp(nanoseconds(1));
  for (unsigned int i = 0; i < num_spans; i++)
    tracer->StartSpan(is_unique ? std::to_string(num_spans) : "", start)->End(end);
}

/*
 * Helper function that creates and ends num_spans spans instantly, while evenly
 * spreading spans across all latency bands. If is_unique is true, then all spans
 * will have different names.
 */
void StartEndSpansLatency(std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
                          unsigned int num_spans,
                          bool is_unique,
                          unsigned int latency_span_offset)
{
  opentelemetry::trace::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(nanoseconds(0));
  for (unsigned int i = 0; i < num_spans; i++)
  {
    // Latency bucket depends on the index. Pass-by-ref offset means each span name will have all
    // buckets filled
    nanoseconds latency_band =
        kLatencyBoundaries[num_spans + latency_span_offset % kLatencyBoundaries.size()];
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
void StartEndSpansError(std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
                        unsigned int num_spans,
                        bool is_unique)
{
  for (unsigned int i = 0; i < num_spans; i++)
    tracer
        ->StartSpan(is_unique ? std::to_string(num_spans) : "")
        // Random error status
        ->SetStatus(opentelemetry::trace::CanonicalCode::CANCELLED, "");
}

/*
 * Helper function that creates num_spans spans that always run. To remain in scope and running,
 * spans are referenced in the passed in vector. If is_unique is true, then all spans
 * will have different names.
 */
void StartSpans(std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> &spans,
                std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
                unsigned int num_spans,
                bool is_unique)
{
  for (unsigned int i = 0; i < num_spans; i++)
    spans.push_back(tracer->StartSpan(is_unique ? std::to_string(num_spans) : ""));
}

/*
 * Helper function that creates about num_spans spans, evenly split between running, latency (which
 * is further split by bucket), and error. If is_unique is true, then all spans will have
 * different names. The running spans vector is returned so that they remain running. A vector is
 * returned holding the running spans so they can be held in the caller function and remain running.
 */
std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> MakeManySpans(
    std::shared_ptr<opentelemetry::trace::Tracer> &tracer,
    unsigned int num_spans,
    bool is_unique,
    unsigned int &latency_span_offset)
{
  // Running spans must be stored in a vector in order to stay running, since only OnStart is called
  // for those spans. This vector is only accessed by the run thread, as the other functions' spans
  // automatically get moved to the processor memory when calling both OnStart and OnEnd
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> running_spans;
  // Use threads to speed up the work
  std::thread run(StartSpans, std::ref(running_spans), std::ref(tracer), num_spans / 6, is_unique);
  std::thread err(StartEndSpansError, std::ref(tracer), num_spans / 6, is_unique);
  StartEndSpansLatency(tracer, num_spans * 3 / 4, is_unique, latency_span_offset);
  run.join();
  err.join();
  // Increment offset for later
  latency_span_offset++;
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
    aggregator_ = std::unique_ptr<TracezDataAggregator>(new TracezDataAggregator(processor));

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
  void SetUp(const ::benchmark::State &state)
  {
    std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
    tracer_ = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
    aggregator_peer_ =
        std::unique_ptr<TracezDataAggregatorPeer>(new TracezDataAggregatorPeer(processor));
  }

  std::unique_ptr<TracezDataAggregatorPeer> aggregator_peer_;
  // Tracer for creating spans
  std::shared_ptr<opentelemetry::trace::Tracer> tracer_;
};

//////////////////////////// BENCHMARK DEFINITIONS /////////////////////////////////

/*
 * Aggregator handling many spans where minimal sorting of spans into different
 * latency bands is required, as all spans should be sorted in the same bucket.
 * It also means there's minimal memory usage for the Tracez data structure holding
 * sampled spans.
 */
BENCHMARK_DEFINE_F(TracezAggregator, BM_SingleBucket)(benchmark::State &state)
{
  const unsigned int num_spans  = state.range(0);
  const bool is_unique          = state.range(1);
  const bool run_periodic_query = state.range(2);

  if (run_periodic_query)
    aggregator_peer_->StartPeriodicQueryThread();

  for (auto _ : state)
  {
    // Do not time span creation, as we're only benchmarking aggregation work
    state.PauseTiming();
    StartEndSpans(tracer_, num_spans, is_unique);
    state.ResumeTiming();
    aggregator_peer_->Aggregate();
  }
}

/*
 * Aggregator handling many spans who may fall under error, running, and any
 * latency group. This requires the aggregator to sorting of spans into their
 * respective buckets (running, error, latency [including within latency bands]),
 * which also utilizes more memory.
 */
BENCHMARK_DEFINE_F(TracezAggregator, BM_ManyBuckets)(benchmark::State &state)
{
  const unsigned int num_spans  = state.range(0);
  const bool is_unique          = state.range(1);
  const bool run_periodic_query = state.range(2);

  // Ensure spans get added to each latency bucket
  unsigned int latency_index_offset = 0;

  if (run_periodic_query)
    aggregator_peer_->StartPeriodicQueryThread();

  for (auto _ : state)
  {
    state.PauseTiming();
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> running_spans =
        MakeManySpans(tracer_, num_spans, is_unique, latency_index_offset);
    state.ResumeTiming();
    aggregator_peer_->Aggregate();
  }
}

//////////////////////////// RUN BENCHMARKS ///////////////////////////////

/* Args: spans, is_unique, run_periodic_query
 *
 * @num_spans int number of spans created for each iteration
 * @is_unique bool whether spans are unique or not
 * @run_periodic_query bool whether to run a background periodic thread of not
 */

BENCHMARK_REGISTER_F(TracezAggregator, BM_SingleBucket)
    /*
     * Same name spans, with different number of spans and periodic query status
     *
     * Spans all have the same name, which means there's work to clear stored spans
     * from memory fairly often so that the number of sampled spans won't go over
     * the max and memory use is minimal, which are performance factors
     */
    ->Args({10, false, false})
    ->Args({10, false, true})
    ->Args({1000, false, false})
    ->Args({1000, false, true})
    /*
     * Many name spans, with different number of spans and periodic query status
     *
     * Spans have num_spans unique names, so many more spans are kept stored in memory
     * and not cleared as often, which affects performance
     */
    ->Args({10, true, false})
    ->Args({10, true, true})
    ->Args({1000, true, false})
    ->Args({1000, true, true});

// Do same permutation of number of spans, span uniqueness, and periodic thread
// running for many buckets too
BENCHMARK_REGISTER_F(TracezAggregator, BM_ManyBuckets)
    ->Args({10, false, false})
    ->Args({10, false, true})
    ->Args({1000, false, false})
    ->Args({1000, false, true})
    ->Args({10, true, false})
    ->Args({10, true, true})
    ->Args({1000, true, false})
    ->Args({1000, true, true});

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE

BENCHMARK_MAIN();
