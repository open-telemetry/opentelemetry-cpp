#include "opentelemetry/sdk/trace/samplers/probability.h"
#include "src/common/random.h"

#include <gtest/gtest.h>
#include <cstdlib>
#include <ctime>

using opentelemetry::sdk::trace::ProbabilitySampler;
using opentelemetry::sdk::trace::Decision;
using opentelemetry::sdk::common::Random;

namespace
{
	int RunShouldSampleCountDecision(
		opentelemetry::sdk::trace::Sampler::SpanContext& context,
		ProbabilitySampler& sampler, int iterations)
	{
		int actual_count = 0;

		opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

		using M = std::map<std::string, int>;
		M m1 = {{}};
		opentelemetry::trace::KeyValueIterableView<M> view{m1};

		srand(time(0));

		for (int i = 0; i < iterations; ++i)
		{
			uint8_t buf[16] = {0};
			Random::GenerateRandomBuffer(buf);

			opentelemetry::trace::TraceId trace_id(buf);

			auto result = sampler.ShouldSample(&context, trace_id, "", span_kind, view);
			if (result.decision == Decision::RECORD_AND_SAMPLE)
			{ ++actual_count; }
		}

		return actual_count;
	}
}

TEST(ProbabilitySampler, ShouldSampleWithoutContext)
{
	opentelemetry::trace::TraceId invalid_trace_id;

	opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

	using M = std::map<std::string, int>;
	M m1 = {{}};
	opentelemetry::trace::KeyValueIterableView<M> view{m1};

	ProbabilitySampler s1(0.01);

	auto sampling_result = s1.ShouldSample(nullptr, invalid_trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	constexpr uint8_t buf[] = {0, 0, 0, 0, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 0, 0, 0};
	opentelemetry::trace::TraceId valid_trace_id(buf);

	sampling_result = s1.ShouldSample(nullptr, valid_trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	ProbabilitySampler s2(0.50000001);

	sampling_result = s2.ShouldSample(nullptr, valid_trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	ProbabilitySampler s3(0.49999999);

	sampling_result = s3.ShouldSample(nullptr, valid_trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	ProbabilitySampler s4(0.50000000);

	sampling_result = s4.ShouldSample(nullptr, valid_trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(ProbabilitySampler, ShouldSampleWithContext)
{
	opentelemetry::trace::TraceId trace_id;
	opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;
	opentelemetry::sdk::trace::Sampler::SpanContext c1(false, false);
	opentelemetry::sdk::trace::Sampler::SpanContext c2(true, false);
	opentelemetry::sdk::trace::Sampler::SpanContext c3(false, true);
	opentelemetry::sdk::trace::Sampler::SpanContext c4(true, true);

	using M = std::map<std::string, int>;
	M m1 = {{}};
	opentelemetry::trace::KeyValueIterableView<M> view{m1};

 	ProbabilitySampler s1(0.01);

	auto sampling_result = s1.ShouldSample(&c1, trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	sampling_result = s1.ShouldSample(&c2, trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	sampling_result = s1.ShouldSample(&c3, trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);

	sampling_result = s1.ShouldSample(&c4, trace_id, "", span_kind, view);

	ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
	ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(ProbabilitySampler, ProbabilitySamplerHalf)
{
	double probability = 0.5;
	int iterations = 100000, expected_count = iterations * probability,
		variance = iterations * 0.01;

	opentelemetry::sdk::trace::Sampler::SpanContext c(true, true);
	ProbabilitySampler s(probability);

	int actual_count= RunShouldSampleCountDecision(c, s, iterations);

	ASSERT_TRUE(actual_count < (expected_count + variance));
	ASSERT_TRUE(actual_count > (expected_count - variance));
}

TEST(ProbabilitySampler, ProbabilitySamplerOnePercent)
{
	double probability = 0.01;
	int iterations = 100000, expected_count = iterations * probability,
		variance = iterations * 0.01;

	opentelemetry::sdk::trace::Sampler::SpanContext c(true, true);
	ProbabilitySampler s(probability);

	int actual_count= RunShouldSampleCountDecision(c, s, iterations);

	ASSERT_TRUE(actual_count < (expected_count + variance));
	ASSERT_TRUE(actual_count > (expected_count - variance));
}

TEST(ProbabilitySampler, ProbabilitySamplerAll)
{
	double probability = 1.0;
	int iterations = 100000, expected_count = iterations * probability;

	opentelemetry::sdk::trace::Sampler::SpanContext c(true, true);
	ProbabilitySampler s(probability);

	int actual_count= RunShouldSampleCountDecision(c, s, iterations);

	ASSERT_EQ(actual_count, expected_count);
}

TEST(ProbabilitySampler, ProbabilitySamplerNone)
{
	double probability = 0.0;
	int iterations = 100000, expected_count = iterations * probability;

	opentelemetry::sdk::trace::Sampler::SpanContext c(true, true);
	ProbabilitySampler s(probability);

	int actual_count= RunShouldSampleCountDecision(c, s, iterations);

	ASSERT_EQ(actual_count, expected_count);
}

TEST(ProbabilitySampler, GetDescription)
{
	ProbabilitySampler s1(0.01);
	ASSERT_EQ("ProbabilitySampler{0.010000}", s1.GetDescription());

	ProbabilitySampler s2(0.00);
	ASSERT_EQ("ProbabilitySampler{0.000000}", s2.GetDescription());

	ProbabilitySampler s3(1.00);
	ASSERT_EQ("ProbabilitySampler{1.000000}", s3.GetDescription());

	ProbabilitySampler s4(0.102030405);
	ASSERT_EQ("ProbabilitySampler{0.102030}", s4.GetDescription());

	ProbabilitySampler s5(3.00);
	ASSERT_EQ("ProbabilitySampler{1.000000}", s5.GetDescription());

	ProbabilitySampler s6(-3.00);
	ASSERT_EQ("ProbabilitySampler{0.000000}", s6.GetDescription());

	ProbabilitySampler s7(1.00000000001);
	ASSERT_EQ("ProbabilitySampler{1.000000}", s7.GetDescription());

	ProbabilitySampler s8(-1.00000000001);
	ASSERT_EQ("ProbabilitySampler{0.000000}", s8.GetDescription());

	ProbabilitySampler s9(0.50);
	ASSERT_EQ("ProbabilitySampler{0.500000}", s9.GetDescription());
}
