#include "opentelemetry/sdk/trace/always_off_sampler.h"
#include "opentelemetry/nostd/span.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::nostd;

TEST(AlwaysOffSampler, ShouldSample)
{
	AlwaysOffSampler sampler;

	opentelemetry::trace::TraceId trace_id;

	auto sampling_result = sampler.ShouldSample(
		nullptr, trace_id, "Test", trace_api::SpanKind::kInternal, span<AttributeKeyValue>());

	ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(AlwaysOffSampler, GetDescription)
{
	AlwaysOffSampler sampler;

	ASSERT_EQ("AlwaysOffSampler", sampler.GetDescription());
}