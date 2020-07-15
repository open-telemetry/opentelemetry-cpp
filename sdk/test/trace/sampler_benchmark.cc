#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"
#include "opentelemetry/sdk/trace/samplers/probability.h"

#include <cstdint>

#include <benchmark/benchmark.h>

namespace
{
using opentelemetry::sdk::trace::AlwaysOffSampler;
using opentelemetry::sdk::trace::AlwaysOnSampler;
using opentelemetry::sdk::trace::ParentOrElseSampler;
using opentelemetry::sdk::trace::ProbabilitySampler;
using opentelemetry::sdk::trace::Decision;
using opentelemetry::trace::SpanContext;

// NOOP sampler constructor used as a baseline to compare with other samplers
void BM_AlwaysOffSamplerConstruction(benchmark::State &state)
{
	while (state.KeepRunning())
	{
		benchmark::DoNotOptimize(AlwaysOffSampler());
	}
}
BENCHMARK(BM_AlwaysOffSamplerConstruction);

// NOOP sampler constructor used as a baseline to compare with other samplers
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

// NOOP sampler used as a baseline to compare with other samplers
void BM_AlwaysOffSamplerShouldSample(benchmark::State &state)
{
	AlwaysOffSampler sampler;

	opentelemetry::trace::TraceId trace_id;
	opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

	using M = std::map<std::string, int>;
	M m1 = {{}};
	opentelemetry::trace::KeyValueIterableView<M> view{m1};

	while (state.KeepRunning())
	{
		benchmark::DoNotOptimize(sampler.ShouldSample(nullptr, trace_id, "", span_kind, view));
	}
}
BENCHMARK(BM_AlwaysOffSamplerShouldSample);

// NOOP sampler used as a baseline to compare with other samplers
void BM_AlwaysOnSamplerShouldSample(benchmark::State &state)
{
	AlwaysOnSampler sampler;

	opentelemetry::trace::TraceId trace_id;
	opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

	using M = std::map<std::string, int>;
	M m1 = {{}};
	opentelemetry::trace::KeyValueIterableView<M> view{m1};

	while (state.KeepRunning())
	{
		benchmark::DoNotOptimize(sampler.ShouldSample(nullptr, trace_id, "", span_kind, view));
	}
}
BENCHMARK(BM_AlwaysOnSamplerShouldSample);

void BM_ParentOrElseSamplerShouldSample(benchmark::State &state)
{
	ParentOrElseSampler sampler(std::make_shared<AlwaysOnSampler>());

	opentelemetry::trace::TraceId trace_id;
	opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

	using M = std::map<std::string, int>;
	M m1 = {{}};
	opentelemetry::trace::KeyValueIterableView<M> view{m1};

	while (state.KeepRunning())
	{
		benchmark::DoNotOptimize(sampler.ShouldSample(nullptr, trace_id, "", span_kind, view));
	}
}
BENCHMARK(BM_ParentOrElseSamplerShouldSample);

void BM_ProbabilitySamplerShouldSample(benchmark::State &state)
{
	ProbabilitySampler sampler(0.01);

	opentelemetry::trace::TraceId trace_id;
	opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

	using M = std::map<std::string, int>;
	M m1 = {{}};
	opentelemetry::trace::KeyValueIterableView<M> view{m1};

	while (state.KeepRunning())
	{
		benchmark::DoNotOptimize(sampler.ShouldSample(nullptr, trace_id, "", span_kind, view));
	}
}
BENCHMARK(BM_ProbabilitySamplerShouldSample);

} // namespace
BENCHMARK_MAIN();
