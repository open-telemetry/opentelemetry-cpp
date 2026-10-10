// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off
//
// ~/build/sdk/test/trace/sampler_benchmark --benchmark_repetitions=5 --benchmark_display_aggregates_only=true
// 2026-09-21T17:21:28+00:00
// Running /home/devuser/build/sdk/test/trace/sampler_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 1.14, 1.82, 2.80
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// ------------------------------------------------------------------------------------------------------------------------
// Benchmark                                                                              Time             CPU   Iterations
// ------------------------------------------------------------------------------------------------------------------------
// BM_AlwaysOffSamplerConstruction_mean                                               0.353 ns        0.353 ns            5
// BM_AlwaysOffSamplerConstruction_median                                             0.352 ns        0.352 ns            5
// BM_AlwaysOffSamplerConstruction_stddev                                             0.000 ns        0.000 ns            5
// BM_AlwaysOffSamplerConstruction_cv                                                  0.05 %          0.05 %             5
// BM_AlwaysOnSamplerConstruction_mean                                                0.353 ns        0.353 ns            5
// BM_AlwaysOnSamplerConstruction_median                                              0.353 ns        0.353 ns            5
// BM_AlwaysOnSamplerConstruction_stddev                                              0.000 ns        0.000 ns            5
// BM_AlwaysOnSamplerConstruction_cv                                                   0.10 %          0.10 %             5
// BM_AlwaysOffSamplerShouldSample_mean                                                3.12 ns         3.12 ns            5
// BM_AlwaysOffSamplerShouldSample_median                                              3.12 ns         3.12 ns            5
// BM_AlwaysOffSamplerShouldSample_stddev                                             0.009 ns        0.009 ns            5
// BM_AlwaysOffSamplerShouldSample_cv                                                  0.30 %          0.30 %             5
// BM_AlwaysOnSamplerShouldSample_mean                                                 3.17 ns         3.17 ns            5
// BM_AlwaysOnSamplerShouldSample_median                                               3.16 ns         3.16 ns            5
// BM_AlwaysOnSamplerShouldSample_stddev                                              0.037 ns        0.037 ns            5
// BM_AlwaysOnSamplerShouldSample_cv                                                   1.18 %          1.17 %             5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/1_mean                                    9.18 ns         9.18 ns            5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/1_median                                  9.18 ns         9.18 ns            5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/1_stddev                                 0.006 ns        0.006 ns            5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/1_cv                                      0.07 %          0.07 %             5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/32_mean                                    192 ns          192 ns            5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/32_median                                  192 ns          192 ns            5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/32_stddev                                0.170 ns        0.164 ns            5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/32_cv                                     0.09 %          0.09 %             5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/256_mean                                  1501 ns         1501 ns            5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/256_median                                1500 ns         1500 ns            5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/256_stddev                                4.32 ns         4.33 ns            5
// BM_AlwaysOnSamplerShouldSampleNestedDepth/256_cv                                    0.29 %          0.29 %             5
// BM_ParentBasedSamplerShouldSample_mean                                              6.34 ns         6.34 ns            5
// BM_ParentBasedSamplerShouldSample_median                                            6.34 ns         6.34 ns            5
// BM_ParentBasedSamplerShouldSample_stddev                                           0.001 ns        0.001 ns            5
// BM_ParentBasedSamplerShouldSample_cv                                                0.02 %          0.02 %             5
// BM_TraceIdRatioBasedSamplerShouldSample_mean                                        2.77 ns         2.77 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_median                                      2.77 ns         2.77 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_stddev                                     0.003 ns        0.003 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_cv                                          0.10 %          0.11 %             5
// BM_ProbabilitySamplerShouldSampleDrops_mean                                          126 ns          126 ns            5
// BM_ProbabilitySamplerShouldSampleDrops_median                                        126 ns          126 ns            5
// BM_ProbabilitySamplerShouldSampleDrops_stddev                                      0.367 ns        0.371 ns            5
// BM_ProbabilitySamplerShouldSampleDrops_cv                                           0.29 %          0.29 %             5
// BM_ProbabilitySamplerShouldSampleSamples_mean                                       22.2 ns         22.2 ns            5
// BM_ProbabilitySamplerShouldSampleSamples_median                                     22.2 ns         22.2 ns            5
// BM_ProbabilitySamplerShouldSampleSamples_stddev                                    0.053 ns        0.053 ns            5
// BM_ProbabilitySamplerShouldSampleSamples_cv                                         0.24 %          0.24 %             5
// BM_CompositeProbabilitySamplerShouldSampleDrops_mean                                 127 ns          127 ns            5
// BM_CompositeProbabilitySamplerShouldSampleDrops_median                               127 ns          127 ns            5
// BM_CompositeProbabilitySamplerShouldSampleDrops_stddev                             0.189 ns        0.190 ns            5
// BM_CompositeProbabilitySamplerShouldSampleDrops_cv                                  0.15 %          0.15 %             5
// BM_CompositeProbabilitySamplerShouldSampleSamples_mean                              24.0 ns         24.0 ns            5
// BM_CompositeProbabilitySamplerShouldSampleSamples_median                            23.9 ns         23.9 ns            5
// BM_CompositeProbabilitySamplerShouldSampleSamples_stddev                           0.151 ns        0.151 ns            5
// BM_CompositeProbabilitySamplerShouldSampleSamples_cv                                0.63 %          0.63 %             5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/1_mean                       155 ns          155 ns            5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/1_median                     155 ns          155 ns            5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/1_stddev                    2.63 ns         2.64 ns            5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/1_cv                        1.70 %          1.70 %             5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/23_mean                      441 ns          441 ns            5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/23_median                    441 ns          441 ns            5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/23_stddev                   4.99 ns         4.99 ns            5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/23_cv                       1.13 %          1.13 %             5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/46_mean                      692 ns          692 ns            5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/46_median                    692 ns          692 ns            5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/46_stddev                  0.936 ns        0.931 ns            5
// BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount/46_cv                       0.14 %          0.13 %             5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/1_mean               152 ns          152 ns            5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/1_median             152 ns          152 ns            5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/1_stddev           0.243 ns        0.245 ns            5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/1_cv                0.16 %          0.16 %             5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/16_mean              541 ns          541 ns            5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/16_median            542 ns          542 ns            5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/16_stddev           2.15 ns         2.15 ns            5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/16_cv               0.40 %          0.40 %             5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/31_mean              917 ns          917 ns            5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/31_median            915 ns          915 ns            5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/31_stddev           2.65 ns         2.66 ns            5
// BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount/31_cv               0.29 %          0.29 %             5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/1_mean                        99.6 ns         99.6 ns            5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/1_median                      99.6 ns         99.6 ns            5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/1_stddev                     0.314 ns        0.314 ns            5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/1_cv                          0.32 %          0.32 %             5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/32_mean                        702 ns          702 ns            5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/32_median                      703 ns          702 ns            5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/32_stddev                    0.562 ns        0.556 ns            5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/32_cv                         0.08 %          0.08 %             5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/256_mean                      4813 ns         4813 ns            5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/256_median                    4814 ns         4814 ns            5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/256_stddev                    2.55 ns         2.56 ns            5
// BM_CompositeProbabilitySamplerShouldSampleNestedDepth/256_cv                        0.05 %          0.05 %             5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesKind_mean                             137 ns          137 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesKind_median                           137 ns          137 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesKind_stddev                         0.080 ns        0.079 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesKind_cv                              0.06 %          0.06 %             5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesParent_mean                           139 ns          139 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesParent_median                         138 ns          138 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesParent_stddev                        1.12 ns         1.11 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesParent_cv                            0.80 %          0.80 %             5
// BM_CompositeRuleBasedSamplerShouldSampleNoMatch_mean                                 129 ns          129 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleNoMatch_median                               129 ns          129 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleNoMatch_stddev                             0.089 ns        0.089 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleNoMatch_cv                                  0.07 %          0.07 %             5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesValue_mean                            152 ns          152 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesValue_median                          152 ns          152 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesValue_stddev                        0.655 ns        0.660 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesValue_cv                             0.43 %          0.43 %             5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesPattern_mean                          159 ns          159 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesPattern_median                        159 ns          159 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesPattern_stddev                      0.239 ns        0.237 ns            5
// BM_CompositeRuleBasedSamplerShouldSampleMatchesPattern_cv                           0.15 %          0.15 %             5
// BM_CompositeParentThresholdSamplerShouldSampleParentHasThreshold_mean                152 ns          152 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentHasThreshold_median              152 ns          152 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentHasThreshold_stddev            0.236 ns        0.227 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentHasThreshold_cv                 0.16 %          0.15 %             5
// BM_CompositeParentThresholdSamplerShouldSampleNoParent_mean                         12.0 ns         12.0 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNoParent_median                       12.0 ns         12.0 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNoParent_stddev                      0.003 ns        0.003 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNoParent_cv                           0.03 %          0.02 %             5
// BM_CompositeParentThresholdSamplerShouldSampleParentSampledNoThreshold_mean         13.4 ns         13.4 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentSampledNoThreshold_median       13.4 ns         13.4 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentSampledNoThreshold_stddev      0.010 ns        0.010 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleParentSampledNoThreshold_cv           0.08 %          0.08 %             5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/1_mean                    84.4 ns         84.4 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/1_median                  84.4 ns         84.4 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/1_stddev                 0.045 ns        0.044 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/1_cv                      0.05 %          0.05 %             5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/32_mean                    953 ns          953 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/32_median                  953 ns          953 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/32_stddev                 1.38 ns         1.38 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/32_cv                     0.14 %          0.15 %             5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/256_mean                  7111 ns         7110 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/256_median                7113 ns         7112 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/256_stddev                8.75 ns         8.98 ns            5
// BM_CompositeParentThresholdSamplerShouldSampleNestedDepth/256_cv                    0.12 %          0.13 %             5
// BM_CompositeAlwaysOnSamplerShouldSample_mean                                         133 ns          133 ns            5
// BM_CompositeAlwaysOnSamplerShouldSample_median                                       133 ns          133 ns            5
// BM_CompositeAlwaysOnSamplerShouldSample_stddev                                     0.158 ns        0.158 ns            5
// BM_CompositeAlwaysOnSamplerShouldSample_cv                                          0.12 %          0.12 %             5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/1_mean                           83.8 ns         83.8 ns            5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/1_median                         83.8 ns         83.8 ns            5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/1_stddev                        0.047 ns        0.049 ns            5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/1_cv                             0.06 %          0.06 %             5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/32_mean                           624 ns          624 ns            5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/32_median                         624 ns          624 ns            5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/32_stddev                        1.47 ns         1.51 ns            5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/32_cv                            0.24 %          0.24 %             5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/256_mean                         4400 ns         4399 ns            5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/256_median                       4395 ns         4395 ns            5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/256_stddev                       12.1 ns         12.0 ns            5
// BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth/256_cv                           0.27 %          0.27 %             5
// BM_CompositeAlwaysOffSamplerShouldSample_mean                                        127 ns          127 ns            5
// BM_CompositeAlwaysOffSamplerShouldSample_median                                      127 ns          127 ns            5
// BM_CompositeAlwaysOffSamplerShouldSample_stddev                                    0.527 ns        0.528 ns            5
// BM_CompositeAlwaysOffSamplerShouldSample_cv                                         0.42 %          0.42 %             5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/1_mean                          16.4 ns         16.4 ns            5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/1_median                        16.4 ns         16.4 ns            5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/1_stddev                       0.001 ns        0.001 ns            5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/1_cv                            0.01 %          0.01 %             5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/32_mean                          450 ns          450 ns            5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/32_median                        450 ns          450 ns            5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/32_stddev                      0.132 ns        0.137 ns            5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/32_cv                           0.03 %          0.03 %             5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/256_mean                        3573 ns         3572 ns            5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/256_median                      3572 ns         3572 ns            5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/256_stddev                     0.357 ns        0.317 ns            5
// BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth/256_cv                          0.01 %          0.01 %             5
// BM_SpanCreation_mean                                                                 225 ns          225 ns            5
// BM_SpanCreation_median                                                               225 ns          225 ns            5
// BM_SpanCreation_stddev                                                             0.055 ns        0.063 ns            5
// BM_SpanCreation_cv                                                                  0.02 %          0.03 %             5
// BM_NoopSpanCreation_mean                                                            32.0 ns         32.0 ns            5
// BM_NoopSpanCreation_median                                                          32.0 ns         32.0 ns            5
// BM_NoopSpanCreation_stddev                                                         0.024 ns        0.025 ns            5
// BM_NoopSpanCreation_cv                                                              0.08 %          0.08 %             5
// BM_SpanCreationWithSamplingResultAttributes/1_mean                                   278 ns          278 ns            5
// BM_SpanCreationWithSamplingResultAttributes/1_median                                 277 ns          277 ns            5
// BM_SpanCreationWithSamplingResultAttributes/1_stddev                               0.976 ns        0.969 ns            5
// BM_SpanCreationWithSamplingResultAttributes/1_cv                                    0.35 %          0.35 %             5
// BM_SpanCreationWithSamplingResultAttributes/10_mean                                  636 ns          636 ns            5
// BM_SpanCreationWithSamplingResultAttributes/10_median                                637 ns          637 ns            5
// BM_SpanCreationWithSamplingResultAttributes/10_stddev                               3.40 ns         3.39 ns            5
// BM_SpanCreationWithSamplingResultAttributes/10_cv                                   0.54 %          0.53 %             5
// BM_SpanCreationWithSamplingResultAttributes/128_mean                                5741 ns         5740 ns            5
// BM_SpanCreationWithSamplingResultAttributes/128_median                              5717 ns         5716 ns            5
// BM_SpanCreationWithSamplingResultAttributes/128_stddev                              87.4 ns         87.3 ns            5
// BM_SpanCreationWithSamplingResultAttributes/128_cv                                  1.52 %          1.52 %             5
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

// Builds an "ot" value carrying th/rv plus `count` other sub-keys
std::string MakeOtValueWithSubkeys(int count)
{
  std::string value = "rv:00000000000000;th:8";
  for (int i = 0; i < count; ++i)
  {
    char key[2] = {static_cast<char>('a' + i / 26), static_cast<char>('a' + i % 26)};
    value += ";" + std::string(key, 2) + ":1";
  }
  return value;
}

// Builds a valid, sampled, remote parent whose tracestate carries
// `other_member_count` unrelated top-level tracestate members (e.g. "vendor1",
// "vendor2", ...) alongside "ot"
SpanContext MakeRemoteParentWithManyTraceStateMembers(int other_member_count,
                                                      nostd::string_view ot_value)
{
  constexpr uint8_t trace_id_bytes[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  constexpr uint8_t span_id_bytes[]  = {1, 2, 3, 4, 5, 6, 7, 8};
  opentelemetry::trace::TraceFlags trace_flags(opentelemetry::trace::TraceFlags::kIsSampled |
                                               opentelemetry::trace::TraceFlags::kIsRandom);
  auto trace_state = opentelemetry::trace::TraceState::GetDefault();
  for (int i = 1; i <= other_member_count; ++i)
  {
    trace_state = trace_state->Set("vendor" + std::to_string(i), "x");
  }
  trace_state = trace_state->Set("ot", ot_value);
  return SpanContext(opentelemetry::trace::TraceId(trace_id_bytes),
                     opentelemetry::trace::SpanId(span_id_bytes), trace_flags,
                     /*is_remote=*/true, trace_state);
}

const std::map<std::string, opentelemetry::common::AttributeValue> &GetAttributes()
{
  static const std::map<std::string, opentelemetry::common::AttributeValue> attributes = {
      {"key_one", "value_one"},
      {"key_two", static_cast<int64_t>(1)},
      {"key_three", "value_target"},
  };
  return attributes;
}

// Simulates `depth` nested spans within one trace.
void BenchmarkShouldSampleNested(Sampler &sampler, int depth, benchmark::State &state)
{
  // Last 7 bytes are all 0xff so and ratio-based sampler (e.g. ComposableProbabilitySampler) keeps
  // every span
  constexpr uint8_t trace_id_bytes[] = {1, 2,    3,    4,    5,    6,    7,    8,
                                        9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  constexpr uint8_t span_id_bytes[]  = {1, 2, 3, 4, 5, 6, 7, 8};
  opentelemetry::trace::TraceId trace_id(trace_id_bytes);
  opentelemetry::trace::SpanId span_id(span_id_bytes);
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
    SpanContext parent = SpanContext::GetInvalid();
    for (int i = 0; i < depth; ++i)
    {
      auto result = sampler.ShouldSample(parent, trace_id, "", span_kind, view, links);
      opentelemetry::trace::TraceFlags flags(result.decision == Decision::RECORD_AND_SAMPLE
                                                 ? opentelemetry::trace::TraceFlags::kIsSampled
                                                 : 0);
      parent = SpanContext(trace_id, span_id, flags, /*is_remote=*/false, result.trace_state);
    }
    benchmark::DoNotOptimize(parent);
  }
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

void BM_AlwaysOnSamplerShouldSampleNestedDepth(benchmark::State &state)
{
  AlwaysOnSampler sampler;
  BenchmarkShouldSampleNested(sampler, static_cast<int>(state.range(0)), state);
}
BENCHMARK(BM_AlwaysOnSamplerShouldSampleNestedDepth)->Arg(1)->Arg(32)->Arg(256);

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

// Scales the number of other sub-keys carried inside the "ot" value
// alongside th/rv, up toward the practical ceiling imposed by the
// 256-character "ot" value size limit.
void BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount(benchmark::State &state)
{
  auto sampler =
      CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(0.5));
  std::string ot_value = MakeOtValueWithSubkeys(static_cast<int>(state.range(0)));
  BenchmarkShouldSampler(*sampler, MakeRemoteParentWithOt(ot_value), state);
}
BENCHMARK(BM_CompositeProbabilitySamplerShouldSampleOtSubkeyCount)->Arg(1)->Arg(23)->Arg(46);

// Scales the number of unrelated top-level tracestate members alongside "ot",
// up toward the 32-entry TraceState::kMaxKeyValuePairs cap.
void BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount(benchmark::State &state)
{
  auto sampler =
      CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(0.5));
  BenchmarkShouldSampler(*sampler,
                         MakeRemoteParentWithManyTraceStateMembers(static_cast<int>(state.range(0)),
                                                                   kOtValueMinRandomness),
                         state);
}
BENCHMARK(BM_CompositeProbabilitySamplerShouldSampleTraceStateMemberCount)
    ->Arg(1)
    ->Arg(16)
    ->Arg(31);

void BM_CompositeProbabilitySamplerShouldSampleNestedDepth(benchmark::State &state)
{
  auto sampler =
      CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(0.5));
  BenchmarkShouldSampleNested(*sampler, static_cast<int>(state.range(0)), state);
}
BENCHMARK(BM_CompositeProbabilitySamplerShouldSampleNestedDepth)->Arg(1)->Arg(32)->Arg(256);

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

void BM_CompositeParentThresholdSamplerShouldSampleNestedDepth(benchmark::State &state)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<ComposableParentThresholdSampler>(
      std::make_shared<ComposableAlwaysOnSampler>()));
  BenchmarkShouldSampleNested(*sampler, static_cast<int>(state.range(0)), state);
}
BENCHMARK(BM_CompositeParentThresholdSamplerShouldSampleNestedDepth)->Arg(1)->Arg(32)->Arg(256);

void BM_CompositeAlwaysOnSamplerShouldSample(benchmark::State &state)
{
  CompositeSampler sampler(std::make_shared<ComposableAlwaysOnSampler>());
  BenchmarkShouldSampler(sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), state);
}
BENCHMARK(BM_CompositeAlwaysOnSamplerShouldSample);

void BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth(benchmark::State &state)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<ComposableAlwaysOnSampler>());
  BenchmarkShouldSampleNested(*sampler, static_cast<int>(state.range(0)), state);
}
BENCHMARK(BM_CompositeAlwaysOnSamplerShouldSampleNestedDepth)->Arg(1)->Arg(32)->Arg(256);

void BM_CompositeAlwaysOffSamplerShouldSample(benchmark::State &state)
{
  CompositeSampler sampler(std::make_shared<ComposableAlwaysOffSampler>());
  BenchmarkShouldSampler(sampler, MakeRemoteParentWithOt(kOtValueMinRandomness), state);
}
BENCHMARK(BM_CompositeAlwaysOffSamplerShouldSample);

void BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth(benchmark::State &state)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<ComposableAlwaysOffSampler>());
  BenchmarkShouldSampleNested(*sampler, static_cast<int>(state.range(0)), state);
}
BENCHMARK(BM_CompositeAlwaysOffSamplerShouldSampleNestedDepth)->Arg(1)->Arg(32)->Arg(256);

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
