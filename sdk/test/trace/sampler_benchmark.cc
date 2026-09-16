// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off
//
//  ~/build/sdk/test/trace/sampler_benchmark --benchmark_repetitions=5 --benchmark_display_aggregates_only=true
// 2026-09-14T19:42:38+00:00
// Running /home/devuser/build/sdk/test/trace/sampler_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 4.67, 2.39, 1.00
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// ------------------------------------------------------------------------------------------------------------------------
// Benchmark                                                                              Time             CPU   Iterations
// ------------------------------------------------------------------------------------------------------------------------
// BM_AlwaysOffSamplerConstruction_mean                                               0.338 ns        0.338 ns            5
// BM_AlwaysOffSamplerConstruction_median                                             0.339 ns        0.339 ns            5
// BM_AlwaysOffSamplerConstruction_stddev                                             0.002 ns        0.002 ns            5
// BM_AlwaysOffSamplerConstruction_cv                                                  0.52 %          0.51 %             5
// BM_AlwaysOnSamplerConstruction_mean                                                0.349 ns        0.349 ns            5
// BM_AlwaysOnSamplerConstruction_median                                              0.352 ns        0.352 ns            5
// BM_AlwaysOnSamplerConstruction_stddev                                              0.005 ns        0.005 ns            5
// BM_AlwaysOnSamplerConstruction_cv                                                   1.33 %          1.33 %             5
// BM_AlwaysOffSamplerShouldSample_mean                                                3.11 ns         3.11 ns            5
// BM_AlwaysOffSamplerShouldSample_median                                              3.12 ns         3.12 ns            5
// BM_AlwaysOffSamplerShouldSample_stddev                                             0.011 ns        0.011 ns            5
// BM_AlwaysOffSamplerShouldSample_cv                                                  0.34 %          0.34 %             5
// BM_AlwaysOnSamplerShouldSample_mean                                                 3.11 ns         3.11 ns            5
// BM_AlwaysOnSamplerShouldSample_median                                               3.11 ns         3.11 ns            5
// BM_AlwaysOnSamplerShouldSample_stddev                                              0.008 ns        0.008 ns            5
// BM_AlwaysOnSamplerShouldSample_cv                                                   0.26 %          0.25 %             5
// BM_ParentBasedSamplerShouldSample_mean                                              6.18 ns         6.18 ns            5
// BM_ParentBasedSamplerShouldSample_median                                            6.15 ns         6.14 ns            5
// BM_ParentBasedSamplerShouldSample_stddev                                           0.091 ns        0.091 ns            5
// BM_ParentBasedSamplerShouldSample_cv                                                1.47 %          1.47 %             5
// BM_TraceIdRatioBasedSamplerShouldSample_mean                                        2.83 ns         2.83 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_median                                      2.80 ns         2.80 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_stddev                                     0.062 ns        0.062 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_cv                                          2.18 %          2.18 %             5
// BM_ProbabilitySamplerShouldSampleDrops_mean                                          274 ns          274 ns            5
// BM_ProbabilitySamplerShouldSampleDrops_median                                        274 ns          274 ns            5
// BM_ProbabilitySamplerShouldSampleDrops_stddev                                       3.25 ns         3.26 ns            5
// BM_ProbabilitySamplerShouldSampleDrops_cv                                           1.18 %          1.19 %             5
// BM_ProbabilitySamplerShouldSampleSamples_mean                                        289 ns          289 ns            5
// BM_ProbabilitySamplerShouldSampleSamples_median                                      289 ns          288 ns            5
// BM_ProbabilitySamplerShouldSampleSamples_stddev                                     2.53 ns         2.52 ns            5
// BM_ProbabilitySamplerShouldSampleSamples_cv                                         0.88 %          0.87 %             5
// BM_CompositeProbabilitySamplerShouldSampleDrops_mean                                 221 ns          221 ns            5
// BM_CompositeProbabilitySamplerShouldSampleDrops_median                               224 ns          223 ns            5
// BM_CompositeProbabilitySamplerShouldSampleDrops_stddev                              4.53 ns         4.52 ns            5
// BM_CompositeProbabilitySamplerShouldSampleDrops_cv                                  2.05 %          2.05 %             5
// BM_CompositeProbabilitySamplerShouldSampleSamples_mean                               241 ns          241 ns            5
// BM_CompositeProbabilitySamplerShouldSampleSamples_median                             240 ns          240 ns            5
// BM_CompositeProbabilitySamplerShouldSampleSamples_stddev                            2.53 ns         2.54 ns            5
// BM_CompositeProbabilitySamplerShouldSampleSamples_cv                                1.05 %          1.05 %             5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesKind_mean                             248 ns          248 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesKind_median                           248 ns          248 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesKind_stddev                          1.78 ns         1.80 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesKind_cv                              0.72 %          0.73 %             5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesParent_mean                           248 ns          248 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesParent_median                         248 ns          248 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesParent_stddev                        2.35 ns         2.36 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesParent_cv                            0.95 %          0.95 %             5
// BM_CompositeRuleBasedSamplerShouldSampleNoMatch_mean                                 224 ns          224 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleNoMatch_median                               224 ns          223 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleNoMatch_stddev                              1.17 ns         1.18 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleNoMatch_cv                                  0.52 %          0.53 %             5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesValue_mean                            288 ns          288 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesValue_median                          288 ns          288 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesValue_stddev                         3.97 ns         3.97 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesValue_cv                             1.38 %          1.38 %             5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesPattern_mean                          276 ns          276 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesPattern_median                        277 ns          277 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesPattern_stddev                       1.27 ns         1.27 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesPattern_cv                           0.46 %          0.46 %             5
// BM_CompositeParentThresholdSamplerShouldSampleParentHasThreshold_mean                290 ns          290 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentHasThreshold_median              290 ns          290 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentHasThreshold_stddev             1.07 ns         1.08 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentHasThreshold_cv                 0.37 %          0.37 %             5
// BM_CompositeParentThresholdSamplerShouldSampleNoParent_mean                         49.8 ns         49.8 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNoParent_median                       50.0 ns         49.9 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNoParent_stddev                      0.213 ns        0.217 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNoParent_cv                           0.43 %          0.43 %             5
// BM_CompositeParentThresholdSamplerShouldSampleParentSampledNoThreshold_mean         55.7 ns         55.7 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentSampledNoThreshold_median       55.8 ns         55.8 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentSampledNoThreshold_stddev      0.407 ns        0.408 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentSampledNoThreshold_cv           0.73 %          0.73 %             5
// BM_CompositeAlwaysOnSamplerShouldSample_mean                                         245 ns          245 ns            5
// BM_CompositeAlwaysOnSamplerShouldSample_median                                       245 ns          245 ns            5
// BM_CompositeAlwaysOnSamplerShouldSample_stddev                                      3.36 ns         3.37 ns            5
// BM_CompositeAlwaysOnSamplerShouldSample_cv                                          1.37 %          1.38 %             5
// BM_CompositeAlwaysOffSamplerShouldSample_mean                                        220 ns          220 ns            5
// BM_CompositeAlwaysOffSamplerShouldSample_median                                      220 ns          220 ns            5
// BM_CompositeAlwaysOffSamplerShouldSample_stddev                                     5.15 ns         5.15 ns            5
// BM_CompositeAlwaysOffSamplerShouldSample_cv                                         2.34 %          2.34 %             5
// BM_SpanCreation_mean                                                                 218 ns          218 ns            5
// BM_SpanCreation_median                                                               218 ns          218 ns            5
// BM_SpanCreation_stddev                                                              1.00 ns         1.02 ns            5
// BM_SpanCreation_cv                                                                  0.46 %          0.47 %             5
// BM_NoopSpanCreation_mean                                                            31.6 ns         31.6 ns            5
// BM_NoopSpanCreation_median                                                          31.6 ns         31.6 ns            5
// BM_NoopSpanCreation_stddev                                                         0.359 ns        0.359 ns            5
// BM_NoopSpanCreation_cv                                                              1.14 %          1.13 %             5
// BM_SpanCreationWithSamplingResultAttributes/1_mean                                   263 ns          263 ns            5
// BM_SpanCreationWithSamplingResultAttributes/1_median                                 263 ns          263 ns            5
// BM_SpanCreationWithSamplingResultAttributes/1_stddev                                1.77 ns         1.75 ns            5
// BM_SpanCreationWithSamplingResultAttributes/1_cv                                    0.67 %          0.67 %             5
// BM_SpanCreationWithSamplingResultAttributes/10_mean                                  621 ns          621 ns            5
// BM_SpanCreationWithSamplingResultAttributes/10_median                                617 ns          617 ns            5
// BM_SpanCreationWithSamplingResultAttributes/10_stddev                               7.70 ns         7.71 ns            5
// BM_SpanCreationWithSamplingResultAttributes/10_cv                                   1.24 %          1.24 %             5
// BM_SpanCreationWithSamplingResultAttributes/128_mean                                5476 ns         5475 ns            5
// BM_SpanCreationWithSamplingResultAttributes/128_median                              5506 ns         5505 ns            5
// BM_SpanCreationWithSamplingResultAttributes/128_stddev                              60.4 ns         60.4 ns            5
// BM_SpanCreationWithSamplingResultAttributes/128_cv                                  1.10 %          1.10 %             5
//
// clang-format on

#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/context/context_value.h"  // IWYU pragma: keep
#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/composable_always_off.h"
#include "opentelemetry/sdk/trace/samplers/composable_always_on.h"
#include "opentelemetry/sdk/trace/samplers/composable_parent_threshold.h"
#include "opentelemetry/sdk/trace/samplers/composable_probability.h"
#include "opentelemetry/sdk/trace/samplers/composable_rule_based.h"
#include "opentelemetry/sdk/trace/samplers/composable_sampler.h"
#include "opentelemetry/sdk/trace/samplers/composite_sampler.h"
#include "opentelemetry/sdk/trace/samplers/composite_sampler_factory.h"
#include "opentelemetry/sdk/trace/samplers/parent.h"
#include "opentelemetry/sdk/trace/samplers/probability.h"
#include "opentelemetry/sdk/trace/samplers/rule_based_predicate.h"
#include "opentelemetry/sdk/trace/samplers/trace_id_ratio.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/trace_state.h"
#include "opentelemetry/trace/tracer.h"

using namespace opentelemetry::sdk::trace;
using opentelemetry::exporter::memory::InMemorySpanExporter;
using opentelemetry::trace::SpanContext;

namespace nostd = opentelemetry::nostd;

namespace
{
// Sampler constructor used as a baseline to compare with other samplers
void BM_AlwaysOffSamplerConstruction(benchmark::State &state)
{
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(AlwaysOffSampler());
  }
}
BENCHMARK(BM_AlwaysOffSamplerConstruction);

// Sampler constructor used as a baseline to compare with other samplers
void BM_AlwaysOnSamplerConstruction(benchmark::State &state)
{
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(AlwaysOnSampler());
  }
}
BENCHMARK(BM_AlwaysOnSamplerConstruction);

/*
  Fails to build with GCC.
  See upstream bug: https://github.com/google/benchmark/issues/1675
*/
#if 0
void BM_ParentBasedSamplerConstruction(benchmark::State &state)
{
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(ParentBasedSampler(std::make_shared<AlwaysOnSampler>()));
  }
}
BENCHMARK(BM_ParentBasedSamplerConstruction);

void BM_TraceIdRatioBasedSamplerConstruction(benchmark::State &state)
{
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(TraceIdRatioBasedSampler(0.01));
  }
}
BENCHMARK(BM_TraceIdRatioBasedSamplerConstruction);
#endif

void BenchmarkShouldSampler(Sampler &sampler,
                            const SpanContext &parent_context,
                            benchmark::State &state)
{
  opentelemetry::trace::TraceId trace_id   = parent_context.trace_id();
  opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

  using M = std::map<std::string, int>;
  M m1    = {{}};

  using L =
      std::vector<std::pair<opentelemetry::trace::SpanContext, std::map<std::string, std::string>>>;
  L l1 = {{opentelemetry::trace::SpanContext(false, false), {}},
          {opentelemetry::trace::SpanContext(false, false), {}}};

  opentelemetry::common::KeyValueIterableView<M> view{m1};
  opentelemetry::trace::SpanContextKeyValueIterableView<L> links{l1};

  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(
        sampler.ShouldSample(parent_context, trace_id, "", span_kind, view, links));
  }
}

void BenchmarkShouldSampler(Sampler &sampler, benchmark::State &state)
{
  BenchmarkShouldSampler(sampler, SpanContext::GetInvalid(), state);
}

void BenchmarkShouldSampler(
    Sampler &sampler,
    const SpanContext &parent_context,
    const std::map<std::string, opentelemetry::common::AttributeValue> &attributes_map,
    benchmark::State &state)
{
  opentelemetry::trace::TraceId trace_id   = parent_context.trace_id();
  opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

  using L =
      std::vector<std::pair<opentelemetry::trace::SpanContext, std::map<std::string, std::string>>>;
  L l1 = {{opentelemetry::trace::SpanContext(false, false), {}},
          {opentelemetry::trace::SpanContext(false, false), {}}};

  opentelemetry::common::KeyValueIterableView<
      std::map<std::string, opentelemetry::common::AttributeValue>>
      view{attributes_map};
  opentelemetry::trace::SpanContextKeyValueIterableView<L> links{l1};

  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(
        sampler.ShouldSample(parent_context, trace_id, "", span_kind, view, links));
  }
}

// Builds a valid, sampled, remote parent whose tracestate carries the given "ot" value.
// Adds valid (non-zero) trace and span IDs and sets the random flag.
SpanContext MakeRemoteParentWithOt(nostd::string_view ot_value)
{
  constexpr uint8_t trace_id_bytes[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  constexpr uint8_t span_id_bytes[]  = {1, 2, 3, 4, 5, 6, 7, 8};
  opentelemetry::trace::TraceFlags trace_flags(opentelemetry::trace::TraceFlags::kIsSampled |
                                               opentelemetry::trace::TraceFlags::kIsRandom);
  auto trace_state = opentelemetry::trace::TraceState::GetDefault()->Set("ot", ot_value);
  return SpanContext(opentelemetry::trace::TraceId(trace_id_bytes),
                     opentelemetry::trace::SpanId(span_id_bytes), trace_flags,
                     /*is_remote=*/true, trace_state);
}

constexpr const char *kOtValueMinRandomness = "rv:00000000000000;th:8";
constexpr const char *kOtValueMaxRandomness = "rv:ffffffffffffff;th:8";

const std::map<std::string, opentelemetry::common::AttributeValue> &GetAttributes()
{
  static const std::map<std::string, opentelemetry::common::AttributeValue> attributes = {
      {"key_one", "value_one"},
      {"key_two", static_cast<int64_t>(1)},
      {"key_three", "value_target"},
  };
  return attributes;
}

// Sampler used as a baseline to compare with other samplers
void BM_AlwaysOffSamplerShouldSample(benchmark::State &state)
{
  AlwaysOffSampler sampler;

  BenchmarkShouldSampler(sampler, state);
}
BENCHMARK(BM_AlwaysOffSamplerShouldSample);

// Sampler used as a baseline to compare with other samplers
void BM_AlwaysOnSamplerShouldSample(benchmark::State &state)
{
  AlwaysOnSampler sampler;

  BenchmarkShouldSampler(sampler, state);
}
BENCHMARK(BM_AlwaysOnSamplerShouldSample);

void BM_ParentBasedSamplerShouldSample(benchmark::State &state)
{
  ParentBasedSampler sampler(std::make_shared<AlwaysOnSampler>());

  BenchmarkShouldSampler(sampler, state);
}
BENCHMARK(BM_ParentBasedSamplerShouldSample);

void BM_TraceIdRatioBasedSamplerShouldSample(benchmark::State &state)
{
  TraceIdRatioBasedSampler sampler(0.01);

  BenchmarkShouldSampler(sampler, state);
}
BENCHMARK(BM_TraceIdRatioBasedSamplerShouldSample);

void BM_ProbabilitySamplerShouldSampleDrops(benchmark::State &state)
{
  ProbabilitySampler sampler(0.5);
  BenchmarkShouldSampler(sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), state);
}
BENCHMARK(BM_ProbabilitySamplerShouldSampleDrops);

void BM_ProbabilitySamplerShouldSampleSamples(benchmark::State &state)
{
  ProbabilitySampler sampler(0.5);
  BenchmarkShouldSampler(sampler, MakeRemoteParentWithOt(kOtValueMaxRandomness), state);
}
BENCHMARK(BM_ProbabilitySamplerShouldSampleSamples);

void BM_CompositeProbabilitySamplerShouldSampleDrops(benchmark::State &state)
{
  auto sampler =
      CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(0.5));
  BenchmarkShouldSampler(*sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), state);
}
BENCHMARK(BM_CompositeProbabilitySamplerShouldSampleDrops);

void BM_CompositeProbabilitySamplerShouldSampleSamples(benchmark::State &state)
{
  auto sampler =
      CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(0.5));
  BenchmarkShouldSampler(*sampler, MakeRemoteParentWithOt(kOtValueMaxRandomness), state);
}
BENCHMARK(BM_CompositeProbabilitySamplerShouldSampleSamples);

void BM_CompositeRuleBasedSamplerShouldSampleMatchesKind(benchmark::State &state)
{
  std::vector<PredicatedSampler> rules;
  RuleBasedPredicateOptions options;
  options.match_span_kind_internal = true;
  rules.push_back({std::make_shared<RuleBasedPredicate>(std::move(options)),
                   std::make_shared<ComposableAlwaysOnSampler>()});
  auto sampler = CompositeSamplerFactory::Create(
      std::make_shared<ComposableRuleBasedSampler>(std::move(rules)));
  BenchmarkShouldSampler(*sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), state);
}
BENCHMARK(BM_CompositeRuleBasedSamplerShouldSampleMatchesKind);

void BM_CompositeRuleBasedSamplerShouldSampleMatchesParent(benchmark::State &state)
{
  std::vector<PredicatedSampler> rules;
  RuleBasedPredicateOptions options;
  options.match_parent_remote = true;
  rules.push_back({std::make_shared<RuleBasedPredicate>(std::move(options)),
                   std::make_shared<ComposableAlwaysOnSampler>()});
  auto sampler = CompositeSamplerFactory::Create(
      std::make_shared<ComposableRuleBasedSampler>(std::move(rules)));
  BenchmarkShouldSampler(*sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), state);
}
BENCHMARK(BM_CompositeRuleBasedSamplerShouldSampleMatchesParent);

void BM_CompositeRuleBasedSamplerShouldSampleNoMatch(benchmark::State &state)
{
  std::vector<PredicatedSampler> rules;
  RuleBasedPredicateOptions options;
  options.match_span_kind_server = true;  // never matches the kInternal span kind used here
  rules.push_back({std::make_shared<RuleBasedPredicate>(std::move(options)),
                   std::make_shared<ComposableAlwaysOnSampler>()});
  auto sampler = CompositeSamplerFactory::Create(
      std::make_shared<ComposableRuleBasedSampler>(std::move(rules)));
  BenchmarkShouldSampler(*sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), state);
}
BENCHMARK(BM_CompositeRuleBasedSamplerShouldSampleNoMatch);

void BM_CompositeRuleBasedSamplerShouldSampleMatchesValue(benchmark::State &state)
{
  std::vector<PredicatedSampler> rules;
  RuleBasedPredicateOptions options;
  options.match_values = true;
  options.values_key   = "key_three";
  options.values       = {"value_target"};
  rules.push_back({std::make_shared<RuleBasedPredicate>(std::move(options)),
                   std::make_shared<ComposableAlwaysOnSampler>()});
  auto sampler = CompositeSamplerFactory::Create(
      std::make_shared<ComposableRuleBasedSampler>(std::move(rules)));
  BenchmarkShouldSampler(*sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), GetAttributes(),
                         state);
}
BENCHMARK(BM_CompositeRuleBasedSamplerShouldSampleMatchesValue);

void BM_CompositeRuleBasedSamplerShouldSampleMatchesPattern(benchmark::State &state)
{
  std::vector<PredicatedSampler> rules;
  RuleBasedPredicateOptions options;
  options.match_patterns = true;
  options.patterns_key   = "key_three";
  options.included       = {"value_*"};
  rules.push_back({std::make_shared<RuleBasedPredicate>(std::move(options)),
                   std::make_shared<ComposableAlwaysOnSampler>()});
  auto sampler = CompositeSamplerFactory::Create(
      std::make_shared<ComposableRuleBasedSampler>(std::move(rules)));
  BenchmarkShouldSampler(*sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), GetAttributes(),
                         state);
}
BENCHMARK(BM_CompositeRuleBasedSamplerShouldSampleMatchesPattern);

void BM_CompositeParentThresholdSamplerShouldSampleParentHasThreshold(benchmark::State &state)
{
  CompositeSampler sampler(std::make_shared<ComposableParentThresholdSampler>(
      std::make_shared<ComposableProbabilitySampler>(0.5)));
  BenchmarkShouldSampler(sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), state);
}
BENCHMARK(BM_CompositeParentThresholdSamplerShouldSampleParentHasThreshold);

void BM_CompositeParentThresholdSamplerShouldSampleNoParent(benchmark::State &state)
{
  CompositeSampler sampler(std::make_shared<ComposableParentThresholdSampler>(
      std::make_shared<ComposableProbabilitySampler>(0.5)));
  BenchmarkShouldSampler(sampler, state);
}
BENCHMARK(BM_CompositeParentThresholdSamplerShouldSampleNoParent);

void BM_CompositeParentThresholdSamplerShouldSampleParentSampledNoThreshold(benchmark::State &state)
{
  CompositeSampler sampler(std::make_shared<ComposableParentThresholdSampler>(
      std::make_shared<ComposableProbabilitySampler>(0.5)));
  BenchmarkShouldSampler(sampler, MakeRemoteParentWithOt(""), state);
}
BENCHMARK(BM_CompositeParentThresholdSamplerShouldSampleParentSampledNoThreshold);

void BM_CompositeAlwaysOnSamplerShouldSample(benchmark::State &state)
{
  CompositeSampler sampler(std::make_shared<ComposableAlwaysOnSampler>());
  BenchmarkShouldSampler(sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), state);
}
BENCHMARK(BM_CompositeAlwaysOnSamplerShouldSample);

void BM_CompositeAlwaysOffSamplerShouldSample(benchmark::State &state)
{
  CompositeSampler sampler(std::make_shared<ComposableAlwaysOffSampler>());
  BenchmarkShouldSampler(sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), state);
}
BENCHMARK(BM_CompositeAlwaysOffSamplerShouldSample);

// Sampler Helper Function
void BenchmarkSpanCreation(std::unique_ptr<Sampler> &&sampler, benchmark::State &state)
{
  std::unique_ptr<SpanExporter> exporter(new InMemorySpanExporter());
  std::unique_ptr<SpanProcessor> processor(new SimpleSpanProcessor(std::move(exporter)));
  std::vector<std::unique_ptr<SpanProcessor>> processors;
  processors.push_back(std::move(processor));
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  auto context =
      std::make_shared<TracerContext>(std::move(processors), resource, std::move(sampler));
  auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(context));

  while (state.KeepRunning())
  {
    auto span = tracer->StartSpan("span");

    span->SetAttribute("attr1", 3.1);

    span->End();
  }
}

// Test to measure performance for span creation
void BM_SpanCreation(benchmark::State &state)
{
  std::unique_ptr<Sampler> sampler(new AlwaysOnSampler());
  BenchmarkSpanCreation(std::move(sampler), state);
}
BENCHMARK(BM_SpanCreation);

// Test to measure performance overhead for no-op span creation
void BM_NoopSpanCreation(benchmark::State &state)
{
  std::unique_ptr<Sampler> sampler(new AlwaysOffSampler());
  BenchmarkSpanCreation(std::move(sampler), state);
}
BENCHMARK(BM_NoopSpanCreation);

namespace
{
class AttributeContributingSampler : public Sampler
{
public:
  AttributeContributingSampler(std::size_t num_attributes)
      : attributes_(CreateAttributes(num_attributes))
  {}

  static std::map<std::string, opentelemetry::common::AttributeValue> CreateAttributes(
      std::size_t num_attributes)
  {
    auto attributes_map = std::map<std::string, opentelemetry::common::AttributeValue>();
    for (std::size_t i = 0; i < num_attributes; ++i)
    {
      attributes_map.emplace("attr" + std::to_string(i), static_cast<int>(i));
    }
    return attributes_map;
  }

  SamplingResult ShouldSample(
      const opentelemetry::trace::SpanContext & /*parent_context*/,
      opentelemetry::trace::TraceId /*trace_id*/,
      nostd::string_view /*name*/,
      opentelemetry::trace::SpanKind /*span_kind*/,
      const opentelemetry::common::KeyValueIterable & /*attributes*/,
      const opentelemetry::trace::SpanContextKeyValueIterable & /*links*/) noexcept override
  {
    return SamplingResult{
        Decision::RECORD_AND_SAMPLE,
        std::make_unique<std::map<std::string, opentelemetry::common::AttributeValue>>(attributes_),
        {}};
  }

  nostd::string_view GetDescription() const noexcept override
  {
    return "AttributeContributingSampler";
  }

private:
  std::map<std::string, opentelemetry::common::AttributeValue> attributes_;
};
}  // namespace

// Test to measure performance for a sampler that adds attributes to the span
void BM_SpanCreationWithSamplingResultAttributes(benchmark::State &state)
{
  std::size_t num_attributes = static_cast<std::size_t>(state.range(0));
  std::unique_ptr<Sampler> sampler(new AttributeContributingSampler(num_attributes));
  BenchmarkSpanCreation(std::move(sampler), state);
}
BENCHMARK(BM_SpanCreationWithSamplingResultAttributes)->Arg(1)->Arg(10)->Arg(128);

}  // namespace
BENCHMARK_MAIN();
