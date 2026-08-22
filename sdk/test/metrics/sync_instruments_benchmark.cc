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
// 2026-08-22T20:05:18+00:00
// Running /home/devuser/build/sdk/test/metrics/sync_instruments_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 2.11, 4.01, 3.82
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// -----------------------------------------------------------------------------------------------------
// Benchmark                                                           Time             CPU   Iterations
// -----------------------------------------------------------------------------------------------------
// BM_Record_Counter_Disabled_ByThreads/threads:1                  0.223 ns        0.223 ns   3149132264
// BM_Record_Counter_Disabled_ByThreads/threads:2                  0.334 ns        0.334 ns   2424266898
// BM_Record_Counter_Disabled_ByThreads/threads:4                  0.335 ns        0.335 ns   1860780336
// BM_Record_Counter_Drop_ByThreads/threads:1                        181 ns          181 ns      3883111
// BM_Record_Counter_Drop_ByThreads/threads:2                        336 ns          336 ns      3412070
// BM_Record_Counter_Drop_ByThreads/threads:4                        548 ns          519 ns      1265888
// BM_Record_Counter_Sum_ByThreads/threads:1                         183 ns          183 ns      3725731
// BM_Record_Counter_Sum_ByThreads/threads:2                         313 ns          313 ns      1731700
// BM_Record_Counter_Sum_ByThreads/threads:4                         658 ns          605 ns      1074776
// BM_Record_Counter_Sum_ByAttributes/0                             21.7 ns         21.7 ns     32259034
// BM_Record_Counter_Sum_ByAttributes/1                             63.7 ns         63.7 ns     11103173
// BM_Record_Counter_Sum_ByAttributes/10                             665 ns          665 ns      1067127
// BM_Record_Counter_Sum_ByAttributes/128                          11495 ns        11495 ns        58949
// BM_Record_Counter_Sum_ByCardinality/10                            189 ns          189 ns      3620079
// BM_Record_Counter_Sum_ByCardinality/500                           187 ns          187 ns      3732425
// BM_Record_Counter_Sum_ByCardinality/2000                          185 ns          185 ns      3746437
// BM_Record_Counter_Sum_ByCardinality/4000                          195 ns          195 ns      3587680
// BM_Record_Histogram_Disabled_ByThreads/threads:1                0.215 ns        0.215 ns   3245684429
// BM_Record_Histogram_Disabled_ByThreads/threads:2                0.215 ns        0.215 ns   3219420938
// BM_Record_Histogram_Disabled_ByThreads/threads:4                0.227 ns        0.227 ns   2728839212
// BM_Record_Histogram_Drop_ByThreads/threads:1                      180 ns          180 ns      3891143
// BM_Record_Histogram_Drop_ByThreads/threads:2                      198 ns          198 ns      3255264
// BM_Record_Histogram_Drop_ByThreads/threads:4                      556 ns          515 ns      1294128
// BM_Record_Histogram_Explicit_ByThreads/threads:1                  192 ns          192 ns      3652980
// BM_Record_Histogram_Explicit_ByThreads/threads:2                  473 ns          471 ns      1458558
// BM_Record_Histogram_Explicit_ByThreads/threads:4                  910 ns          806 ns       840420
// BM_Record_Histogram_Explicit_ByAttributes/0                      25.4 ns         25.4 ns     27240860
// BM_Record_Histogram_Explicit_ByAttributes/1                      73.7 ns         73.7 ns      9490681
// BM_Record_Histogram_Explicit_ByAttributes/10                      662 ns          662 ns      1069361
// BM_Record_Histogram_Explicit_ByAttributes/128                   10666 ns        10665 ns        65408
// BM_Record_Histogram_Explicit_ByCardinality/10                     202 ns          202 ns      3515610
// BM_Record_Histogram_Explicit_ByCardinality/500                    202 ns          202 ns      3499771
// BM_Record_Histogram_Explicit_ByCardinality/2000                   205 ns          205 ns      3499176
// BM_Record_Histogram_Base2Expo_ByThreads/threads:1                 209 ns          209 ns      3356693
// BM_Record_Histogram_Base2Expo_ByThreads/threads:2                 378 ns          378 ns      1566898
// BM_Record_Histogram_Base2Expo_ByThreads/threads:4                 884 ns          786 ns       881608
// BM_Record_Histogram_Base2Expo_ByAttributes/0                     27.4 ns         27.4 ns     23210481
// BM_Record_Histogram_Base2Expo_ByAttributes/1                     72.1 ns         72.1 ns      9380658
// BM_Record_Histogram_Base2Expo_ByAttributes/10                     662 ns          662 ns      1060240
// BM_Record_Histogram_Base2Expo_ByAttributes/128                  10619 ns        10618 ns        57887
// BM_Record_Histogram_Base2Expo_ByCardinality/10                    207 ns          207 ns      3466383
// BM_Record_Histogram_Base2Expo_ByCardinality/500                   203 ns          203 ns      3436550
// BM_Record_Histogram_Base2Expo_ByCardinality/2000                  206 ns          206 ns      3400210
// BM_Record_Counter_Sum_Exemplar_AlwaysOff                          181 ns          181 ns      3876313
// BM_Record_Counter_Sum_Exemplar_AlwaysOn                           331 ns          331 ns      2119737
// BM_Record_Counter_Sum_Exemplar_TraceBased_EmptyContext            188 ns          188 ns      3748573
// BM_Record_Counter_Sum_Exemplar_TraceBased_UnsampledContext        191 ns          191 ns      3628388
// BM_Record_Counter_Sum_Exemplar_TraceBased_SampledContext          341 ns          341 ns      2064088
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOff                   191 ns          191 ns      3657244
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOn                    339 ns          339 ns      2063476
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOff                  190 ns          190 ns      3650174
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOn                   343 ns          343 ns      2076733
//
// *******************************************************************************************************
// ABIv2 with preview options
//    ENABLE_METRICS_EXEMPLAR_PREVIEW = ON
//    OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW = ON
// -------------------------------------------------------------------------------------------------------
// ~/build/sdk/test/metrics/sync_instruments_benchmark
// 2026-08-22T19:58:12+00:00
// Running /home/devuser/build/sdk/test/metrics/sync_instruments_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 1.76, 4.54, 3.46
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// -----------------------------------------------------------------------------------------------------
// Benchmark                                                           Time             CPU   Iterations
// -----------------------------------------------------------------------------------------------------
// BM_Record_Counter_Disabled_ByThreads/threads:1                  0.225 ns        0.225 ns   3106874136
// BM_Record_Counter_Disabled_ByThreads/threads:2                  0.235 ns        0.235 ns   2399463178
// BM_Record_Counter_Disabled_ByThreads/threads:4                  0.235 ns        0.235 ns   3078807728
// BM_Record_Counter_Drop_ByThreads/threads:1                        291 ns          291 ns      2434016
// BM_Record_Counter_Drop_ByThreads/threads:2                        545 ns          536 ns      1124740
// BM_Record_Counter_Drop_ByThreads/threads:4                       1222 ns         1021 ns       646440
// BM_Record_Counter_Sum_ByThreads/threads:1                         293 ns          293 ns      2380047
// BM_Record_Counter_Sum_ByThreads/threads:2                         546 ns          541 ns      1106950
// BM_Record_Counter_Sum_ByThreads/threads:4                        1372 ns         1130 ns       480448
// BM_Record_Counter_Sum_ByAttributes/0                             26.3 ns         26.3 ns     26542376
// BM_Record_Counter_Sum_ByAttributes/1                              102 ns          102 ns      6829105
// BM_Record_Counter_Sum_ByAttributes/10                            1086 ns         1086 ns       638378
// BM_Record_Counter_Sum_ByAttributes/128                          17152 ns        17151 ns        41400
// BM_Record_Counter_Sum_ByCardinality/10                            296 ns          296 ns      2372167
// BM_Record_Counter_Sum_ByCardinality/500                           293 ns          293 ns      2327678
// BM_Record_Counter_Sum_ByCardinality/2000                          308 ns          308 ns      2324245
// BM_Record_Counter_Sum_ByCardinality/4000                          285 ns          285 ns      2494418
// BM_Record_Histogram_Disabled_ByThreads/threads:1                0.219 ns        0.219 ns   3091515656
// BM_Record_Histogram_Disabled_ByThreads/threads:2                0.220 ns        0.220 ns   3112553734
// BM_Record_Histogram_Disabled_ByThreads/threads:4                0.228 ns        0.228 ns   3084546480
// BM_Record_Histogram_Drop_ByThreads/threads:1                      288 ns          288 ns      2422775
// BM_Record_Histogram_Drop_ByThreads/threads:2                      540 ns          535 ns      1184022
// BM_Record_Histogram_Drop_ByThreads/threads:4                     1423 ns         1175 ns       453324
// BM_Record_Histogram_Explicit_ByThreads/threads:1                  297 ns          297 ns      2348315
// BM_Record_Histogram_Explicit_ByThreads/threads:2                  746 ns          683 ns       960462
// BM_Record_Histogram_Explicit_ByThreads/threads:4                 1759 ns         1392 ns       396388
// BM_Record_Histogram_Explicit_ByAttributes/0                      34.0 ns         34.0 ns     20428456
// BM_Record_Histogram_Explicit_ByAttributes/1                       113 ns          113 ns      6214071
// BM_Record_Histogram_Explicit_ByAttributes/10                     1078 ns         1077 ns       650557
// BM_Record_Histogram_Explicit_ByAttributes/128                   17187 ns        17185 ns        41086
// BM_Record_Histogram_Explicit_ByCardinality/10                     305 ns          305 ns      2279601
// BM_Record_Histogram_Explicit_ByCardinality/500                    313 ns          313 ns      2186641
// BM_Record_Histogram_Explicit_ByCardinality/2000                   332 ns          332 ns      2170545
// BM_Record_Histogram_Base2Expo_ByThreads/threads:1                 300 ns          300 ns      2344747
// BM_Record_Histogram_Base2Expo_ByThreads/threads:2                 588 ns          582 ns      1194796
// BM_Record_Histogram_Base2Expo_ByThreads/threads:4                1500 ns         1196 ns       518668
// BM_Record_Histogram_Base2Expo_ByAttributes/0                     39.3 ns         39.3 ns     19087472
// BM_Record_Histogram_Base2Expo_ByAttributes/1                      115 ns          115 ns      5936592
// BM_Record_Histogram_Base2Expo_ByAttributes/10                    1067 ns         1067 ns       656089
// BM_Record_Histogram_Base2Expo_ByAttributes/128                  18242 ns        18241 ns        38268
// BM_Record_Histogram_Base2Expo_ByCardinality/10                    313 ns          313 ns      2235656
// BM_Record_Histogram_Base2Expo_ByCardinality/500                   317 ns          317 ns      2232139
// BM_Record_Histogram_Base2Expo_ByCardinality/2000                  331 ns          331 ns      2086932
// BM_Record_Gauge_Disabled_ByThreads/threads:1                    0.219 ns        0.219 ns   3205741054
// BM_Record_Gauge_Disabled_ByThreads/threads:2                    0.223 ns        0.223 ns   2690301556
// BM_Record_Gauge_Disabled_ByThreads/threads:4                    0.229 ns        0.229 ns   2606758588
// BM_Record_Gauge_Drop_ByThreads/threads:1                          286 ns          286 ns      2439706
// BM_Record_Gauge_Drop_ByThreads/threads:2                          546 ns          539 ns      1177080
// BM_Record_Gauge_Drop_ByThreads/threads:4                         1200 ns         1012 ns       497924
// BM_Record_Gauge_LastValue_ByThreads/threads:1                     293 ns          293 ns      2371945
// BM_Record_Gauge_LastValue_ByThreads/threads:2                     564 ns          559 ns      1232712
// BM_Record_Gauge_LastValue_ByThreads/threads:4                    1513 ns         1214 ns       520464
// BM_Record_Gauge_LastValue_ByAttributes/0                         37.1 ns         37.1 ns     18902040
// BM_Record_Gauge_LastValue_ByAttributes/1                          108 ns          108 ns      6302008
// BM_Record_Gauge_LastValue_ByAttributes/10                        1092 ns         1092 ns       647118
// BM_Record_Gauge_LastValue_ByAttributes/128                      16605 ns        16604 ns        41995
// BM_Record_Gauge_LastValue_ByCardinality/10                        298 ns          298 ns      2289380
// BM_Record_Gauge_LastValue_ByCardinality/500                       309 ns          309 ns      2298937
// BM_Record_Gauge_LastValue_ByCardinality/2000                      318 ns          318 ns      2242299
// BM_Record_Counter_Sum_Exemplar_AlwaysOff                          296 ns          295 ns      2395056
// BM_Record_Counter_Sum_Exemplar_AlwaysOn                           437 ns          437 ns      1585966
// BM_Record_Counter_Sum_Exemplar_TraceBased_EmptyContext            302 ns          302 ns      2365598
// BM_Record_Counter_Sum_Exemplar_TraceBased_UnsampledContext        303 ns          303 ns      2348573
// BM_Record_Counter_Sum_Exemplar_TraceBased_SampledContext          450 ns          450 ns      1557082
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOff                   301 ns          301 ns      2266520
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOn                    453 ns          453 ns      1573692
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOff                  303 ns          303 ns      2274437
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOn                   453 ns          453 ns      1550092
// BM_Record_BoundCounter_Disabled_ByThreads/threads:1             0.221 ns        0.221 ns   3142560276
// BM_Record_BoundCounter_Disabled_ByThreads/threads:2             0.222 ns        0.222 ns   2660334766
// BM_Record_BoundCounter_Disabled_ByThreads/threads:4             0.226 ns        0.226 ns   2861051432
// BM_Record_BoundCounter_Drop_ByThreads/threads:1                  5.81 ns         5.81 ns    120342496
// BM_Record_BoundCounter_Drop_ByThreads/threads:2                  34.8 ns         33.5 ns     20000000
// BM_Record_BoundCounter_Drop_ByThreads/threads:4                   123 ns          112 ns      6062044
// BM_Record_BoundCounter_Sum_ByThreads/threads:1                   10.7 ns         10.7 ns     65164652
// BM_Record_BoundCounter_Sum_ByThreads/threads:2                   19.6 ns         13.2 ns     48429404
// BM_Record_BoundCounter_Sum_ByThreads/threads:4                   73.4 ns         41.7 ns     32971620
// BM_Record_BoundHistogram_Disabled_ByThreads/threads:1           0.214 ns        0.214 ns   3277171217
// BM_Record_BoundHistogram_Disabled_ByThreads/threads:2           0.214 ns        0.214 ns   2444610710
// BM_Record_BoundHistogram_Disabled_ByThreads/threads:4           0.224 ns        0.224 ns   2556315680
// BM_Record_BoundHistogram_Drop_ByThreads/threads:1                6.19 ns         6.19 ns    113761230
// BM_Record_BoundHistogram_Drop_ByThreads/threads:2                32.6 ns         29.9 ns     61564776
// BM_Record_BoundHistogram_Drop_ByThreads/threads:4                 155 ns          145 ns     42223756
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:1            11.8 ns         11.8 ns     59006925
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:2            23.6 ns         13.5 ns     44770066
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:4             196 ns          133 ns      9838224
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:1           19.3 ns         19.3 ns     37052293
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:2           32.8 ns         21.5 ns     35315540
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:4           67.1 ns         25.8 ns     31687100
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
