#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/nostd/span.h"

#include <gtest/gtest.h>
#include <memory>
#include <map>

using opentelemetry::sdk::trace::AlwaysOffSampler;
using opentelemetry::sdk::trace::AlwaysOnSampler;
using opentelemetry::sdk::trace::Decision;
using opentelemetry::sdk::trace::ParentOrElseSampler;

using namespace opentelemetry::sdk::trace;
using opentelemetry::core::SteadyTimestamp;
using opentelemetry::core::SystemTimestamp;
namespace nostd  = opentelemetry::nostd;
namespace common = opentelemetry::common;

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
                    {{"abc", 123}, {"def", 456}}))};
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
std::shared_ptr<opentelemetry::trace::Tracer> initTracer(
    std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> &received,
    std::shared_ptr<Sampler> sampler)
{
  std::unique_ptr<SpanExporter> exporter(new MockSpanExporter(received));
  auto processor = std::make_shared<SimpleSpanProcessor>(std::move(exporter));
  return std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor, sampler));
}
}  // namespace

TEST(Tracer, StartSpanWithSamplerAttributes)
{
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);
  // The default tracer has two elements in the sampling result attribute
  auto tracer_mock = initTracer(spans_received, std::make_shared<MockSampler>());
  {
    tracer_mock->StartSpan("span 3", {{"attr1", 314159}, {"attr2", false}});
  }
  auto &span_data3 = spans_received->at(0);
  // It has a total of four attributes: two from StartSpan and two from the sampler.
  ASSERT_EQ(4, span_data3->GetAttributes().size());
}

TEST(ParentOrElseSampler, ShouldSample)
{
  ParentOrElseSampler sampler_off(std::make_shared<AlwaysOffSampler>());
  ParentOrElseSampler sampler_on(std::make_shared<AlwaysOnSampler>());

  // Set up parameters
  opentelemetry::trace::TraceId trace_id;
  opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;
  using M = std::map<std::string, int>;
  M m1 = {{}};
  opentelemetry::trace::KeyValueIterableView<M> view{m1};
  opentelemetry::sdk::trace::Sampler::SpanContext parent_context_sampled(true, true);
  opentelemetry::sdk::trace::Sampler::SpanContext parent_context_nonsampled(true, false);

  // Case 1: Parent doesn't exist. Return result of delegateSampler()
  auto sampling_result = sampler_off.ShouldSample(nullptr, trace_id, "", span_kind, view);
  auto sampling_result2 = sampler_on.ShouldSample(nullptr, trace_id, "", span_kind, view);

  ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);
  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result2.decision);

  // Case 2: Parent exists and SampledFlag is true
  auto sampling_result3 =
      sampler_off.ShouldSample(&parent_context_sampled, trace_id, "", span_kind, view);
  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result3.decision);

  // Case 3: Parent exists and SampledFlag is false
  auto sampling_result4 =
      sampler_on.ShouldSample(&parent_context_nonsampled, trace_id, "", span_kind, view);
  ASSERT_EQ(Decision::NOT_RECORD, sampling_result4.decision);
}

TEST(ParentOrElseSampler, GetDescription)
{
  ParentOrElseSampler sampler(std::make_shared<AlwaysOffSampler>());
  ASSERT_EQ("ParentOrElse{AlwaysOffSampler}", sampler.GetDescription());
  ParentOrElseSampler sampler2(std::make_shared<AlwaysOnSampler>());
  ASSERT_EQ("ParentOrElse{AlwaysOnSampler}", sampler2.GetDescription());
}

TEST(AlwaysOffSampler, ShouldSample)
{
	AlwaysOffSampler sampler;

	opentelemetry::trace::TraceId trace_id;
	opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

	using M = std::map<std::string, int>;
	M m1 = {{}};
	opentelemetry::trace::KeyValueIterableView<M> view{m1};

	auto sampling_result = sampler.ShouldSample(nullptr, trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(AlwaysOffSampler, GetDescription)
{
	AlwaysOffSampler sampler;

	ASSERT_EQ("AlwaysOffSampler", sampler.GetDescription());
}

TEST(AlwaysOnSampler, ShouldSample)
{
  AlwaysOnSampler sampler;

  // A buffer of trace_id with no specific meaning
  constexpr uint8_t buf[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};

  trace_api::TraceId trace_id_invalid;
  trace_api::TraceId trace_id_valid(buf);
  std::map<std::string, int> key_value_container = {{"key", 0}};

  // Test with invalid (empty) trace id and empty parent context
  auto sampling_result = sampler.ShouldSample(
      nullptr, trace_id_invalid, "invalid trace id test", trace_api::SpanKind::kServer,
      trace_api::KeyValueIterableView<std::map<std::string, int>>(key_value_container));

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);

  // Test with a valid trace id and empty parent context
  sampling_result = sampler.ShouldSample(
      nullptr, trace_id_valid, "valid trace id test", trace_api::SpanKind::kServer,
      trace_api::KeyValueIterableView<std::map<std::string, int>>(key_value_container));

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(AlwaysOnSampler, GetDescription)
{
  AlwaysOnSampler sampler;

  ASSERT_EQ("AlwaysOnSampler", sampler.GetDescription());
}
