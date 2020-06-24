#include "opentelemetry/sdk/trace/always_off_sampler.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::trace::AlwaysOffSampler;
using opentelemetry::sdk::trace::Decision;

TEST(AlwaysOffSampler, ShouldSample)
{
	AlwaysOffSampler sampler;

	opentelemetry::trace::TraceId trace_id;
	opentelemetry::trace::KeyValueIterableView<std::map<std::string, int>> view;
	opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

	auto sampling_result = sampler.ShouldSample(nullptr, trace_id, "Test", span_kind, view);

	ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(AlwaysOffSampler, GetDescription)
{
	AlwaysOffSampler sampler;

	ASSERT_EQ("AlwaysOffSampler", sampler.GetDescription());
}