#include "../../../exporters/memory/include/opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"
#include "opentelemetry/sdk/trace/samplers/probability.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/tracer.h"

#include <cstdint>

#include <benchmark/benchmark.h>

using namespace opentelemetry::sdk::trace;
using opentelemetry::exporter::memory::InMemorySpanExporter;

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

void BM_ParentOrElseSamplerConstruction(benchmark::State &state)
{
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(ParentOrElseSampler(std::make_shared<AlwaysOnSampler>()));
  }
}
BENCHMARK(BM_ParentOrElseSamplerConstruction);

void BM_ProbabilitySamplerConstruction(benchmark::State &state)
{
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(ProbabilitySampler(0.01));
  }
}
BENCHMARK(BM_ProbabilitySamplerConstruction);

// Sampler Helper Function
void BenchmarkShouldSampler(Sampler &sampler, benchmark::State &state)
{
  opentelemetry::trace::TraceId trace_id;
  opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

  using M = std::map<std::string, int>;
  M m1    = {{}};
  opentelemetry::trace::KeyValueIterableView<M> view{m1};

  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(sampler.ShouldSample(nullptr, trace_id, "", span_kind, view));
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

void BM_ParentOrElseSamplerShouldSample(benchmark::State &state)
{
  ParentOrElseSampler sampler(std::make_shared<AlwaysOnSampler>());

  BenchmarkShouldSampler(sampler, state);
}
BENCHMARK(BM_ParentOrElseSamplerShouldSample);

void BM_ProbabilitySamplerShouldSample(benchmark::State &state)
{
  ProbabilitySampler sampler(0.01);

  BenchmarkShouldSampler(sampler, state);
}
BENCHMARK(BM_ProbabilitySamplerShouldSample);

// Sampler Helper Function
void BenchmarkSpanCreation(std::shared_ptr<Sampler> sampler, benchmark::State &state)
{
  std::unique_ptr<SpanExporter> exporter(new InMemorySpanExporter());
  auto processor = std::make_shared<SimpleSpanProcessor>(std::move(exporter));
  auto tracer    = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor, sampler));

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
  BenchmarkSpanCreation(std::move(std::make_shared<AlwaysOnSampler>()), state);
}
BENCHMARK(BM_SpanCreation);

// Test to measure performance overhead for no-op span creation
void BM_NoopSpanCreation(benchmark::State &state)
{
  BenchmarkSpanCreation(std::move(std::make_shared<AlwaysOffSampler>()), state);
}
BENCHMARK(BM_NoopSpanCreation);

}  // namespace
BENCHMARK_MAIN();
