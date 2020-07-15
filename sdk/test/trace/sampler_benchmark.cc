#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"
#include "opentelemetry/sdk/trace/samplers/probability.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <cstdint>

#include <benchmark/benchmark.h>

using namespace opentelemetry::sdk::trace;
namespace nostd  = opentelemetry::nostd;
namespace common = opentelemetry::common;
using opentelemetry::trace::SpanContext;

/**
 * A mock sampler that returns non-empty sampling results attributes.
 */
class MockSampler final : public Sampler
{
public:
  SamplingResult ShouldSample(const SpanContext * /*parent_context*/,
                              trace_api::TraceId /*trace_id*/,
                              nostd::string_view /*name*/,
                              trace_api::SpanKind /*span_kind*/,
                              const trace_api::KeyValueIterable & /*attributes*/) noexcept override
  {
    // Return two pairs of attributes. These attributes should be added to the span attributes
    return {Decision::RECORD_AND_SAMPLE,
            nostd::unique_ptr<const std::map<std::string, opentelemetry::common::AttributeValue>>(
                new const std::map<std::string, opentelemetry::common::AttributeValue>(
                    {{"sampling_attr1", 123}, {"sampling_attr2", "string"}}))};
  }

  std::string GetDescription() const noexcept override { return "MockSampler"; }
};

/**
 * A mock exporter that switches a flag once a valid recordable was received.
 */
class MockSpanExporter final : public SpanExporter
{
public:
  MockSpanExporter(std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received) noexcept
      : spans_received_(spans_received)
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new SpanData);
  }

  ExportResult Export(const nostd::span<std::unique_ptr<Recordable>> &recordables) noexcept override
  {
    for (auto &recordable : recordables)
    {
      auto span = std::unique_ptr<SpanData>(static_cast<SpanData *>(recordable.release()));
      if (span != nullptr)
      {
        spans_received_->push_back(std::move(span));
      }
    }

    return ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {}

private:
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_;
};

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

// Sampler used as a baseline to compare with other samplers
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

// Sampler used as a baseline to compare with other samplers
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

// Test to measure performance for span creation
void BM_SpanCreation(benchmark::State &state)
{
	std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_off(
		new std::vector<std::unique_ptr<SpanData>>);

	std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(spans_received_off));
	auto processor = std::make_shared<SimpleSpanProcessor>(std::move(exporter));
	auto tracer_off = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor, std::make_shared<AlwaysOnSampler>()));

	while (state.KeepRunning())
	{
		auto span_off_1 = tracer_off->StartSpan("span with AlwaysOn sampler");

		span_off_1->SetAttribute("attr1", 3.1);  // Not recorded.

		span_off_1->End();
	}
}
BENCHMARK(BM_SpanCreation);

// Test to measure performance overhead for no-op span creation
void BM_NoopSpanCreation(benchmark::State &state)
{
	std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_off(
		new std::vector<std::unique_ptr<SpanData>>);

	std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(spans_received_off));
	auto processor = std::make_shared<SimpleSpanProcessor>(std::move(exporter));
	auto tracer_off = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor, std::make_shared<AlwaysOffSampler>()));

	while (state.KeepRunning())
	{
		auto span_off_1 = tracer_off->StartSpan("span with AlwaysOff sampler");

		span_off_1->SetAttribute("attr1", 3.1);  // Not recorded.

		span_off_1->End();
	}
}
BENCHMARK(BM_NoopSpanCreation);


} // namespace
BENCHMARK_MAIN();
