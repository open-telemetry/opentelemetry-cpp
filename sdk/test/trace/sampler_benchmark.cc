// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off
//
//  ~/build/sdk/test/trace/sampler_benchmark --benchmark_repetitions=5 --benchmark_display_aggregates_only=true
// 2026-07-15T00:31:56+00:00
// Running /home/devuser/build/sdk/test/trace/sampler_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 7.64, 4.23, 2.79
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// -----------------------------------------------------------------------------------------
// Benchmark                                               Time             CPU   Iterations
// -----------------------------------------------------------------------------------------
// BM_AlwaysOffSamplerConstruction_mean                0.339 ns        0.338 ns            5
// BM_AlwaysOffSamplerConstruction_median              0.338 ns        0.338 ns            5
// BM_AlwaysOffSamplerConstruction_stddev              0.002 ns        0.002 ns            5
// BM_AlwaysOffSamplerConstruction_cv                   0.53 %          0.53 %             5
// BM_AlwaysOnSamplerConstruction_mean                 0.337 ns        0.337 ns            5
// BM_AlwaysOnSamplerConstruction_median               0.337 ns        0.337 ns            5
// BM_AlwaysOnSamplerConstruction_stddev               0.002 ns        0.002 ns            5
// BM_AlwaysOnSamplerConstruction_cv                    0.46 %          0.47 %             5
// BM_AlwaysOffSamplerShouldSample_mean                 4.18 ns         4.18 ns            5
// BM_AlwaysOffSamplerShouldSample_median               4.17 ns         4.17 ns            5
// BM_AlwaysOffSamplerShouldSample_stddev              0.031 ns        0.031 ns            5
// BM_AlwaysOffSamplerShouldSample_cv                   0.73 %          0.73 %             5
// BM_AlwaysOnSamplerShouldSample_mean                  4.33 ns         4.33 ns            5
// BM_AlwaysOnSamplerShouldSample_median                4.33 ns         4.33 ns            5
// BM_AlwaysOnSamplerShouldSample_stddev               0.049 ns        0.049 ns            5
// BM_AlwaysOnSamplerShouldSample_cv                    1.13 %          1.13 %             5
// BM_ParentBasedSamplerShouldSample_mean               7.07 ns         7.07 ns            5
// BM_ParentBasedSamplerShouldSample_median             7.06 ns         7.06 ns            5
// BM_ParentBasedSamplerShouldSample_stddev            0.022 ns        0.022 ns            5
// BM_ParentBasedSamplerShouldSample_cv                 0.32 %          0.32 %             5
// BM_TraceIdRatioBasedSamplerShouldSample_mean         3.94 ns         3.94 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_median       3.94 ns         3.94 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_stddev      0.012 ns        0.012 ns            5
// BM_TraceIdRatioBasedSamplerShouldSample_cv           0.31 %          0.31 %             5
// BM_SpanCreation_mean                                  235 ns          235 ns            5
// BM_SpanCreation_median                                235 ns          235 ns            5
// BM_SpanCreation_stddev                              0.616 ns        0.607 ns            5
// BM_SpanCreation_cv                                   0.26 %          0.26 %             5
// BM_NoopSpanCreation_mean                             59.3 ns         59.3 ns            5
// BM_NoopSpanCreation_median                           59.2 ns         59.2 ns            5
// BM_NoopSpanCreation_stddev                          0.421 ns        0.422 ns            5
// BM_NoopSpanCreation_cv                               0.71 %          0.71 %             5
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

}  // namespace
BENCHMARK_MAIN();
