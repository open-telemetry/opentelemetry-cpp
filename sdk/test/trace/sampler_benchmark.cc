// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off
//
//  ~/build/sdk/test/trace/sampler_benchmark --benchmark_repetitions=5 --benchmark_display_aggregates_only=true
// 2026-07-18T13:33:22+00:00
// Running /home/devuser/build/sdk/test/trace/sampler_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 0.93, 2.55, 2.62
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// -------------------------------------------------------------------------------------------------
// Benchmark                                                       Time             CPU   Iterations
// -------------------------------------------------------------------------------------------------
// BM_AlwaysOffSamplerConstruction_mean                        0.345 ns        0.345 ns            5
// BM_AlwaysOffSamplerConstruction_median                      0.345 ns        0.345 ns            5
// BM_AlwaysOffSamplerConstruction_stddev                      0.003 ns        0.003 ns            5
// BM_AlwaysOffSamplerConstruction_cv                           0.78 %          0.77 %             5
// BM_AlwaysOnSamplerConstruction_mean                         0.339 ns        0.339 ns            5
// BM_AlwaysOnSamplerConstruction_median                       0.336 ns        0.336 ns            5
// BM_AlwaysOnSamplerConstruction_stddev                       0.008 ns        0.008 ns            5
// BM_AlwaysOnSamplerConstruction_cv                            2.33 %          2.32 %             5
// BM_AlwaysOffSamplerShouldSample_mean                         4.14 ns         4.14 ns            5
// BM_AlwaysOffSamplerShouldSample_median                       4.12 ns         4.12 ns            5
// BM_AlwaysOffSamplerShouldSample_stddev                      0.053 ns        0.053 ns            5
// BM_AlwaysOffSamplerShouldSample_cv                           1.29 %          1.28 %             5
// BM_AlwaysOnSamplerShouldSample_mean                          4.51 ns         4.51 ns            5
// BM_AlwaysOnSamplerShouldSample_median                        4.52 ns         4.52 ns            5
// BM_AlwaysOnSamplerShouldSample_stddev                       0.052 ns        0.052 ns            5
// BM_AlwaysOnSamplerShouldSample_cv                            1.16 %          1.15 %             5
// BM_ParentBasedSamplerShouldSample_mean                       7.42 ns         7.42 ns            5
// BM_ParentBasedSamplerShouldSample_median                     7.42 ns         7.41 ns            5
// BM_ParentBasedSamplerShouldSample_stddev                    0.011 ns        0.011 ns            5
// BM_ParentBasedSamplerShouldSample_cv                         0.15 %          0.15 %             5
// BM_TraceIdRatioBasedSamplerShouldSample_mean                 4.03 ns         4.03 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_median               4.03 ns         4.03 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_stddev              0.007 ns        0.007 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_cv                   0.18 %          0.19 %             5
// BM_SpanCreation_mean                                          208 ns          208 ns            5
// BM_SpanCreation_median                                        207 ns          207 ns            5
// BM_SpanCreation_stddev                                       1.17 ns         1.18 ns            5
// BM_SpanCreation_cv                                           0.57 %          0.57 %             5
// BM_NoopSpanCreation_mean                                     30.0 ns         30.0 ns            5
// BM_NoopSpanCreation_median                                   30.0 ns         30.0 ns            5
// BM_NoopSpanCreation_stddev                                  0.048 ns        0.046 ns            5
// BM_NoopSpanCreation_cv                                       0.16 %          0.15 %             5
// BM_SpanCreationWithSamplingResultAttributes/0_mean            217 ns          217 ns            5
// BM_SpanCreationWithSamplingResultAttributes/0_median          216 ns          216 ns            5
// BM_SpanCreationWithSamplingResultAttributes/0_stddev        0.843 ns        0.850 ns            5
// BM_SpanCreationWithSamplingResultAttributes/0_cv             0.39 %          0.39 %             5
// BM_SpanCreationWithSamplingResultAttributes/1_mean            257 ns          257 ns            5
// BM_SpanCreationWithSamplingResultAttributes/1_median          257 ns          257 ns            5
// BM_SpanCreationWithSamplingResultAttributes/1_stddev        0.649 ns        0.641 ns            5
// BM_SpanCreationWithSamplingResultAttributes/1_cv             0.25 %          0.25 %             5
// BM_SpanCreationWithSamplingResultAttributes/10_mean           634 ns          633 ns            5
// BM_SpanCreationWithSamplingResultAttributes/10_median         628 ns          628 ns            5
// BM_SpanCreationWithSamplingResultAttributes/10_stddev        12.0 ns         12.0 ns            5
// BM_SpanCreationWithSamplingResultAttributes/10_cv            1.89 %          1.89 %             5
// BM_SpanCreationWithSamplingResultAttributes/128_mean         8954 ns         8953 ns            5
// BM_SpanCreationWithSamplingResultAttributes/128_median       9197 ns         9196 ns            5
// BM_SpanCreationWithSamplingResultAttributes/128_stddev        381 ns          381 ns            5
// BM_SpanCreationWithSamplingResultAttributes/128_cv           4.26 %          4.25 %             5
//
// clang-format on

#include <benchmark/benchmark.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/context/context_value.h"  // IWYU pragma: keep
#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/parent.h"
#include "opentelemetry/sdk/trace/samplers/trace_id_ratio.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/trace_id.h"
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

// Sampler Helper Function
void BenchmarkShouldSampler(Sampler &sampler, benchmark::State &state)
{
  opentelemetry::trace::TraceId trace_id;
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
    auto invalid_ctx = SpanContext::GetInvalid();
    benchmark::DoNotOptimize(
        sampler.ShouldSample(invalid_ctx, trace_id, "", span_kind, view, links));
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
