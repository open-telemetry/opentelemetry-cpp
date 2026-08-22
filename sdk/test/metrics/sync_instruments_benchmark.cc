// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// Benchmarks for the synchronous instrument recording path including:
// Counter, Histogram, and Gauge (ABI v2) and supported Bound variants.
//
// Each instrument type is measured across the following scenarios:
//   - Record values with 1-4 threads contending on the same instrument with nominal attributes and
//   the following variations:
//        - Meter disabled
//        - Drop aggregation
//        - Value Aggregation (Sum for Counter, LastValue for Gauge, Explicit Buckets and Base2
//        Exponential for Histogram)
//   - Record values with a range of attributes from 0 to 128
//   - Record values with a range of a nominal attribute count that creates cardinality from 10 to
//   4000 (default limit is 2000)
//   - Record values with ExemplarFilter set to AlwaysOff, AlwaysOn, and TraceBased.
//   - Record values with bound instrument variants (ABI v2 preview)
//
// clang-format off
//
// Results:
// *******************************************************************************************************
// ABIv1 with preview options
//    ENABLE_METRICS_EXEMPLAR_PREVIEW = ON
//    OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW = OFF
// -------------------------------------------------------------------------------------------------------
//  ~/build/sdk/test/metrics/sync_instruments_benchmark
// 2026-08-22T18:29:21+00:00
// Running /home/devuser/build/sdk/test/metrics/sync_instruments_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 0.31, 1.59, 6.96
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// -----------------------------------------------------------------------------------------------------
// Benchmark                                                           Time             CPU   Iterations
// -----------------------------------------------------------------------------------------------------
// BM_Record_Counter_Disabled_ByThreads/threads:1                  0.240 ns        0.240 ns   2810278222
// BM_Record_Counter_Disabled_ByThreads/threads:2                  0.251 ns        0.251 ns   2657675522
// BM_Record_Counter_Disabled_ByThreads/threads:4                  0.249 ns        0.249 ns   2217318376
// BM_Record_Counter_Drop_ByThreads/threads:1                        172 ns          172 ns      4061768
// BM_Record_Counter_Drop_ByThreads/threads:2                        199 ns          199 ns      3502520
// BM_Record_Counter_Drop_ByThreads/threads:4                        480 ns          451 ns      1460396
// BM_Record_Counter_Sum_ByAttributes/0                             21.3 ns         21.3 ns     32952539
// BM_Record_Counter_Sum_ByAttributes/1                             58.9 ns         58.9 ns     11783391
// BM_Record_Counter_Sum_ByAttributes/10                             622 ns          621 ns      1147270
// BM_Record_Counter_Sum_ByAttributes/128                          10463 ns        10461 ns        61934
// BM_Record_Counter_Sum_ByCardinality/10                            167 ns          167 ns      4229155
// BM_Record_Counter_Sum_ByCardinality/500                           182 ns          182 ns      3887130
// BM_Record_Counter_Sum_ByCardinality/2000                          184 ns          184 ns      3895325
// BM_Record_Counter_Sum_ByCardinality/4000                          188 ns          188 ns      3684246
// BM_Record_Counter_Sum_ByThreads/threads:1                         165 ns          165 ns      4253673
// BM_Record_Counter_Sum_ByThreads/threads:2                         306 ns          306 ns      2191990
// BM_Record_Counter_Sum_ByThreads/threads:4                         776 ns          695 ns      1046688
// BM_Record_Histogram_Disabled_ByThreads/threads:1                0.211 ns        0.211 ns   3317925441
// BM_Record_Histogram_Disabled_ByThreads/threads:2                0.220 ns        0.220 ns   2448391836
// BM_Record_Histogram_Disabled_ByThreads/threads:4                0.215 ns        0.215 ns   2565877976
// BM_Record_Histogram_Drop_ByThreads/threads:1                      168 ns          168 ns      4025374
// BM_Record_Histogram_Drop_ByThreads/threads:2                      183 ns          183 ns      3715406
// BM_Record_Histogram_Drop_ByThreads/threads:4                      386 ns          374 ns      1466744
// BM_Record_Histogram_Explicit_ByAttributes/0                      24.7 ns         24.7 ns     27963357
// BM_Record_Histogram_Explicit_ByAttributes/1                      68.2 ns         68.2 ns      9980062
// BM_Record_Histogram_Explicit_ByAttributes/10                      615 ns          615 ns      1116173
// BM_Record_Histogram_Explicit_ByAttributes/128                   10998 ns        10996 ns        60390
// BM_Record_Histogram_Explicit_ByCardinality/10                     172 ns          172 ns      3897194
// BM_Record_Histogram_Explicit_ByCardinality/500                    186 ns          186 ns      3635122
// BM_Record_Histogram_Explicit_ByCardinality/2000                   189 ns          189 ns      3714421
// BM_Record_Histogram_Explicit_ByThreads/threads:1                  171 ns          171 ns      4044930
// BM_Record_Histogram_Explicit_ByThreads/threads:2                  386 ns          384 ns      1623758
// BM_Record_Histogram_Explicit_ByThreads/threads:4                  895 ns          791 ns       877804
// BM_Record_Histogram_Base2Expo_ByAttributes/0                     30.4 ns         30.4 ns     25880391
// BM_Record_Histogram_Base2Expo_ByAttributes/1                     70.5 ns         70.4 ns      9238204
// BM_Record_Histogram_Base2Expo_ByAttributes/10                     634 ns          634 ns      1091161
// BM_Record_Histogram_Base2Expo_ByAttributes/128                  11322 ns        11321 ns        59828
// BM_Record_Histogram_Base2Expo_ByCardinality/10                    188 ns          188 ns      3823091
// BM_Record_Histogram_Base2Expo_ByCardinality/500                   197 ns          197 ns      3442858
// BM_Record_Histogram_Base2Expo_ByCardinality/2000                  198 ns          198 ns      3510467
// BM_Record_Histogram_Base2Expo_ByThreads/threads:1                 187 ns          187 ns      3738470
// BM_Record_Histogram_Base2Expo_ByThreads/threads:2                 372 ns          372 ns      1751874
// BM_Record_Histogram_Base2Expo_ByThreads/threads:4                 846 ns          756 ns       900708
// BM_Record_Counter_Sum_Exemplar_AlwaysOff                          172 ns          172 ns      4053919
// BM_Record_Counter_Sum_Exemplar_AlwaysOn                           311 ns          311 ns      2222684
// BM_Record_Counter_Sum_Exemplar_TraceBased_EmptyContext            178 ns          178 ns      3918620
// BM_Record_Counter_Sum_Exemplar_TraceBased_UnsampledContext        181 ns          181 ns      3802845
// BM_Record_Counter_Sum_Exemplar_TraceBased_SampledContext          323 ns          323 ns      2180391
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOff                   182 ns          182 ns      3842404
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOn                    322 ns          322 ns      2148460
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOff                  191 ns          191 ns      3660302
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOn                   326 ns          326 ns      2160708
//
// *******************************************************************************************************
// ABIv2 with preview options
//    ENABLE_METRICS_EXEMPLAR_PREVIEW = ON
//    OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW = ON
// -------------------------------------------------------------------------------------------------------
// ~/build/sdk/test/metrics/sync_instruments_benchmark
// 2026-08-22T18:35:28+00:00
// Running /home/devuser/build/sdk/test/metrics/sync_instruments_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 1.26, 3.33, 6.24
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// -----------------------------------------------------------------------------------------------------
// Benchmark                                                           Time             CPU   Iterations
// -----------------------------------------------------------------------------------------------------
// BM_Record_Counter_Disabled_ByThreads/threads:1                  0.244 ns        0.244 ns   2854410569
// BM_Record_Counter_Disabled_ByThreads/threads:2                  0.246 ns        0.246 ns   2283314028
// BM_Record_Counter_Disabled_ByThreads/threads:4                  0.259 ns        0.259 ns   2767156460
// BM_Record_Counter_Drop_ByThreads/threads:1                        280 ns          280 ns      2500556
// BM_Record_Counter_Drop_ByThreads/threads:2                        504 ns          502 ns      1230074
// BM_Record_Counter_Drop_ByThreads/threads:4                       1188 ns         1005 ns       693584
// BM_Record_Counter_Sum_ByAttributes/0                             25.9 ns         25.9 ns     26993356
// BM_Record_Counter_Sum_ByAttributes/1                              102 ns          102 ns      6857076
// BM_Record_Counter_Sum_ByAttributes/10                            1047 ns         1047 ns       673706
// BM_Record_Counter_Sum_ByAttributes/128                          18473 ns        18472 ns        40446
// BM_Record_Counter_Sum_ByCardinality/10                            264 ns          264 ns      2605094
// BM_Record_Counter_Sum_ByCardinality/500                           283 ns          282 ns      2415468
// BM_Record_Counter_Sum_ByCardinality/2000                          288 ns          288 ns      2405574
// BM_Record_Counter_Sum_ByCardinality/4000                          284 ns          284 ns      2464235
// BM_Record_Counter_Sum_ByThreads/threads:1                         267 ns          267 ns      2619454
// BM_Record_Counter_Sum_ByThreads/threads:2                         545 ns          540 ns      1119670
// BM_Record_Counter_Sum_ByThreads/threads:4                        1382 ns         1143 ns       600404
// BM_Record_Histogram_Disabled_ByThreads/threads:1                0.230 ns        0.230 ns   3061468421
// BM_Record_Histogram_Disabled_ByThreads/threads:2                0.226 ns        0.226 ns   2510237466
// BM_Record_Histogram_Disabled_ByThreads/threads:4                0.227 ns        0.227 ns   2325233576
// BM_Record_Histogram_Drop_ByThreads/threads:1                      282 ns          282 ns      2483406
// BM_Record_Histogram_Drop_ByThreads/threads:2                      518 ns          513 ns      1299874
// BM_Record_Histogram_Drop_ByThreads/threads:4                     1163 ns          986 ns       722172
// BM_Record_Histogram_Explicit_ByAttributes/0                      34.0 ns         34.0 ns     20597805
// BM_Record_Histogram_Explicit_ByAttributes/1                       114 ns          114 ns      6252720
// BM_Record_Histogram_Explicit_ByAttributes/10                     1049 ns         1049 ns       678527
// BM_Record_Histogram_Explicit_ByAttributes/128                   15747 ns        15744 ns        43593
// BM_Record_Histogram_Explicit_ByCardinality/10                     278 ns          278 ns      2514763
// BM_Record_Histogram_Explicit_ByCardinality/500                    309 ns          309 ns      2329567
// BM_Record_Histogram_Explicit_ByCardinality/2000                   318 ns          318 ns      2247860
// BM_Record_Histogram_Explicit_ByThreads/threads:1                  280 ns          280 ns      2502514
// BM_Record_Histogram_Explicit_ByThreads/threads:2                  796 ns          701 ns       993288
// BM_Record_Histogram_Explicit_ByThreads/threads:4                 1710 ns         1327 ns       419468
// BM_Record_Histogram_Base2Expo_ByAttributes/0                     36.6 ns         36.6 ns     19063681
// BM_Record_Histogram_Base2Expo_ByAttributes/1                      115 ns          115 ns      5972985
// BM_Record_Histogram_Base2Expo_ByAttributes/10                    1040 ns         1040 ns       670899
// BM_Record_Histogram_Base2Expo_ByAttributes/128                  16436 ns        16435 ns        39525
// BM_Record_Histogram_Base2Expo_ByCardinality/10                    280 ns          280 ns      2450117
// BM_Record_Histogram_Base2Expo_ByCardinality/500                   301 ns          301 ns      2191454
// BM_Record_Histogram_Base2Expo_ByCardinality/2000                  313 ns          312 ns      2204814
// BM_Record_Histogram_Base2Expo_ByThreads/threads:1                 282 ns          282 ns      2481630
// BM_Record_Histogram_Base2Expo_ByThreads/threads:2                 645 ns          611 ns       887212
// BM_Record_Histogram_Base2Expo_ByThreads/threads:4                1651 ns         1314 ns       511640
// BM_Record_Gauge_Disabled_ByThreads/threads:1                    0.216 ns        0.216 ns   3232809477
// BM_Record_Gauge_Disabled_ByThreads/threads:2                    0.216 ns        0.216 ns   3244371026
// BM_Record_Gauge_Disabled_ByThreads/threads:4                    0.223 ns        0.223 ns   2298688384
// BM_Record_Gauge_Drop_ByThreads/threads:1                          278 ns          278 ns      2532793
// BM_Record_Gauge_Drop_ByThreads/threads:2                          517 ns          513 ns      1321788
// BM_Record_Gauge_Drop_ByThreads/threads:4                         1245 ns         1037 ns       696352
// BM_Record_Gauge_LastValue_ByAttributes/0                         34.9 ns         34.9 ns     19978376
// BM_Record_Gauge_LastValue_ByAttributes/1                          108 ns          108 ns      6470010
// BM_Record_Gauge_LastValue_ByAttributes/10                        1039 ns         1039 ns       676449
// BM_Record_Gauge_LastValue_ByAttributes/128                      17818 ns        17815 ns        37888
// BM_Record_Gauge_LastValue_ByCardinality/10                        271 ns          271 ns      2582416
// BM_Record_Gauge_LastValue_ByCardinality/500                       302 ns          302 ns      2359183
// BM_Record_Gauge_LastValue_ByCardinality/2000                      318 ns          318 ns      2295226
// BM_Record_Gauge_LastValue_ByThreads/threads:1                     270 ns          270 ns      2573195
// BM_Record_Gauge_LastValue_ByThreads/threads:2                     784 ns          686 ns       994984
// BM_Record_Gauge_LastValue_ByThreads/threads:4                    1620 ns         1270 ns       413504
// BM_Record_Counter_Sum_Exemplar_AlwaysOff                          286 ns          286 ns      2470999
// BM_Record_Counter_Sum_Exemplar_AlwaysOn                           420 ns          420 ns      1669949
// BM_Record_Counter_Sum_Exemplar_TraceBased_EmptyContext            290 ns          290 ns      2436719
// BM_Record_Counter_Sum_Exemplar_TraceBased_UnsampledContext        289 ns          289 ns      2414961
// BM_Record_Counter_Sum_Exemplar_TraceBased_SampledContext          432 ns          431 ns      1615447
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOff                   292 ns          292 ns      2407769
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOn                    435 ns          435 ns      1616356
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOff                  295 ns          295 ns      2347904
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOn                   436 ns          436 ns      1623613
// BM_Record_BoundCounter_Disabled_ByThreads/threads:1             0.336 ns        0.336 ns   2095049975
// BM_Record_BoundCounter_Disabled_ByThreads/threads:2             0.292 ns        0.292 ns   2104157288
// BM_Record_BoundCounter_Disabled_ByThreads/threads:4             0.285 ns        0.285 ns   1865496864
// BM_Record_BoundCounter_Drop_ByThreads/threads:1                  5.86 ns         5.86 ns    120410059
// BM_Record_BoundCounter_Drop_ByThreads/threads:2                  10.5 ns         7.56 ns     69726378
// BM_Record_BoundCounter_Drop_ByThreads/threads:4                   141 ns          131 ns      8562596
// BM_Record_BoundCounter_Sum_ByThreads/threads:1                   10.2 ns         10.2 ns     68523190
// BM_Record_BoundCounter_Sum_ByThreads/threads:2                   15.5 ns         10.3 ns     67745558
// BM_Record_BoundCounter_Sum_ByThreads/threads:4                   31.3 ns         12.4 ns     45178704
// BM_Record_BoundHistogram_Disabled_ByThreads/threads:1           0.214 ns        0.214 ns   3254335214
// BM_Record_BoundHistogram_Disabled_ByThreads/threads:2           0.215 ns        0.215 ns   3209349632
// BM_Record_BoundHistogram_Disabled_ByThreads/threads:4           0.228 ns        0.227 ns   2214249064
// BM_Record_BoundHistogram_Drop_ByThreads/threads:1                6.17 ns         6.17 ns    113446830
// BM_Record_BoundHistogram_Drop_ByThreads/threads:2                37.1 ns         35.7 ns     20000000
// BM_Record_BoundHistogram_Drop_ByThreads/threads:4                 151 ns          149 ns     40000000
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:1            11.6 ns         11.6 ns     60184742
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:2            18.9 ns         12.8 ns     48591882
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:4             106 ns         63.9 ns     36674124
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:1           19.0 ns         19.0 ns     36771009
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:2           83.2 ns         74.1 ns     18307054
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:4           82.0 ns         33.1 ns     31623316
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

// Create a set of random values for recording into histograms
static std::vector<double> MakeRecordingValues()
{
  constexpr std::size_t kNumValues = 1024;
  constexpr double kMinValue       = 1.0;
  constexpr double kMaxValue       = 10000.0;

  thread_local std::mt19937 rng(std::random_device{}());
  thread_local std::uniform_real_distribution<double> dist(kMinValue, kMaxValue);
  std::vector<double> values(kNumValues);
  for (auto &val : values)
  {
    val = dist(rng);
  }
  return values;
}

// Create a set of attributes with a given count
static AttributeMap MakeAttributes(std::size_t count, std::string value = {})
{
  AttributeMap attributes;
  for (std::size_t i = 0; i < count; i++)
  {
    attributes["attr_" + std::to_string(i)] = value.empty() ? "value_" + std::to_string(i) : value;
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
    sets.push_back(MakeAttributes(kNominalAttributeCount, "value_" + std::to_string(i)));
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
  static BenchmarkProvider provider(false);
  static auto counter   = provider.meter->CreateDoubleCounter("benchmark_counter");
  const auto attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  const opentelemetry::context::Context context{};
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Counter_Disabled_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Counter_Drop_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider = [] {
    BenchmarkProvider p;
    p.AddView(metrics_sdk::InstrumentType::kCounter, "benchmark_counter",
              metrics_sdk::AggregationType::kDrop);
    return p;
  }();
  static auto counter   = provider.meter->CreateDoubleCounter("benchmark_counter");
  const auto attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  const opentelemetry::context::Context context{};
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Counter_Drop_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Counter_Sum_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider;
  static auto counter   = provider.meter->CreateDoubleCounter("benchmark_counter");
  const auto attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  const opentelemetry::context::Context context{};
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(counter);
    counter->Add(1.0, attributes_view, context);
  }
}
BENCHMARK(BM_Record_Counter_Sum_ByThreads)->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Counter_Sum_ByAttributes(benchmark::State &state)
{
  BenchmarkProvider provider;
  auto counter          = provider.meter->CreateDoubleCounter("benchmark_counter");
  const auto attributes = MakeAttributes(state.range(0));
  const AttributesView attributes_view(attributes);
  const opentelemetry::context::Context context{};
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
  auto counter              = provider.meter->CreateDoubleCounter("benchmark_counter");
  const auto attribute_sets = MakeAttributeSets(state.range(0));
  const std::vector<AttributesView> attribute_views(attribute_sets.begin(), attribute_sets.end());
  const opentelemetry::context::Context context{};
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

// ---------------------------------------------------------------------------
// Histogram (Explicit Buckets) benchmarks

void BM_Record_Histogram_Disabled_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider(false);
  static auto histogram = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  const auto attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  const opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    histogram->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Histogram_Disabled_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Histogram_Drop_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider = [] {
    BenchmarkProvider p;
    p.AddView(metrics_sdk::InstrumentType::kHistogram, "benchmark_histogram",
              metrics_sdk::AggregationType::kDrop);
    return p;
  }();
  static auto histogram = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  const auto attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  const opentelemetry::context::Context context{};
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

void BM_Record_Histogram_Explicit_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider;
  static auto histogram = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  const auto attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  const opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    histogram->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Histogram_Explicit_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Histogram_Explicit_ByAttributes(benchmark::State &state)
{
  BenchmarkProvider provider;
  auto histogram        = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  const auto attributes = MakeAttributes(state.range(0));
  const AttributesView attributes_view(attributes);
  const opentelemetry::context::Context context{};
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
  auto histogram            = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  const auto attribute_sets = MakeAttributeSets(state.range(0));
  const std::vector<AttributesView> attribute_views(attribute_sets.begin(), attribute_sets.end());
  const opentelemetry::context::Context context{};
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

// ---------------------------------------------------------------------------
// Histogram (Base2ExponentialHistogram) benchmarks

void BM_Record_Histogram_Base2Expo_ByThreads(benchmark::State &state)
{
  static auto provider = []() -> BenchmarkProvider {
    BenchmarkProvider p;
    p.AddView(metrics_sdk::InstrumentType::kHistogram, "benchmark_histogram",
              metrics_sdk::AggregationType::kBase2ExponentialHistogram);
    return p;
  }();
  static auto histogram = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  const auto attributes = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  const opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(histogram);
    histogram->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Histogram_Base2Expo_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Histogram_Base2Expo_ByAttributes(benchmark::State &state)
{
  BenchmarkProvider provider;
  provider.AddView(metrics_sdk::InstrumentType::kHistogram, "benchmark_histogram",
                   metrics_sdk::AggregationType::kBase2ExponentialHistogram);
  auto histogram  = provider.meter->CreateDoubleHistogram("benchmark_histogram");
  auto attributes = MakeAttributes(state.range(0));
  const AttributesView attributes_view(attributes);
  const opentelemetry::context::Context context{};
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
  const std::vector<AttributesView> attribute_views(attribute_sets.begin(), attribute_sets.end());
  const opentelemetry::context::Context context{};
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

// ---------------------------------------------------------------------------
// Gauge (LastValue) benchmarks

#if OPENTELEMETRY_ABI_VERSION_NO > 1

void BM_Record_Gauge_Disabled_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider(false);
  static auto gauge = provider.meter->CreateDoubleGauge("benchmark_gauge");
  auto attributes   = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  const auto values = MakeRecordingValues();
  const opentelemetry::context::Context context{};
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(gauge);
    gauge->Record(values[index++ % values.size()], attributes_view, context);
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
  const AttributesView attributes_view(attributes);
  const auto values = MakeRecordingValues();
  const opentelemetry::context::Context context{};
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(gauge);
    gauge->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Gauge_Drop_ByThreads)->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Gauge_LastValue_ByThreads(benchmark::State &state)
{
  static BenchmarkProvider provider;
  static auto gauge = provider.meter->CreateDoubleGauge("benchmark_gauge");
  auto attributes   = MakeAttributes(kNominalAttributeCount);
  const AttributesView attributes_view(attributes);
  const auto values = MakeRecordingValues();
  const opentelemetry::context::Context context{};
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(gauge);
    gauge->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Gauge_LastValue_ByThreads)
    ->ThreadRange(1, static_cast<int>(GetBenchmarkThreads()));

void BM_Record_Gauge_LastValue_ByAttributes(benchmark::State &state)
{
  BenchmarkProvider provider;
  auto gauge      = provider.meter->CreateDoubleGauge("benchmark_gauge");
  auto attributes = MakeAttributes(state.range(0));
  const AttributesView attributes_view(attributes);
  const opentelemetry::context::Context context{};
  const auto values = MakeRecordingValues();
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(gauge);
    gauge->Record(values[index++ % values.size()], attributes_view, context);
  }
}
BENCHMARK(BM_Record_Gauge_LastValue_ByAttributes)->Arg(0)->Arg(1)->Arg(10)->Arg(128);

void BM_Record_Gauge_LastValue_ByCardinality(benchmark::State &state)
{
  BenchmarkProvider provider;
  auto gauge                = provider.meter->CreateDoubleGauge("benchmark_gauge");
  const auto attribute_sets = MakeAttributeSets(state.range(0));
  const std::vector<AttributesView> attribute_views(attribute_sets.begin(), attribute_sets.end());
  const auto values = MakeRecordingValues();
  const opentelemetry::context::Context context{};
  std::size_t index = 0;
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(gauge);
    const std::size_t i = index++;
    gauge->Record(values[i % values.size()], attribute_views[i % attribute_views.size()], context);
  }
}
BENCHMARK(BM_Record_Gauge_LastValue_ByCardinality)->Arg(10)->Arg(500)->Arg(2000);

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
  const auto context =
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
  const auto context =
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
  const opentelemetry::context::Context context{};
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
  const auto context = MakeContext(opentelemetry::trace::TraceFlags{});
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
  const auto context =
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
  const auto context =
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
  const auto context =
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
  const auto context =
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
  const auto context =
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
