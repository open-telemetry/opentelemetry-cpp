#include "opentelemetry/sdk/trace/probability_sampler.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::trace::ProbabilitySampler;
using opentelemetry::sdk::trace::Decision;

TEST(ProbabilitySampler, ShouldSampleWithoutContext)
{
	ProbabilitySampler s1(0.01);

	opentelemetry::trace::TraceId invalid_trace_id;

	opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

	using M = std::map<std::string, int>;
	M m1 = {{}};
	opentelemetry::trace::KeyValueIterableView<M> view{m1};

	auto sampling_result = s1.ShouldSample(nullptr, invalid_trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	constexpr uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 6, 5, 4, 3, 2, 1};
	opentelemetry::trace::TraceId valid_trace_id(buf);

	sampling_result = s1.ShouldSample(nullptr, valid_trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	ProbabilitySampler s2(0.123457);

	sampling_result = s2.ShouldSample(nullptr, valid_trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(ProbabilitySampler, ShouldSampleWithContext)
{
	ProbabilitySampler s1(0.01);

	opentelemetry::trace::TraceId trace_id;
	opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;
	opentelemetry::sdk::trace::Sampler::SpanContext c1(false, false);
	opentelemetry::sdk::trace::Sampler::SpanContext c2(false, true);
	opentelemetry::sdk::trace::Sampler::SpanContext c3(true, false);
	opentelemetry::sdk::trace::Sampler::SpanContext c4(true, true);

	using M = std::map<std::string, int>;
	M m1 = {{}};
	opentelemetry::trace::KeyValueIterableView<M> view{m1};

	auto sampling_result = s1.ShouldSample(&c1, trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	sampling_result = s1.ShouldSample(&c2, trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	sampling_result = s1.ShouldSample(&c3, trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::RECORD, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	sampling_result = s1.ShouldSample(&c4, trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(ProbabilitySampler, GetDescription)
{
	ProbabilitySampler s1(0.01);
	ASSERT_EQ("ProbabilitySampler{0.010000}", s1.GetDescription());

	ProbabilitySampler s2(0.00);
	ASSERT_EQ("ProbabilitySampler{0.000000}", s2.GetDescription());

	ProbabilitySampler s3(1.00);
	ASSERT_EQ("ProbabilitySampler{1.000000}", s3.GetDescription());

	ProbabilitySampler s4(3.00);
	ASSERT_EQ("ProbabilitySampler{1.000000}", s4.GetDescription());

	ProbabilitySampler s5(-3.00);
	ASSERT_EQ("ProbabilitySampler{0.000000}", s5.GetDescription());

	ProbabilitySampler s6(0.102030405);
	ASSERT_EQ("ProbabilitySampler{0.102030}", s6.GetDescription());
}