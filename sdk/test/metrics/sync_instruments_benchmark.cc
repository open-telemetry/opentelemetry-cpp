// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off
//
// *******************************************************************************************************
// ABIv1 with preview options
//    ENABLE_METRICS_EXEMPLAR_PREVIEW = ON
//    OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW = OFF
// *******************************************************************************************************
// ~/build/sdk/test/metrics/sync_instruments_benchmark
// 2026-08-21T21:58:43+00:00
// Running /home/devuser/build/sdk/test/metrics/sync_instruments_benchmark
// Run on (32 X 800 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 6.57, 7.88, 6.02
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// -------------------------------------------------------------------------------------------------------
// Benchmark                                                             Time             CPU   Iterations
// -------------------------------------------------------------------------------------------------------
// BM_Record_Counter_Drop                                              168 ns          168 ns      3987426
// BM_Record_Counter_Sum_ByAttributes/0                               13.4 ns         13.4 ns     50673977
// BM_Record_Counter_Sum_ByAttributes/1                               59.1 ns         59.0 ns     11648365
// BM_Record_Counter_Sum_ByAttributes/10                               601 ns          600 ns      1152340
// BM_Record_Counter_Sum_ByAttributes/128                            12258 ns        12257 ns        54263
// BM_Record_Counter_Sum_ByCardinality/10                              162 ns          162 ns      4166286
// BM_Record_Counter_Sum_ByCardinality/500                             170 ns          170 ns      3989939
// BM_Record_Counter_Sum_ByCardinality/2000                            173 ns          173 ns      3909146
// BM_Record_Counter_Sum_ByCardinality/4000                            184 ns          184 ns      3747914
// BM_Record_Counter_Sum_ByThreads/threads:1                           162 ns          162 ns      4211788
// BM_Record_Counter_Sum_ByThreads/threads:2                           278 ns          278 ns      2294024
// BM_Record_Counter_Sum_ByThreads/threads:4                           645 ns          591 ns      1224916
// BM_Record_Histogram_Drop                                            168 ns          168 ns      4079689
// BM_Record_Histogram_ByAttributes/0                                 24.1 ns         24.1 ns     28174246
// BM_Record_Histogram_ByAttributes/1                                 68.4 ns         68.4 ns      9872132
// BM_Record_Histogram_ByAttributes/10                                 624 ns          624 ns      1101566
// BM_Record_Histogram_ByAttributes/128                              10144 ns        10143 ns        54976
// BM_Record_Histogram_ByCardinality/10                                171 ns          171 ns      4011809
// BM_Record_Histogram_ByCardinality/500                               183 ns          183 ns      3616072
// BM_Record_Histogram_ByCardinality/2000                              184 ns          184 ns      3634659
// BM_Record_Histogram_ByThreads/threads:1                             177 ns          177 ns      3928635
// BM_Record_Histogram_ByThreads/threads:2                             413 ns          412 ns      1611338
// BM_Record_Histogram_ByThreads/threads:4                             899 ns          796 ns       879956
// BM_Record_Histogram_Base2Expo_Drop                                  167 ns          167 ns      4089631
// BM_Record_Histogram_Base2Expo_ByAttributes/0                       30.5 ns         30.5 ns     26291186
// BM_Record_Histogram_Base2Expo_ByAttributes/1                       72.0 ns         72.0 ns      9186116
// BM_Record_Histogram_Base2Expo_ByAttributes/10                       628 ns          628 ns      1106815
// BM_Record_Histogram_Base2Expo_ByAttributes/128                    11121 ns        11119 ns        60648
// BM_Record_Histogram_Base2Expo_ByCardinality/10                      179 ns          179 ns      3966895
// BM_Record_Histogram_Base2Expo_ByCardinality/500                     192 ns          192 ns      3622430
// BM_Record_Histogram_Base2Expo_ByCardinality/2000                    191 ns          191 ns      3494182
// BM_Record_Histogram_Base2Expo_ByThreads/threads:1                   177 ns          177 ns      3928414
// BM_Record_Histogram_Base2Expo_ByThreads/threads:2                   379 ns          378 ns      1500150
// BM_Record_Histogram_Base2Expo_ByThreads/threads:4                   928 ns          811 ns       872660
// BM_Record_Histogram_ExemplarFilter_AlwaysOff                        180 ns          180 ns      3828794
// BM_Record_Histogram_ExemplarFilter_AlwaysOn                         316 ns          316 ns      2151816
// BM_Record_Histogram_Base2Expo_ExemplarFilter_AlwaysOff              183 ns          183 ns      3808523
// BM_Record_Histogram_Base2Expo_ExemplarFilter_AlwaysOn               318 ns          318 ns      2167809
// BM_Record_Counter_ExemplarFilter_AlwaysOff                          170 ns          170 ns      4071325
// BM_Record_Counter_ExemplarFilter_AlwaysOn                           317 ns          317 ns      2208892
// BM_Record_Counter_ExemplarFilter_TraceBased_EmptyContext            178 ns          178 ns      3914168
// BM_Record_Counter_ExemplarFilter_TraceBased_UnsampledContext        183 ns          183 ns      3775693
// BM_Record_Counter_ExemplarFilter_TraceBased_SampledContext          319 ns          319 ns      2185685
//
// *******************************************************************************************************
// ABIv2 with preview options
//    ENABLE_METRICS_EXEMPLAR_PREVIEW = ON
//    OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW = ON
// *******************************************************************************************************
// ~/build/sdk/test/metrics/sync_instruments_benchmark
// 2026-08-21T21:53:56+00:00
// Running /home/devuser/build/sdk/test/metrics/sync_instruments_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 3.47, 7.54, 5.41
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// -------------------------------------------------------------------------------------------------------
// Benchmark                                                             Time             CPU   Iterations
// -------------------------------------------------------------------------------------------------------
// BM_Record_Counter_Drop                                              272 ns          272 ns      2524674
// BM_Record_Counter_Sum_ByAttributes/0                               21.4 ns         21.4 ns     32408512
// BM_Record_Counter_Sum_ByAttributes/1                                101 ns          101 ns      6689829
// BM_Record_Counter_Sum_ByAttributes/10                               946 ns          946 ns       721512
// BM_Record_Counter_Sum_ByAttributes/128                            14831 ns        14828 ns        48382
// BM_Record_Counter_Sum_ByCardinality/10                              263 ns          263 ns      2642020
// BM_Record_Counter_Sum_ByCardinality/500                             276 ns          275 ns      2460887
// BM_Record_Counter_Sum_ByCardinality/2000                            280 ns          280 ns      2426883
// BM_Record_Counter_Sum_ByCardinality/4000                            267 ns          267 ns      2520303
// BM_Record_Counter_Sum_ByThreads/threads:1                           254 ns          254 ns      2693502
// BM_Record_Counter_Sum_ByThreads/threads:2                           629 ns          590 ns      1076912
// BM_Record_Counter_Sum_ByThreads/threads:4                          1378 ns         1132 ns       596732
// BM_Record_Histogram_Drop                                            272 ns          272 ns      2519934
// BM_Record_Histogram_ByAttributes/0                                 33.2 ns         33.2 ns     21084946
// BM_Record_Histogram_ByAttributes/1                                  109 ns          109 ns      6253275
// BM_Record_Histogram_ByAttributes/10                                1045 ns         1045 ns       663322
// BM_Record_Histogram_ByAttributes/128                              15644 ns        15642 ns        42846
// BM_Record_Histogram_ByCardinality/10                                289 ns          289 ns      2468624
// BM_Record_Histogram_ByCardinality/500                               295 ns          295 ns      2344321
// BM_Record_Histogram_ByCardinality/2000                              322 ns          322 ns      2256514
// BM_Record_Histogram_ByThreads/threads:1                             289 ns          289 ns      2418006
// BM_Record_Histogram_ByThreads/threads:2                             630 ns          620 ns      1038072
// BM_Record_Histogram_ByThreads/threads:4                            1476 ns         1182 ns       486216
// BM_Record_Histogram_Base2Expo_Drop                                  290 ns          290 ns      2356193
// BM_Record_Histogram_Base2Expo_ByAttributes/0                       34.0 ns         34.0 ns     21807161
// BM_Record_Histogram_Base2Expo_ByAttributes/1                        117 ns          117 ns      5768139
// BM_Record_Histogram_Base2Expo_ByAttributes/10                      1056 ns         1056 ns       654970
// BM_Record_Histogram_Base2Expo_ByAttributes/128                    18454 ns        18452 ns        38375
// BM_Record_Histogram_Base2Expo_ByCardinality/10                      283 ns          283 ns      2445878
// BM_Record_Histogram_Base2Expo_ByCardinality/500                     314 ns          314 ns      2270390
// BM_Record_Histogram_Base2Expo_ByCardinality/2000                    308 ns          308 ns      2261228
// BM_Record_Histogram_Base2Expo_ByThreads/threads:1                   282 ns          282 ns      2485837
// BM_Record_Histogram_Base2Expo_ByThreads/threads:2                   652 ns          639 ns      1047128
// BM_Record_Histogram_Base2Expo_ByThreads/threads:4                  1635 ns         1291 ns       396896
// BM_Record_Gauge_Drop                                                280 ns          280 ns      2488998
// BM_Record_Gauge_LastValue_ByAttributes/0                           34.1 ns         34.1 ns     20476626
// BM_Record_Gauge_LastValue_ByAttributes/1                            107 ns          107 ns      6383567
// BM_Record_Gauge_LastValue_ByAttributes/10                          1019 ns         1019 ns       666139
// BM_Record_Gauge_LastValue_ByAttributes/128                        15570 ns        15569 ns        43835
// BM_Record_Gauge_LastValue_ByCardinality/10                          262 ns          262 ns      2633975
// BM_Record_Gauge_LastValue_ByCardinality/500                         289 ns          289 ns      2423386
// BM_Record_Gauge_LastValue_ByCardinality/2000                        296 ns          296 ns      2329964
// BM_Record_Gauge_LastValue_ByThreads/threads:1                       270 ns          270 ns      2593040
// BM_Record_Gauge_LastValue_ByThreads/threads:2                       550 ns          539 ns      1285084
// BM_Record_Gauge_LastValue_ByThreads/threads:4                      1393 ns         1124 ns       599968
// BM_Record_Histogram_ExemplarFilter_AlwaysOff                        288 ns          288 ns      2380934
// BM_Record_Histogram_ExemplarFilter_AlwaysOn                         440 ns          440 ns      1540212
// BM_Record_Histogram_Base2Expo_ExemplarFilter_AlwaysOff              298 ns          298 ns      2378637
// BM_Record_Histogram_Base2Expo_ExemplarFilter_AlwaysOn               433 ns          433 ns      1572360
// BM_Record_Counter_ExemplarFilter_AlwaysOff                          288 ns          288 ns      2399458
// BM_Record_Counter_ExemplarFilter_AlwaysOn                           428 ns          428 ns      1606371
// BM_Record_Counter_ExemplarFilter_TraceBased_EmptyContext            286 ns          286 ns      2369366
// BM_Record_Counter_ExemplarFilter_TraceBased_UnsampledContext        294 ns          294 ns      2351982
// BM_Record_Counter_ExemplarFilter_TraceBased_SampledContext          428 ns          428 ns      1601929
// BM_Record_BoundCounter_Drop                                        5.61 ns         5.60 ns    120495562
// BM_Record_BoundCounter_Sum                                         9.90 ns         9.90 ns     67730700
// BM_Record_BoundHistogram_Explicit                                  12.0 ns         12.0 ns     56143923
// BM_Record_BoundHistogram_Base2Expo                                 17.3 ns         17.3 ns     33561116
// BM_Record_BoundCounter_Sum_ByThreads/threads:1                     10.3 ns         10.3 ns     67369178
// BM_Record_BoundCounter_Sum_ByThreads/threads:2                     20.2 ns         11.4 ns     62937932
// BM_Record_BoundCounter_Sum_ByThreads/threads:4                     74.5 ns         49.4 ns     29200628
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:1              11.4 ns         11.4 ns     58970940
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:2              23.4 ns         11.9 ns     59744092
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:4              38.6 ns         12.5 ns     51175520
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:1             16.7 ns         16.7 ns     41416382
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:2             61.8 ns         52.2 ns     20000000
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:4              134 ns         62.9 ns     12483604
//
// clang-format on

#include <benchmark/benchmark.h>

#include <chrono>
#include <cstdlib>
#include <map>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#include "opentelemetry/sdk/metrics/view/meter_selector.h"
#include "opentelemetry/sdk/metrics/view/view.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/resource/resource.h"

#include "opentelemetry/version.h"  // IWYU pragma: keep

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include <cstdint>
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/trace/context.h"
#  include "opentelemetry/trace/default_span.h"
#  include "opentelemetry/trace/span_context.h"
#  include "opentelemetry/trace/span_id.h"
#  include "opentelemetry/trace/trace_flags.h"
#  include "opentelemetry/trace/trace_id.h"
#endif

namespace metrics_sdk  = opentelemetry::sdk::metrics;
namespace resource_sdk = opentelemetry::sdk::resource;
namespace scope_sdk    = opentelemetry::sdk::instrumentationscope;

namespace
{

// ---------------------------------------------------------------------------
// Shared benchmark components

constexpr std::size_t kNominalCardinality    = 10;
constexpr std::size_t kNominalAttributeCount = 3;

using AttributeMap   = std::map<std::string, std::string>;
using AttributesView = opentelemetry::common::KeyValueIterableView<AttributeMap>;

class MockMetricExporter : public metrics_sdk::MetricReader
{
public:
  metrics_sdk::AggregationTemporality GetAggregationTemporality(
      metrics_sdk::InstrumentType) const noexcept override
  {
    return metrics_sdk::AggregationTemporality::kCumulative;
  }

private:
  bool OnForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool OnShutDown(std::chrono::microseconds) noexcept override { return true; }
  void OnInitialized() noexcept override {}
};

std::size_t GetBenchmarkThreads()
{
  const char *env = std::getenv("BENCHMARK_THREADS");
  if (env != nullptr && env[0] != '\0')
  {
    int val = static_cast<int>(std::strtol(env, nullptr, 10));
    if (val > 0)
    {
      return static_cast<std::size_t>(val);
    }
  }
  return 4;
}

// Create a set of attributes with a given count
static AttributeMap MakeAttributes(std::size_t count)
{
  AttributeMap attributes;
  for (std::size_t i = 0; i < count; i++)
  {
    attributes["attr_" + std::to_string(i)] = "value_" + std::to_string(i);
  }
  return attributes;
}

// Create attribute sets for a given cardinality target
static std::vector<AttributeMap> MakeAttributeSets(std::size_t cardinality)
{
  std::vector<AttributeMap> sets;
  sets.reserve(cardinality);
  for (std::size_t i = 0; i < cardinality; i++)
  {
    AttributeMap attrs;
    for (std::size_t k = 0; k < kNominalAttributeCount; k++)
    {
      attrs["key_" + std::to_string(k)] = std::to_string(i);
    }
    sets.push_back(std::move(attrs));
  }
  return sets;
}

struct BenchmarkProvider
{
  std::shared_ptr<metrics_sdk::MeterProvider> sdk_meter_provider;
  opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter> meter;

  BenchmarkProvider(bool meter_enabled = true)
  {
    auto view_registry = std::make_unique<metrics_sdk::ViewRegistry>();
    auto resource      = resource_sdk::Resource::Create({});

    auto default_config =
        meter_enabled ? metrics_sdk::MeterConfig::Default() : metrics_sdk::MeterConfig::Disabled();

    auto meter_configurator =
        std::make_unique<scope_sdk::ScopeConfigurator<metrics_sdk::MeterConfig>>(
            scope_sdk::ScopeConfigurator<metrics_sdk::MeterConfig>::Builder(default_config)
                .Build());

    sdk_meter_provider = std::make_shared<metrics_sdk::MeterProvider>(
        std::move(view_registry), resource, std::move(meter_configurator));
    sdk_meter_provider->AddMetricReader(std::make_shared<MockMetricExporter>());
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
    sdk_meter_provider->SetExemplarFilter(metrics_sdk::ExemplarFilterType::kAlwaysOff);
#endif
    meter = sdk_meter_provider->GetMeter("benchmark_meter");
  }

  void AddView(metrics_sdk::InstrumentType type,
               const std::string &name,
               metrics_sdk::AggregationType aggregation_type,
               const std::string &stream_name = "")
  {
    auto instrument_selector = std::make_unique<metrics_sdk::InstrumentSelector>(type, name, "");
    auto meter_selector = std::make_unique<metrics_sdk::MeterSelector>("benchmark_meter", "", "");

    std::unique_ptr<metrics_sdk::AggregationConfig> aggregation_config{nullptr};

    if (aggregation_type == metrics_sdk::AggregationType::kBase2ExponentialHistogram)
    {
      aggregation_config =
          std::make_unique<metrics_sdk::Base2ExponentialHistogramAggregationConfig>();
    }
    else if (aggregation_type == metrics_sdk::AggregationType::kHistogram)
    {
      aggregation_config = std::make_unique<metrics_sdk::HistogramAggregationConfig>();
    }

    auto view = std::make_unique<metrics_sdk::View>(stream_name, "", aggregation_type,
                                                    std::move(aggregation_config));
    sdk_meter_provider->AddView(std::move(instrument_selector), std::move(meter_selector),
                                std::move(view));
  }
};

// ---------------------------------------------------------------------------
// Counter benchmarks

void BM_Record_Counter_Disabled_ByThreads(benchmark::State &state)
{
  // Create a meter provider that disabled meters by default
  bool meter_enabled = false;
  static BenchmarkProvider provider(meter_enabled);
  static auto counter = provider.meter->CreateDoubleCounter("benchmark_counter");
  auto attribute_sets = MakeAttributeSets(kNominalCardinality);
  std::vector<AttributesView> attribute_views;
  attribute_views.reserve(attribute_sets.size());
  for (auto &kv_set : attribute_sets)
  {
    attribute_views.emplace_back(kv_set);
  }
  opentelemetry::context::Context context{};
  for (auto &kv_view : attribute_views)
  {
    counter->Add(1.0, kv_view, context);
  }
  std::size_t index = 0;

  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attribute_views[index++ % attribute_views.size()], context);
  }
}
BENCHMARK(BM_Record_Counter_Disabled_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Counter_Drop_ByThreads(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.AddView(metrics_sdk::InstrumentType::kCounter, "benchmark_counter",
                   metrics_sdk::AggregationType::kDrop);
  static auto counter    = provider.meter->CreateDoubleCounter("benchmark_counter");
  static auto attributes = MakeAttributes(kNominalAttributeCount);
  AttributesView attributes_view(attributes);
  opentelemetry::context::Context context{};
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Counter_Drop_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Counter_Sum_ByAttributes(benchmark::State &state)
{
  BenchmarkProvider provider;
  auto counter    = provider.meter->CreateDoubleCounter("benchmark_counter");
  auto attributes = MakeAttributes(state.range(0));
  AttributesView attributes_view(attributes);
  opentelemetry::context::Context context{};
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Counter_Sum_ByAttributes)->Arg(0)->Arg(1)->Arg(10)->Arg(128);

void BM_Record_Counter_Sum_ByCardinality(benchmark::State &state)
{
  BenchmarkProvider provider;
  auto counter        = provider.meter->CreateDoubleCounter("benchmark_counter");
  auto attribute_sets = MakeAttributeSets(state.range(0));
  std::vector<AttributesView> attribute_views;
  attribute_views.reserve(attribute_sets.size());
  for (auto &kv_set : attribute_sets)
  {
    attribute_views.emplace_back(kv_set);
  }
  opentelemetry::context::Context context{};
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attribute_views[index++ % attribute_views.size()], context);
  }
}
BENCHMARK(BM_Record_Counter_Sum_ByCardinality)
    ->Arg(10)
    ->Arg(500)
    ->Arg(2000)
    ->Arg(4000);  //< Overflow cardinality limits (default limit is 2000)

void BM_Record_Counter_Sum_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider;
  static auto counter = provider.meter->CreateDoubleCounter("benchmark_counter");
  auto attribute_sets = MakeAttributeSets(kNominalCardinality);
  std::vector<AttributesView> attribute_views;
  attribute_views.reserve(attribute_sets.size());
  for (auto &kv_set : attribute_sets)
  {
    attribute_views.emplace_back(kv_set);
  }
  opentelemetry::context::Context context{};
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attribute_views[index++ % attribute_views.size()], context);
  }
}
BENCHMARK(BM_Record_Counter_Sum_ByThreads)->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

// ---------------------------------------------------------------------------
// Histogram (Explicit Buckets) benchmarks

static std::vector<double> MakeRecordingValues()
{
  thread_local std::mt19937 rng(std::random_device{}());
  thread_local std::uniform_real_distribution<double> dist(1.0, 10000.0);
  std::vector<double> values(1024);
  for (auto &kv_view : values)
  {
    kv_view = dist(rng);
  }
  return values;
}

void BM_Record_Histogram_Disabled_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider(false);
  static auto histogram = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  auto attribute_sets   = MakeAttributeSets(kNominalCardinality);
  std::vector<AttributesView> attribute_views;
  attribute_views.reserve(attribute_sets.size());
  for (auto &kv_set : attribute_sets)
  {
    attribute_views.emplace_back(kv_set);
  }
  opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    const std::size_t i = index++;
    histogram->Record(values[i % values.size()], attribute_views[i % attribute_views.size()],
                      context);
  }
}
BENCHMARK(BM_Record_Histogram_Disabled_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Histogram_Drop_ByThreads(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.AddView(metrics_sdk::InstrumentType::kHistogram, "benchmark_histogram",
                   metrics_sdk::AggregationType::kDrop);
  static auto histogram  = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  static auto attributes = MakeAttributes(kNominalAttributeCount);
  AttributesView attributes_view(attributes);
  opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    histogram->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Histogram_Drop_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Histogram_Explicit_ByAttributes(benchmark::State &state)
{
  BenchmarkProvider provider;
  auto histogram  = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  auto attributes = MakeAttributes(state.range(0));
  AttributesView attributes_view(attributes);
  opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    histogram->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Histogram_Explicit_ByAttributes)->Arg(0)->Arg(1)->Arg(10)->Arg(128);

void BM_Record_Histogram_Explicit_ByCardinality(benchmark::State &state)
{
  BenchmarkProvider provider;
  auto histogram      = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  auto attribute_sets = MakeAttributeSets(state.range(0));
  std::vector<AttributesView> attribute_views;
  attribute_views.reserve(attribute_sets.size());
  for (auto &kv_set : attribute_sets)
  {
    attribute_views.emplace_back(kv_set);
  }
  opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    const std::size_t i = index++;
    histogram->Record(values[i % values.size()], attribute_views[i % attribute_views.size()],
                      context);
  }
}
BENCHMARK(BM_Record_Histogram_Explicit_ByCardinality)->Arg(10)->Arg(500)->Arg(2000);

void BM_Record_Histogram_Explicit_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider;
  static auto histogram = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  auto attribute_sets   = MakeAttributeSets(kNominalCardinality);
  std::vector<AttributesView> attribute_views;
  attribute_views.reserve(attribute_sets.size());
  for (auto &kv_set : attribute_sets)
  {
    attribute_views.emplace_back(kv_set);
  }
  opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    const std::size_t i = index++;
    histogram->Record(values[i % values.size()], attribute_views[i % attribute_views.size()],
                      context);
  }
}
BENCHMARK(BM_Record_Histogram_Explicit_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

// ---------------------------------------------------------------------------
// Histogram (Base2ExponentialHistogram) benchmarks

void BM_Record_Histogram_Base2Expo_ByAttributes(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.AddView(metrics_sdk::InstrumentType::kHistogram, "benchmark_histogram",
                   metrics_sdk::AggregationType::kBase2ExponentialHistogram);
  auto histogram  = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  auto attributes = MakeAttributes(state.range(0));
  AttributesView attributes_view(attributes);
  opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    histogram->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Histogram_Base2Expo_ByAttributes)->Arg(0)->Arg(1)->Arg(10)->Arg(128);

void BM_Record_Histogram_Base2Expo_ByCardinality(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.AddView(metrics_sdk::InstrumentType::kHistogram, "benchmark_histogram",
                   metrics_sdk::AggregationType::kBase2ExponentialHistogram);
  auto histogram      = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  auto attribute_sets = MakeAttributeSets(state.range(0));
  std::vector<AttributesView> attribute_views;
  attribute_views.reserve(attribute_sets.size());
  for (auto &kv_set : attribute_sets)
  {
    attribute_views.emplace_back(kv_set);
  }
  opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    const std::size_t i = index++;
    histogram->Record(values[i % values.size()], attribute_views[i % attribute_views.size()],
                      context);
  }
}
BENCHMARK(BM_Record_Histogram_Base2Expo_ByCardinality)->Arg(10)->Arg(500)->Arg(2000);

void BM_Record_Histogram_Base2Expo_ByThreads(benchmark::State &state)
{
  static auto provider = []() -> BenchmarkProvider {
    BenchmarkProvider p;
    p.AddView(metrics_sdk::InstrumentType::kHistogram, "benchmark_histogram",
              metrics_sdk::AggregationType::kBase2ExponentialHistogram);
    return p;
  }();
  static auto histogram = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  auto attribute_sets   = MakeAttributeSets(kNominalCardinality);
  std::vector<AttributesView> attribute_views;
  attribute_views.reserve(attribute_sets.size());
  for (auto &kv_set : attribute_sets)
  {
    attribute_views.emplace_back(kv_set);
  }
  opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    const std::size_t i = index++;
    histogram->Record(values[i % values.size()], attribute_views[i % attribute_views.size()],
                      context);
  }
}
BENCHMARK(BM_Record_Histogram_Base2Expo_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

// ---------------------------------------------------------------------------
// Gauge (LastValue) benchmarks

#if OPENTELEMETRY_ABI_VERSION_NO > 1

void BM_Record_Gauge_Disabled_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider(false);
  static auto gauge   = provider.meter->CreateDoubleGauge("benchmark_gauge");
  auto attribute_sets = MakeAttributeSets(kNominalCardinality);
  std::vector<AttributesView> attribute_views;
  attribute_views.reserve(attribute_sets.size());
  for (auto &kv_set : attribute_sets)
  {
    attribute_views.emplace_back(kv_set);
  }
  opentelemetry::context::Context context{};
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(gauge);
    gauge->Record(1.0, attribute_views[index++ % attribute_views.size()], context);
  }
}
BENCHMARK(BM_Record_Gauge_Disabled_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Gauge_Drop_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider = []() -> BenchmarkProvider {
    BenchmarkProvider p;
    p.AddView(metrics_sdk::InstrumentType::kGauge, "benchmark_gauge",
              metrics_sdk::AggregationType::kDrop);
    return p;
  }();
  static auto gauge = provider.meter->CreateDoubleGauge("benchmark_gauge");
  auto attributes   = MakeAttributes(kNominalAttributeCount);
  AttributesView attributes_view(attributes);
  opentelemetry::context::Context context{};
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(gauge);
    gauge->Record(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Gauge_Drop_ByThreads)->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Gauge_LastValue_ByAttributes(benchmark::State &state)
{
  BenchmarkProvider provider;
  auto gauge      = provider.meter->CreateDoubleGauge("benchmark_gauge");
  auto attributes = MakeAttributes(state.range(0));
  AttributesView attributes_view(attributes);
  opentelemetry::context::Context context{};
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(gauge);
    gauge->Record(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Gauge_LastValue_ByAttributes)->Arg(0)->Arg(1)->Arg(10)->Arg(128);

void BM_Record_Gauge_LastValue_ByCardinality(benchmark::State &state)
{
  BenchmarkProvider provider;
  auto gauge          = provider.meter->CreateDoubleGauge("benchmark_gauge");
  auto attribute_sets = MakeAttributeSets(state.range(0));
  std::vector<AttributesView> attribute_views;
  attribute_views.reserve(attribute_sets.size());
  for (auto &kv_set : attribute_sets)
  {
    attribute_views.emplace_back(kv_set);
  }
  opentelemetry::context::Context context{};
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(gauge);
    gauge->Record(1.0, attribute_views[index++ % attribute_views.size()], context);
  }
}
BENCHMARK(BM_Record_Gauge_LastValue_ByCardinality)->Arg(10)->Arg(500)->Arg(2000);

void BM_Record_Gauge_LastValue_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider;
  static auto gauge   = provider.meter->CreateDoubleGauge("benchmark_gauge");
  auto attribute_sets = MakeAttributeSets(kNominalCardinality);
  std::vector<AttributesView> attribute_views;
  attribute_views.reserve(attribute_sets.size());
  for (auto &kv_set : attribute_sets)
  {
    attribute_views.emplace_back(kv_set);
  }
  opentelemetry::context::Context context{};
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(gauge);
    gauge->Record(1.0, attribute_views[index++ % attribute_views.size()], context);
  }
}
BENCHMARK(BM_Record_Gauge_LastValue_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

#endif  // OPENTELEMETRY_ABI_VERSION_NO > 1

// ---------------------------------------------------------------------------
// Exemplar filter benchmarks

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

class RecordingSpan : public opentelemetry::trace::DefaultSpan
{
public:
  RecordingSpan(opentelemetry::trace::TraceFlags flags) : DefaultSpan(MakeSpanContext(flags)) {}
  bool IsRecording() const noexcept override { return true; }

private:
  static opentelemetry::trace::SpanContext MakeSpanContext(opentelemetry::trace::TraceFlags flags)
  {
    static const uint8_t kTraceBytes[16] = {1};
    static const uint8_t kSpanBytes[8]   = {1};
    return opentelemetry::trace::SpanContext{
        opentelemetry::trace::TraceId{
            opentelemetry::nostd::span<const uint8_t, 16>{kTraceBytes, 16}},
        opentelemetry::trace::SpanId{opentelemetry::nostd::span<const uint8_t, 8>{kSpanBytes, 8}},
        flags, false};
  }
};

static opentelemetry::context::Context MakeContext(opentelemetry::trace::TraceFlags flags)
{
  auto span    = std::make_shared<RecordingSpan>(flags);
  auto context = opentelemetry::context::Context{};
  return opentelemetry::trace::SetSpan(context, {span});
}

void BM_Record_Counter_Sum_Exemplar_AlwaysOff(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.sdk_meter_provider->SetExemplarFilter(metrics_sdk::ExemplarFilterType::kAlwaysOff);
  auto counter                  = provider.meter->CreateDoubleCounter("benchmark_counter");
  const AttributeMap attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  auto context =
      MakeContext(opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled});
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Counter_Sum_Exemplar_AlwaysOff);

void BM_Record_Counter_Sum_Exemplar_AlwaysOn(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.sdk_meter_provider->SetExemplarFilter(metrics_sdk::ExemplarFilterType::kAlwaysOn);
  auto counter                  = provider.meter->CreateDoubleCounter("benchmark_counter");
  const AttributeMap attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  auto context =
      MakeContext(opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled});
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Counter_Sum_Exemplar_AlwaysOn);

void BM_Record_Counter_Sum_Exemplar_TraceBased_EmptyContext(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.sdk_meter_provider->SetExemplarFilter(metrics_sdk::ExemplarFilterType::kTraceBased);
  auto counter                  = provider.meter->CreateDoubleCounter("benchmark_counter");
  const AttributeMap attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  opentelemetry::context::Context context{};
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Counter_Sum_Exemplar_TraceBased_EmptyContext);

void BM_Record_Counter_Sum_Exemplar_TraceBased_UnsampledContext(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.sdk_meter_provider->SetExemplarFilter(metrics_sdk::ExemplarFilterType::kTraceBased);
  auto counter                  = provider.meter->CreateDoubleCounter("benchmark_counter");
  const AttributeMap attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  auto context = MakeContext(opentelemetry::trace::TraceFlags{});
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Counter_Sum_Exemplar_TraceBased_UnsampledContext);

void BM_Record_Counter_Sum_Exemplar_TraceBased_SampledContext(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.sdk_meter_provider->SetExemplarFilter(metrics_sdk::ExemplarFilterType::kTraceBased);
  auto counter                  = provider.meter->CreateDoubleCounter("benchmark_counter");
  const AttributeMap attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  auto context =
      MakeContext(opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled});
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Counter_Sum_Exemplar_TraceBased_SampledContext);

void BM_Record_Histogram_Explicit_Exemplar_AlwaysOff(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.sdk_meter_provider->SetExemplarFilter(metrics_sdk::ExemplarFilterType::kAlwaysOff);
  auto histogram                = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  const AttributeMap attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  auto context =
      MakeContext(opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled});
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    histogram->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Histogram_Explicit_Exemplar_AlwaysOff);

void BM_Record_Histogram_Explicit_Exemplar_AlwaysOn(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.sdk_meter_provider->SetExemplarFilter(metrics_sdk::ExemplarFilterType::kAlwaysOn);
  auto histogram                = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  const AttributeMap attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  auto context =
      MakeContext(opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled});
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    histogram->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Histogram_Explicit_Exemplar_AlwaysOn);

void BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOff(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.AddView(metrics_sdk::InstrumentType::kHistogram, "benchmark_histogram",
                   metrics_sdk::AggregationType::kBase2ExponentialHistogram);
  provider.sdk_meter_provider->SetExemplarFilter(metrics_sdk::ExemplarFilterType::kAlwaysOff);
  auto histogram                = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  const AttributeMap attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  auto context =
      MakeContext(opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled});
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    histogram->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOff);

void BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOn(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.AddView(metrics_sdk::InstrumentType::kHistogram, "benchmark_histogram",
                   metrics_sdk::AggregationType::kBase2ExponentialHistogram);
  provider.sdk_meter_provider->SetExemplarFilter(metrics_sdk::ExemplarFilterType::kAlwaysOn);
  auto histogram                = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  const AttributeMap attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  auto context =
      MakeContext(opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled});
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    histogram->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOn);

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW

// ---------------------------------------------------------------------------
// Bound instrument benchmarks

#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW

void BM_Record_BoundCounter_Disabled_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider(false);
  static auto counter    = provider.meter->CreateDoubleCounter("counter_bound_disabled");
  static auto attributes = MakeAttributes(kNominalAttributeCount);
  static auto bound      = counter->Bind(AttributesView(attributes));
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(bound);
    bound->Add(1.0);
  }
}
BENCHMARK(BM_Record_BoundCounter_Disabled_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_BoundCounter_Drop_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider = []() -> BenchmarkProvider {
    BenchmarkProvider p;
    p.AddView(metrics_sdk::InstrumentType::kCounter, "counter_bound_drop",
              metrics_sdk::AggregationType::kDrop);
    return p;
  }();
  static auto counter    = provider.meter->CreateDoubleCounter("counter_bound_drop");
  static auto attributes = MakeAttributes(kNominalAttributeCount);
  static auto bound      = counter->Bind(AttributesView(attributes));
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(bound);
    bound->Add(1.0);
  }
}
BENCHMARK(BM_Record_BoundCounter_Drop_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_BoundCounter_Sum_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider;
  static auto counter    = provider.meter->CreateDoubleCounter("counter_bound_shared");
  static auto attributes = MakeAttributes(kNominalAttributeCount);
  static auto bound      = counter->Bind(AttributesView(attributes));
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(bound);
    bound->Add(1.0);
  }
}
BENCHMARK(BM_Record_BoundCounter_Sum_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_BoundHistogram_Disabled_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider(false);
  static auto histogram  = provider.meter->CreateDoubleHistogram("histogram_bound_disabled");
  static auto attributes = MakeAttributes(kNominalAttributeCount);
  static auto bound      = histogram->Bind(AttributesView(attributes));
  const auto values      = MakeRecordingValues();
  std::size_t index      = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(bound);
    bound->Record(values[index++ % values.size()]);
  }
}
BENCHMARK(BM_Record_BoundHistogram_Disabled_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_BoundHistogram_Drop_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider = []() -> BenchmarkProvider {
    BenchmarkProvider p;
    p.AddView(metrics_sdk::InstrumentType::kHistogram, "histogram_bound_drop",
              metrics_sdk::AggregationType::kDrop);
    return p;
  }();
  static auto histogram  = provider.meter->CreateDoubleHistogram("histogram_bound_drop");
  static auto attributes = MakeAttributes(kNominalAttributeCount);
  static auto bound      = histogram->Bind(AttributesView(attributes));
  const auto values      = MakeRecordingValues();
  std::size_t index      = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(bound);
    bound->Record(values[index++ % values.size()]);
  }
}
BENCHMARK(BM_Record_BoundHistogram_Drop_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_BoundHistogram_Explicit_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider;
  static auto histogram  = provider.meter->CreateDoubleHistogram("histogram_bound_shared");
  static auto attributes = MakeAttributes(kNominalAttributeCount);
  static auto bound      = histogram->Bind(AttributesView(attributes));
  const auto values      = MakeRecordingValues();
  std::size_t index      = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(bound);
    bound->Record(values[index++ % values.size()]);
  }
}
BENCHMARK(BM_Record_BoundHistogram_Explicit_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_BoundHistogram_Base2Expo_ByThreads(benchmark::State &state)
{
  static auto provider = []() -> BenchmarkProvider {
    BenchmarkProvider p;
    p.AddView(metrics_sdk::InstrumentType::kHistogram, "histogram_bound_expo_shared",
              metrics_sdk::AggregationType::kBase2ExponentialHistogram);
    return p;
  }();
  static auto histogram  = provider.meter->CreateDoubleHistogram("histogram_bound_expo_shared");
  static auto attributes = MakeAttributes(kNominalAttributeCount);
  static auto bound      = histogram->Bind(AttributesView(attributes));
  const auto values      = MakeRecordingValues();
  std::size_t index      = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(bound);
    bound->Record(values[index++ % values.size()]);
  }
}
BENCHMARK(BM_Record_BoundHistogram_Base2Expo_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

#endif  // OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW

}  // namespace

BENCHMARK_MAIN();
