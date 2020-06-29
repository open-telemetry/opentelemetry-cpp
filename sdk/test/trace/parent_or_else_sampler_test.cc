#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::trace::ParentOrElseSampler;
using opentelemetry::sdk::trace::Decision;

TEST(ParentOrElseSampler, ShouldSample)
{
	ParentOrElseSampler sampler;

	opentelemetry::trace::TraceId trace_id;
	opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

	using M = std::map<std::string, int>;
	M m1 = {{}};
	opentelemetry::trace::KeyValueIterableView<M> view{m1};

	auto sampling_result = sampler.ShouldSample(nullptr, trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(ParentOrElseSampler, GetDescription)
{
	ParentOrElseSampler sampler;

	ASSERT_EQ("AlwaysOffSampler", sampler.GetDescription());
}
