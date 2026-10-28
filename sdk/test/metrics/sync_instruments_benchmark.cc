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
// ~/build/sdk/test/metrics/sync_instruments_benchmark
// 2026-09-02T04:44:52+00:00
// Running /home/devuser/build/sdk/test/metrics/sync_instruments_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 1.28, 3.38, 2.89
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// -----------------------------------------------------------------------------------------------------
// Benchmark                                                           Time             CPU   Iterations
// -----------------------------------------------------------------------------------------------------
// BM_Record_Counter_Disabled_ByThreads/threads:1                  0.219 ns        0.219 ns   3178418618
// BM_Record_Counter_Disabled_ByThreads/threads:2                  0.220 ns        0.220 ns   3107456060
// BM_Record_Counter_Disabled_ByThreads/threads:4                  0.228 ns        0.227 ns   2777031836
// BM_Record_Counter_Drop_ByThreads/threads:1                       2.29 ns         2.29 ns    305338324
// BM_Record_Counter_Drop_ByThreads/threads:2                       2.29 ns         2.29 ns    269371614
// BM_Record_Counter_Drop_ByThreads/threads:4                       2.35 ns         2.35 ns    240692852
// BM_Record_Counter_Sum_ByThreads/threads:1                         173 ns          173 ns      4033669
// BM_Record_Counter_Sum_ByThreads/threads:2                         263 ns          263 ns      2105922
// BM_Record_Counter_Sum_ByThreads/threads:4                         820 ns          728 ns       932456
// BM_Record_Counter_Sum_ByAttributes/0                             21.7 ns         21.7 ns     32237207
// BM_Record_Counter_Sum_ByAttributes/1                             59.0 ns         59.0 ns     11865426
// BM_Record_Counter_Sum_ByAttributes/10                             615 ns          615 ns      1132969
// BM_Record_Counter_Sum_ByAttributes/128                          10473 ns        10471 ns        61260
// BM_Record_Counter_Sum_ByCardinality/10                            178 ns          178 ns      3741926
// BM_Record_Counter_Sum_ByCardinality/500                           178 ns          178 ns      3875935
// BM_Record_Counter_Sum_ByCardinality/2000                          180 ns          180 ns      3765961
// BM_Record_Counter_Sum_ByCardinality/4000                          187 ns          187 ns      3731380
// BM_Record_Histogram_Disabled_ByThreads/threads:1                0.213 ns        0.213 ns   3288043694
// BM_Record_Histogram_Disabled_ByThreads/threads:2                0.214 ns        0.214 ns   3239303644
// BM_Record_Histogram_Disabled_ByThreads/threads:4                0.219 ns        0.219 ns   2444940680
// BM_Record_Histogram_Drop_ByThreads/threads:1                     2.50 ns         2.50 ns    279768220
// BM_Record_Histogram_Drop_ByThreads/threads:2                     2.52 ns         2.52 ns    231723630
// BM_Record_Histogram_Drop_ByThreads/threads:4                     2.56 ns         2.56 ns    230104300
// BM_Record_Histogram_Explicit_ByThreads/threads:1                  184 ns          184 ns      3795006
// BM_Record_Histogram_Explicit_ByThreads/threads:2                  459 ns          458 ns      1409266
// BM_Record_Histogram_Explicit_ByThreads/threads:4                  989 ns          873 ns       767192
// BM_Record_Histogram_Explicit_ByAttributes/0                      25.5 ns         25.5 ns     27761752
// BM_Record_Histogram_Explicit_ByAttributes/1                      68.1 ns         68.0 ns     10269587
// BM_Record_Histogram_Explicit_ByAttributes/10                      616 ns          616 ns      1137361
// BM_Record_Histogram_Explicit_ByAttributes/128                   11633 ns        11631 ns        59646
// BM_Record_Histogram_Explicit_ByCardinality/10                     187 ns          187 ns      3740950
// BM_Record_Histogram_Explicit_ByCardinality/500                    193 ns          193 ns      3644548
// BM_Record_Histogram_Explicit_ByCardinality/2000                   197 ns          197 ns      3590726
// BM_Record_Histogram_Base2Expo_ByThreads/threads:1                 186 ns          186 ns      3695842
// BM_Record_Histogram_Base2Expo_ByThreads/threads:2                 306 ns          306 ns      2236848
// BM_Record_Histogram_Base2Expo_ByThreads/threads:4                 724 ns          654 ns       967532
// BM_Record_Histogram_Base2Expo_ByAttributes/0                     29.6 ns         29.6 ns     22206631
// BM_Record_Histogram_Base2Expo_ByAttributes/1                     75.2 ns         75.2 ns      9902341
// BM_Record_Histogram_Base2Expo_ByAttributes/10                     653 ns          653 ns      1089054
// BM_Record_Histogram_Base2Expo_ByAttributes/128                  10788 ns        10788 ns        65623
// BM_Record_Histogram_Base2Expo_ByCardinality/10                    194 ns          194 ns      3329016
// BM_Record_Histogram_Base2Expo_ByCardinality/500                   198 ns          198 ns      3574653
// BM_Record_Histogram_Base2Expo_ByCardinality/2000                  197 ns          197 ns      3499208
// BM_Record_Counter_Sum_Exemplar_AlwaysOff                          173 ns          173 ns      4040048
// BM_Record_Counter_Sum_Exemplar_AlwaysOn                           320 ns          320 ns      2235414
// BM_Record_Counter_Sum_Exemplar_TraceBased_EmptyContext            181 ns          181 ns      3851898
// BM_Record_Counter_Sum_Exemplar_TraceBased_UnsampledContext        186 ns          186 ns      3714975
// BM_Record_Counter_Sum_Exemplar_TraceBased_SampledContext          323 ns          323 ns      2173860
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOff                   182 ns          182 ns      3824218
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOn                    324 ns          324 ns      2166338
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOff                  182 ns          182 ns      3739049
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOn                   326 ns          326 ns      2165242
//
// *******************************************************************************************************
// ABIv2 with preview options
//    ENABLE_METRICS_EXEMPLAR_PREVIEW = ON
//    OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW = ON
// -------------------------------------------------------------------------------------------------------
// ~/build/sdk/test/metrics/sync_instruments_benchmark
// 2026-09-02T04:38:03+00:00
// Running /home/devuser/build/sdk/test/metrics/sync_instruments_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 0.22, 1.44, 1.97
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// -----------------------------------------------------------------------------------------------------
// Benchmark                                                           Time             CPU   Iterations
// -----------------------------------------------------------------------------------------------------
// BM_Record_Counter_Disabled_ByThreads/threads:1                  0.222 ns        0.222 ns   3157611074
// BM_Record_Counter_Disabled_ByThreads/threads:2                  0.228 ns        0.228 ns   2317559982
// BM_Record_Counter_Disabled_ByThreads/threads:4                  0.224 ns        0.224 ns   2449047760
// BM_Record_Counter_Drop_ByThreads/threads:1                       1.75 ns         1.75 ns    398352965
// BM_Record_Counter_Drop_ByThreads/threads:2                       1.77 ns         1.77 ns    395926306
// BM_Record_Counter_Drop_ByThreads/threads:4                       1.80 ns         1.80 ns    385184052
// BM_Record_Counter_Sum_ByThreads/threads:1                         290 ns          290 ns      2411526
// BM_Record_Counter_Sum_ByThreads/threads:2                         524 ns          519 ns      1299874
// BM_Record_Counter_Sum_ByThreads/threads:4                        1337 ns         1107 ns       637784
// BM_Record_Counter_Sum_ByAttributes/0                             26.3 ns         26.3 ns     26555865
// BM_Record_Counter_Sum_ByAttributes/1                              104 ns          104 ns      6677963
// BM_Record_Counter_Sum_ByAttributes/10                            1061 ns         1061 ns       666984
// BM_Record_Counter_Sum_ByAttributes/128                          16510 ns        16509 ns        42555
// BM_Record_Counter_Sum_ByCardinality/10                            305 ns          305 ns      2273336
// BM_Record_Counter_Sum_ByCardinality/500                           309 ns          309 ns      2362157
// BM_Record_Counter_Sum_ByCardinality/2000                          306 ns          306 ns      2259027
// BM_Record_Counter_Sum_ByCardinality/4000                          282 ns          282 ns      2441950
// BM_Record_Histogram_Disabled_ByThreads/threads:1                0.237 ns        0.237 ns   3065761938
// BM_Record_Histogram_Disabled_ByThreads/threads:2                0.230 ns        0.230 ns   2427449390
// BM_Record_Histogram_Disabled_ByThreads/threads:4                0.233 ns        0.233 ns   2243067152
// BM_Record_Histogram_Drop_ByThreads/threads:1                     2.12 ns         2.12 ns    330519632
// BM_Record_Histogram_Drop_ByThreads/threads:2                     2.16 ns         2.16 ns    326855378
// BM_Record_Histogram_Drop_ByThreads/threads:4                     2.16 ns         2.16 ns    285233320
// BM_Record_Histogram_Explicit_ByThreads/threads:1                  303 ns          303 ns      2305548
// BM_Record_Histogram_Explicit_ByThreads/threads:2                  584 ns          583 ns      1179140
// BM_Record_Histogram_Explicit_ByThreads/threads:4                 1474 ns         1210 ns       478800
// BM_Record_Histogram_Explicit_ByAttributes/0                      34.0 ns         34.0 ns     20382028
// BM_Record_Histogram_Explicit_ByAttributes/1                       116 ns          116 ns      6048721
// BM_Record_Histogram_Explicit_ByAttributes/10                     1135 ns         1135 ns       615638
// BM_Record_Histogram_Explicit_ByAttributes/128                   18457 ns        18457 ns        36005
// BM_Record_Histogram_Explicit_ByCardinality/10                     318 ns          318 ns      2152688
// BM_Record_Histogram_Explicit_ByCardinality/500                    312 ns          312 ns      2213058
// BM_Record_Histogram_Explicit_ByCardinality/2000                   327 ns          327 ns      2140112
// BM_Record_Histogram_Base2Expo_ByThreads/threads:1                 308 ns          308 ns      2273908
// BM_Record_Histogram_Base2Expo_ByThreads/threads:2                 801 ns          722 ns       919292
// BM_Record_Histogram_Base2Expo_ByThreads/threads:4                1710 ns         1357 ns       467556
// BM_Record_Histogram_Base2Expo_ByAttributes/0                     32.4 ns         32.4 ns     19635763
// BM_Record_Histogram_Base2Expo_ByAttributes/1                      117 ns          117 ns      5991477
// BM_Record_Histogram_Base2Expo_ByAttributes/10                    1070 ns         1070 ns       649709
// BM_Record_Histogram_Base2Expo_ByAttributes/128                  18389 ns        18388 ns        36322
// BM_Record_Histogram_Base2Expo_ByCardinality/10                    317 ns          317 ns      2225556
// BM_Record_Histogram_Base2Expo_ByCardinality/500                   315 ns          315 ns      2219892
// BM_Record_Histogram_Base2Expo_ByCardinality/2000                  344 ns          344 ns      2133056
// BM_Record_Gauge_Disabled_ByThreads/threads:1                    0.231 ns        0.230 ns   2985182463
// BM_Record_Gauge_Disabled_ByThreads/threads:2                    0.258 ns        0.258 ns   2535349748
// BM_Record_Gauge_Disabled_ByThreads/threads:4                    0.263 ns        0.263 ns   2058977740
// BM_Record_Gauge_Drop_ByThreads/threads:1                         1.77 ns         1.77 ns    396510306
// BM_Record_Gauge_Drop_ByThreads/threads:2                         1.76 ns         1.76 ns    396203792
// BM_Record_Gauge_Drop_ByThreads/threads:4                         1.88 ns         1.88 ns    378999736
// BM_Record_Gauge_LastValue_ByThreads/threads:1                     301 ns          301 ns      2322567
// BM_Record_Gauge_LastValue_ByThreads/threads:2                     684 ns          627 ns      1054172
// BM_Record_Gauge_LastValue_ByThreads/threads:4                    1454 ns         1180 ns       485272
// BM_Record_Gauge_LastValue_ByAttributes/0                         36.4 ns         36.4 ns     19232981
// BM_Record_Gauge_LastValue_ByAttributes/1                          112 ns          112 ns      6230722
// BM_Record_Gauge_LastValue_ByAttributes/10                        1091 ns         1090 ns       644785
// BM_Record_Gauge_LastValue_ByAttributes/128                      19942 ns        19941 ns        35750
// BM_Record_Gauge_LastValue_ByCardinality/10                        310 ns          310 ns      2220110
// BM_Record_Gauge_LastValue_ByCardinality/500                       310 ns          310 ns      2273872
// BM_Record_Gauge_LastValue_ByCardinality/2000                      315 ns          315 ns      2176490
// BM_Record_Counter_Sum_Exemplar_AlwaysOff                          297 ns          297 ns      2346887
// BM_Record_Counter_Sum_Exemplar_AlwaysOn                           437 ns          437 ns      1607196
// BM_Record_Counter_Sum_Exemplar_TraceBased_EmptyContext            304 ns          304 ns      2307315
// BM_Record_Counter_Sum_Exemplar_TraceBased_UnsampledContext        303 ns          303 ns      2307124
// BM_Record_Counter_Sum_Exemplar_TraceBased_SampledContext          449 ns          449 ns      1569626
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOff                   307 ns          307 ns      2287304
// BM_Record_Histogram_Explicit_Exemplar_AlwaysOn                    443 ns          443 ns      1578995
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOff                  310 ns          310 ns      2219974
// BM_Record_Histogram_Base2Expo_Exemplar_AlwaysOn                   448 ns          448 ns      1564477
// BM_Record_BoundCounter_Disabled_ByThreads/threads:1             0.225 ns        0.225 ns   3109199368
// BM_Record_BoundCounter_Disabled_ByThreads/threads:2             0.221 ns        0.221 ns   3139159388
// BM_Record_BoundCounter_Disabled_ByThreads/threads:4             0.230 ns        0.230 ns   2444090640
// BM_Record_BoundCounter_Drop_ByThreads/threads:1                  1.14 ns         1.14 ns    602944599
// BM_Record_BoundCounter_Drop_ByThreads/threads:2                  1.16 ns         1.16 ns    582566836
// BM_Record_BoundCounter_Drop_ByThreads/threads:4                  1.20 ns         1.20 ns    527738312
// BM_Record_BoundCounter_Sum_ByThreads/threads:1                   15.7 ns         15.7 ns     44638005
// BM_Record_BoundCounter_Sum_ByThreads/threads:2                    487 ns          487 ns      1730740
// BM_Record_BoundCounter_Sum_ByThreads/threads:4                    334 ns          288 ns      3176988
// BM_Record_BoundHistogram_Disabled_ByThreads/threads:1           0.225 ns        0.225 ns   3074007094
// BM_Record_BoundHistogram_Disabled_ByThreads/threads:2           0.298 ns        0.298 ns   2348095536
// BM_Record_BoundHistogram_Disabled_ByThreads/threads:4           0.267 ns        0.267 ns   1925459520
// BM_Record_BoundHistogram_Drop_ByThreads/threads:1                1.76 ns         1.76 ns    397471719
// BM_Record_BoundHistogram_Drop_ByThreads/threads:2                1.76 ns         1.76 ns    391899074
// BM_Record_BoundHistogram_Drop_ByThreads/threads:4                1.79 ns         1.79 ns    357721228
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:1            16.6 ns         16.6 ns     42000966
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:2             237 ns          230 ns      3395654
// BM_Record_BoundHistogram_Explicit_ByThreads/threads:4             473 ns          394 ns      2378376
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:1           23.9 ns         23.9 ns     28651138
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:2            489 ns          489 ns      2000000
// BM_Record_BoundHistogram_Base2Expo_ByThreads/threads:4            590 ns          494 ns      1754400
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

  // NOLINTNEXTLINE(bugprone-random-generator-seed)
  thread_local std::mt19937 rng(1234);
  thread_local std::uniform_real_distribution<double> dist(kMinValue, kMaxValue);
  std::vector<double> values(kNumValues);
  for (auto &val : values)
  {
    val = dist(rng);
  }
  return values;
}

// Create a set of attributes with a given count
static AttributeMap MakeAttributes(std::size_t count, const std::string &value = {})
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
