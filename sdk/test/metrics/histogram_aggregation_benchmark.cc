// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <benchmark/benchmark.h>
#include <stddef.h>
#include <stdint.h>
#include <chrono>
#include <cmath>
#include <functional>
#include <initializer_list>  // IWYU pragma: keep
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "common.h"

#include "opentelemetry/context/context.h"  // IWYU pragma: keep
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/aggregation/base2_exponential_histogram_aggregation.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#include "opentelemetry/sdk/metrics/view/meter_selector.h"
#include "opentelemetry/sdk/metrics/view/view.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::metrics;

namespace
{

template <class T>
void HistogramAggregation(benchmark::State &state, std::unique_ptr<ViewRegistry> views)
{
  MeterProvider mp(std::move(views));
  auto m = mp.GetMeter("meter1", "version1", "schema1");

  std::unique_ptr<MockMetricExporter> exporter(new MockMetricExporter());
  std::shared_ptr<MetricReader> reader{new MockMetricReader(std::move(exporter))};
  mp.AddMetricReader(reader);
  auto h = m->CreateDoubleHistogram("histogram1", "histogram1_description", "histogram1_unit");
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0, 1000000);
  // Generate 100000 measurements
  constexpr size_t TOTAL_MEASUREMENTS = 100000;
  double measurements[TOTAL_MEASUREMENTS];
  for (size_t i = 0; i < TOTAL_MEASUREMENTS; i++)
  {
    measurements[i] = static_cast<double>(distribution(generator));
  }
  std::vector<T> actuals;
  std::vector<std::thread> collectionThreads;
  std::function<void()> collectMetrics = [&reader, &actuals]() {
    reader->Collect([&](ResourceMetrics &rm) {
      for (const ScopeMetrics &smd : rm.scope_metric_data_)
      {
        for (const MetricData &md : smd.metric_data_)
        {
          for (const PointDataAttributes &dp : md.point_data_attr_)
          {
            actuals.push_back(opentelemetry::nostd::get<T>(dp.point_data));
          }
        }
      }
      return true;
    });
  };

  while (state.KeepRunningBatch(TOTAL_MEASUREMENTS))
  {
    for (size_t i = 0; i < TOTAL_MEASUREMENTS; i++)
    {
      h->Record(measurements[i], {});
      if (i % 1000 == 0 || i == TOTAL_MEASUREMENTS - 1)
      {
        collectMetrics();
      }
      if (i == 100)
      {
        std::this_thread::sleep_for(std::chrono::nanoseconds(4));
      }
    }
  }
}

void BM_HistogramAggregation(benchmark::State &state)
{
  std::unique_ptr<ViewRegistry> views{new ViewRegistry()};
  HistogramAggregation<HistogramPointData>(state, std::move(views));
}

BENCHMARK(BM_HistogramAggregation);

// Add this helper function before your benchmark functions

std::unique_ptr<ViewRegistry> MakeBase2ExponentialViewRegistry(int32_t max_scale)
{
  std::string instrument_unit = "histogram1_unit";
  std::unique_ptr<InstrumentSelector> histogram_instrument_selector{
      new InstrumentSelector(InstrumentType::kHistogram, ".*", instrument_unit)};
  std::unique_ptr<MeterSelector> histogram_meter_selector{
      new MeterSelector("meter1", "version1", "schema1")};

  Base2ExponentialHistogramAggregationConfig config;
  config.max_scale_ = max_scale;

  std::unique_ptr<View> histogram_view{
      new View("base2_expohisto", "description", AggregationType::kBase2ExponentialHistogram,
               std::make_shared<Base2ExponentialHistogramAggregationConfig>(config))};

  std::unique_ptr<ViewRegistry> views{new ViewRegistry()};
  views->AddView(std::move(histogram_instrument_selector), std::move(histogram_meter_selector),
                 std::move(histogram_view));
  return views;
}

void RunBase2ExponentialHistogramAggregation(benchmark::State &state, int32_t scale)
{
  HistogramAggregation<Base2ExponentialHistogramPointData>(state,
                                                           MakeBase2ExponentialViewRegistry(scale));
}

void BM_Base2ExponentialHistogramAggregationZeroScale(benchmark::State &state)
{
  RunBase2ExponentialHistogramAggregation(state, 0);
}
BENCHMARK(BM_Base2ExponentialHistogramAggregationZeroScale);

void BM_Base2ExponentialHistogramAggregationOneScale(benchmark::State &state)
{
  RunBase2ExponentialHistogramAggregation(state, 1);
}
BENCHMARK(BM_Base2ExponentialHistogramAggregationOneScale);

void BM_Base2ExponentialHistogramAggregationTwoScale(benchmark::State &state)
{
  RunBase2ExponentialHistogramAggregation(state, 2);
}
BENCHMARK(BM_Base2ExponentialHistogramAggregationTwoScale);

void BM_Base2ExponentialHistogramAggregationFourScale(benchmark::State &state)
{
  RunBase2ExponentialHistogramAggregation(state, 4);
}
BENCHMARK(BM_Base2ExponentialHistogramAggregationFourScale);

void BM_Base2ExponentialHistogramAggregationEightScale(benchmark::State &state)
{
  RunBase2ExponentialHistogramAggregation(state, 8);
}
BENCHMARK(BM_Base2ExponentialHistogramAggregationEightScale);

void BM_Base2ExponentialHistogramAggregationSixteenScale(benchmark::State &state)
{
  RunBase2ExponentialHistogramAggregation(state, 16);
}
BENCHMARK(BM_Base2ExponentialHistogramAggregationSixteenScale);

// ---------------------------------------------------------------------------
// Multi-threaded aggregation throughput for the base2 exponential histogram.
// See https://github.com/open-telemetry/opentelemetry-cpp/issues/3366.
//
// These benchmarks deliberately do not follow the shape of
// HistogramAggregation() above: measurement generation, thread management,
// collection and sleeps all stay out of the timed region, so the reported time
// covers aggregation only. Concurrency is expressed with the Google Benchmark
// thread support, which starts and joins the workers outside of the measured
// interval and reports wall-clock time for the parallel region.
// ---------------------------------------------------------------------------

constexpr int32_t kBase2MaxScale        = 20;
constexpr size_t kBase2MeasurementCount = 4096;

Base2ExponentialHistogramAggregationConfig MakeBase2Config(size_t max_size, int32_t max_scale)
{
  Base2ExponentialHistogramAggregationConfig config;
  config.max_size_  = max_size;
  config.max_scale_ = max_scale;
  return config;
}

// Values spread over roughly 240 binary orders of magnitude. The uniform
// distribution used by BM_HistogramAggregation covers a single bucket range, so
// once the aggregation has adapted its scale the recorded indices never leave
// the window again and the bucket layout is barely exercised.
std::vector<double> MakeWideRangeMeasurements(size_t count)
{
  std::mt19937 generator(1729);
  std::uniform_real_distribution<double> exponent(-120.0, 120.0);
  std::vector<double> measurements;
  measurements.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    measurements.push_back(std::exp2(exponent(generator)));
  }
  return measurements;
}

const std::vector<double> &WideRangeMeasurements()
{
  static const std::vector<double> measurements = MakeWideRangeMeasurements(kBase2MeasurementCount);
  return measurements;
}

// One measurement per bucket at the configured max scale, offset by half a
// bucket to stay away from the boundaries where ComputeIndex() is documented to
// be inaccurate for positive scales. The first max_size values fill the
// circular buffer, and value 2 * max_size forces the recorded index range past
// max_size a second time, so a full buffer is folded twice per aggregation.
//
// A fresh aggregation is required for that: a scale can only ever decrease, so
// a long-lived aggregation stops downscaling once it has adapted to the input.
std::vector<double> MakeDenseBucketMeasurements(size_t max_size, int32_t max_scale)
{
  const double index_step = std::exp2(-static_cast<double>(max_scale));
  const size_t count      = 2 * max_size + 1;
  std::vector<double> measurements;
  measurements.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    measurements.push_back(std::exp2((static_cast<double>(i) + 0.5) * index_step));
  }
  return measurements;
}

// Number of scales the input actually consumes, so the benchmark output shows
// that downscaling is being exercised rather than assumed.
double ObservedScaleReduction(const std::vector<double> &measurements,
                              const Base2ExponentialHistogramAggregationConfig &config)
{
  Base2ExponentialHistogramAggregation aggregation(&config);
  const PointAttributes attributes;
  for (double value : measurements)
  {
    aggregation.Aggregate(value, attributes);
  }
  const PointType point  = aggregation.ToPoint();
  const auto &point_data = opentelemetry::nostd::get<Base2ExponentialHistogramPointData>(point);
  return static_cast<double>(config.max_scale_ - point_data.scale_);
}

// Each thread aggregates into its own aggregation, so no metric storage lock,
// attribute hashmap or collection is involved. Anything short of linear scaling
// comes from the aggregation itself.
void BM_Base2ExponentialHistogramAggregate(benchmark::State &state)
{
  const Base2ExponentialHistogramAggregationConfig config =
      MakeBase2Config(static_cast<size_t>(state.range(0)), kBase2MaxScale);
  const std::vector<double> &measurements = WideRangeMeasurements();
  const PointAttributes attributes;
  Base2ExponentialHistogramAggregation aggregation(&config);

  for (auto _ : state)
  {
    for (double value : measurements)
    {
      aggregation.Aggregate(value, attributes);
    }
  }
  state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(measurements.size()));
}
BENCHMARK(BM_Base2ExponentialHistogramAggregate)->Arg(160)->ThreadRange(1, 8);

// Downscaling-dominated counterpart of the benchmark above: every iteration
// folds a completely full bucket buffer twice. This is the workload that shows
// the cost of the buffer that DownscaleBuckets() allocates per downscale, and
// the added threads show how much of that cost is allocator contention.
void BM_Base2ExponentialHistogramDownscale(benchmark::State &state)
{
  const size_t max_size = static_cast<size_t>(state.range(0));
  const Base2ExponentialHistogramAggregationConfig config =
      MakeBase2Config(max_size, kBase2MaxScale);
  const std::vector<double> measurements = MakeDenseBucketMeasurements(max_size, kBase2MaxScale);
  const PointAttributes attributes;

  for (auto _ : state)
  {
    Base2ExponentialHistogramAggregation aggregation(&config);
    for (double value : measurements)
    {
      aggregation.Aggregate(value, attributes);
    }
  }
  state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(measurements.size()));
  state.counters["scale_reduction"] = benchmark::Counter(
      ObservedScaleReduction(measurements, config), benchmark::Counter::kAvgThreads);
}
BENCHMARK(BM_Base2ExponentialHistogramDownscale)->Arg(20)->Arg(160)->Arg(640)->ThreadRange(1, 8);

// All threads record on one shared instrument, so every measurement contends on
// the same SyncMetricStorage entry. Same scenario as
// BM_MeasurementsThreadsShareCounterTest in measurements_benchmark.cc, but the
// provider is built outside the timed region and nothing is collected while the
// timer runs.
class SharedBase2InstrumentFixture : public benchmark::Fixture
{
public:
  using benchmark::Fixture::SetUp;
  using benchmark::Fixture::TearDown;

  // Google Benchmark synchronizes every thread before the first iteration and
  // after the last one, so building and destroying the shared state on thread 0
  // is safe as long as the other threads only touch it inside the loop.
  void SetUp(benchmark::State &state) override
  {
    if (state.thread_index() != 0)
    {
      return;
    }
    provider_ = std::make_unique<MeterProvider>(MakeBase2ExponentialViewRegistry(kBase2MaxScale));
    std::unique_ptr<MockMetricExporter> exporter(new MockMetricExporter());
    reader_ = std::shared_ptr<MetricReader>(new MockMetricReader(std::move(exporter)));
    provider_->AddMetricReader(reader_);
    meter_ = provider_->GetMeter("meter1", "version1", "schema1");
    histogram_ =
        meter_->CreateDoubleHistogram("histogram1", "histogram1_description", "histogram1_unit");
  }

  void TearDown(benchmark::State &state) override
  {
    if (state.thread_index() != 0)
    {
      return;
    }
    histogram_.reset();
    meter_ = opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter>();
    reader_.reset();
    provider_.reset();
  }

protected:
  std::unique_ptr<MeterProvider> provider_;
  std::shared_ptr<MetricReader> reader_;
  opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter> meter_;
  opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Histogram<double>> histogram_;
};

BENCHMARK_DEFINE_F(SharedBase2InstrumentFixture, Record)(benchmark::State &state)
{
  const std::vector<double> &measurements = WideRangeMeasurements();
  // Start each thread at a different offset so they do not walk the buffer in
  // lockstep, which would understate the cost of a shared bucket layout.
  size_t index = static_cast<size_t>(state.thread_index()) * 64;

  for (auto _ : state)
  {
    histogram_->Record(measurements[index % measurements.size()], {});
    ++index;
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK_REGISTER_F(SharedBase2InstrumentFixture, Record)->ThreadRange(1, 8);

}  // namespace
BENCHMARK_MAIN();
