#include "opentelemetry/sdk/trace/always_off_sampler.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;

// Placeholder
class MockSpanContext
{}

TEST(AlwaysOffSampler, MockSpanContext)
{
	Sampler sampler(new AlwaysOffSampler());

	ASSERT_EQ("AlwaysOffSampler", sampler.GetDescription());

	// Placeholder
	MockSpanContext context(new MockSpanContext());

	constexpr uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 6, 5, 4, 3, 2, 1}
	opentelemetry::trace::TraceId trace_id(buf);

	auto sampling_result = sampler.ShouldSample(
		context, trace_id, "Test", SpanKind.kInternal, nostd::span());

	ASSERT_NE(nullptr, sampling_result);

	auto decision = sampling_result.GetDecision();

	ASSERT_EQ(Decision::NOT_RECORD, decision);

	auto attributes = sampling_result.GetAttributes();

	ASSERT_EQ(nostd::span(), attributes);
}