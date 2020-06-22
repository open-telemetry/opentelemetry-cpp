#include "opentelemetry/sdk/trace/always_off_sampler.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;

// Placeholder
class MockSpanContext
{}

TEST(AlwaysOffSampler, MockSpanContext)
{
	AlwaysOffSampler sampler();

	ASSERT_EQ("AlwaysOffSampler", sampler.GetDescription());

	// Placeholder
	MockSpanContext context();

	opentelemetry::trace::TraceId trace_id();

	auto sampling_result = sampler.ShouldSample(
		context, trace_id, "Test", SpanKind::kInternal, nostd::span());

	auto decision = sampling_result.GetDecision();

	ASSERT_EQ(Decision::NOT_RECORD, decision);

	auto attributes = sampling_result.GetAttributes();

	ASSERT_EQ(nostd::span(), attributes);
}