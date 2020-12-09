#include "opentelemetry/sdk/trace/samplers/trace_id_ratio.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "src/common/random.h"

#include <gtest/gtest.h>
#include <cstdlib>
#include <ctime>

using opentelemetry::sdk::common::Random;
using opentelemetry::sdk::trace::Decision;
using opentelemetry::sdk::trace::TraceIdRatioBasedSampler;
namespace trace_api = opentelemetry::trace;

namespace
{
/*
 * Helper function for running TraceIdBased sampler tests.
 * Given a span context, sampler, and number of iterations this function
 * will return the number of RECORD_AND_SAMPLE decision based on randomly
 * generated traces.
 *
 * @param context a required valid span context
 * @param sampler a required valid sampler
 * @param iterations a requried number specifying the number of times to
 * generate a random trace_id and check if it should sample using the provided
 * provider and context
 */
int RunShouldSampleCountDecision(trace_api::SpanContext &context,
                                 TraceIdRatioBasedSampler &sampler,
                                 int iterations)
{
  int actual_count = 0;

  opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

  using M = std::map<std::string, int>;
  M m1    = {{}};

  using L = std::vector<std::pair<trace_api::SpanContext, std::map<std::string, std::string>>>;
  L l1 = {{trace_api::SpanContext(false, false), {}}, {trace_api::SpanContext(false, false), {}}};

  opentelemetry::common::KeyValueIterableView<M> view{m1};
  trace_api::SpanContextKeyValueIterableView<L> links{l1};

  for (int i = 0; i < iterations; ++i)
  {
    uint8_t buf[16] = {0};
    Random::GenerateRandomBuffer(buf);

    opentelemetry::trace::TraceId trace_id(buf);

    auto result = sampler.ShouldSample(context, trace_id, "", span_kind, view, links);
    if (result.decision == Decision::RECORD_AND_SAMPLE)
    {
      ++actual_count;
    }
  }

  return actual_count;
}
}  // namespace

TEST(TraceIdRatioBasedSampler, ShouldSampleWithoutContext)
{
  opentelemetry::trace::TraceId invalid_trace_id;

  opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

  using M = std::map<std::string, int>;
  M m1    = {{}};

  using L = std::vector<std::pair<trace_api::SpanContext, std::map<std::string, std::string>>>;
  L l1 = {{trace_api::SpanContext(false, false), {}}, {trace_api::SpanContext(false, false), {}}};

  opentelemetry::common::KeyValueIterableView<M> view{m1};
  trace_api::SpanContextKeyValueIterableView<L> links{l1};

  TraceIdRatioBasedSampler s1(0.01);

  auto sampling_result = s1.ShouldSample(trace_api::SpanContext::GetInvalid(), invalid_trace_id, "",
                                         span_kind, view, links);

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);

  constexpr uint8_t buf[] = {0, 0, 0, 0, 0, 0, 0, 0x80, 0, 0, 0, 0, 0, 0, 0, 0};
  opentelemetry::trace::TraceId valid_trace_id(buf);

  sampling_result = s1.ShouldSample(trace_api::SpanContext::GetInvalid(), valid_trace_id, "",
                                    span_kind, view, links);

  ASSERT_EQ(Decision::DROP, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);

  TraceIdRatioBasedSampler s2(0.50000001);

  sampling_result = s2.ShouldSample(trace_api::SpanContext::GetInvalid(), valid_trace_id, "",
                                    span_kind, view, links);

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);

  TraceIdRatioBasedSampler s3(0.49999999);

  sampling_result = s3.ShouldSample(trace_api::SpanContext::GetInvalid(), valid_trace_id, "",
                                    span_kind, view, links);

  ASSERT_EQ(Decision::DROP, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);

  TraceIdRatioBasedSampler s4(0.50000000);

  sampling_result = s4.ShouldSample(trace_api::SpanContext::GetInvalid(), valid_trace_id, "",
                                    span_kind, view, links);

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(TraceIdRatioBasedSampler, ShouldSampleWithContext)
{

  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;
  trace_api::SpanContext c1(trace_id, span_id, trace_api::TraceFlags{0}, false);
  trace_api::SpanContext c2(trace_id, span_id, trace_api::TraceFlags{1}, false);
  trace_api::SpanContext c3(trace_id, span_id, trace_api::TraceFlags{0}, true);
  trace_api::SpanContext c4(trace_id, span_id, trace_api::TraceFlags{1}, true);

  using M = std::map<std::string, int>;
  M m1    = {{}};

  using L = std::vector<std::pair<trace_api::SpanContext, std::map<std::string, std::string>>>;
  L l1 = {{trace_api::SpanContext(false, false), {}}, {trace_api::SpanContext(false, false), {}}};

  opentelemetry::common::KeyValueIterableView<M> view{m1};
  trace_api::SpanContextKeyValueIterableView<L> links{l1};

  TraceIdRatioBasedSampler s1(0.01);

  auto sampling_result = s1.ShouldSample(c1, trace_id, "", span_kind, view, links);

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);

  sampling_result = s1.ShouldSample(c2, trace_id, "", span_kind, view, links);

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);

  sampling_result = s1.ShouldSample(c3, trace_id, "", span_kind, view, links);

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);

  sampling_result = s1.ShouldSample(c4, trace_id, "", span_kind, view, links);

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(TraceIdRatioBasedSampler, TraceIdRatioBasedSamplerHalf)
{
  double ratio       = 0.5;
  int iterations     = 100000;
  int expected_count = static_cast<int>(iterations * ratio);
  int variance       = static_cast<int>(iterations * 0.01);

  trace_api::SpanContext c(true, true);
  TraceIdRatioBasedSampler s(ratio);

  int actual_count = RunShouldSampleCountDecision(c, s, iterations);

  ASSERT_TRUE(actual_count < (expected_count + variance));
  ASSERT_TRUE(actual_count > (expected_count - variance));
}

TEST(TraceIdRatioBasedSampler, TraceIdRatioBasedSamplerOnePercent)
{
  double ratio       = 0.01;
  int iterations     = 100000;
  int expected_count = static_cast<int>(iterations * ratio);
  int variance       = static_cast<int>(iterations * 0.01);

  trace_api::SpanContext c(true, true);
  TraceIdRatioBasedSampler s(ratio);

  int actual_count = RunShouldSampleCountDecision(c, s, iterations);

  ASSERT_TRUE(actual_count < (expected_count + variance));
  ASSERT_TRUE(actual_count > (expected_count - variance));
}

TEST(TraceIdRatioBasedSampler, TraceIdRatioBasedSamplerAll)
{
  double ratio       = 1.0;
  int iterations     = 100000;
  int expected_count = static_cast<int>(iterations * ratio);

  trace_api::SpanContext c(true, true);
  TraceIdRatioBasedSampler s(ratio);

  int actual_count = RunShouldSampleCountDecision(c, s, iterations);

  ASSERT_EQ(actual_count, expected_count);
}

TEST(TraceIdRatioBasedSampler, TraceIdRatioBasedSamplerNone)
{
  double ratio       = 0.0;
  int iterations     = 100000;
  int expected_count = static_cast<int>(iterations * ratio);

  trace_api::SpanContext c(true, true);
  TraceIdRatioBasedSampler s(ratio);

  int actual_count = RunShouldSampleCountDecision(c, s, iterations);

  ASSERT_EQ(actual_count, expected_count);
}

TEST(TraceIdRatioBasedSampler, GetDescription)
{
  TraceIdRatioBasedSampler s1(0.01);
  ASSERT_EQ("TraceIdRatioBasedSampler{0.010000}", s1.GetDescription());

  TraceIdRatioBasedSampler s2(0.00);
  ASSERT_EQ("TraceIdRatioBasedSampler{0.000000}", s2.GetDescription());

  TraceIdRatioBasedSampler s3(1.00);
  ASSERT_EQ("TraceIdRatioBasedSampler{1.000000}", s3.GetDescription());

  TraceIdRatioBasedSampler s4(0.102030405);
  ASSERT_EQ("TraceIdRatioBasedSampler{0.102030}", s4.GetDescription());

  TraceIdRatioBasedSampler s5(3.00);
  ASSERT_EQ("TraceIdRatioBasedSampler{1.000000}", s5.GetDescription());

  TraceIdRatioBasedSampler s6(-3.00);
  ASSERT_EQ("TraceIdRatioBasedSampler{0.000000}", s6.GetDescription());

  TraceIdRatioBasedSampler s7(1.00000000001);
  ASSERT_EQ("TraceIdRatioBasedSampler{1.000000}", s7.GetDescription());

  TraceIdRatioBasedSampler s8(-1.00000000001);
  ASSERT_EQ("TraceIdRatioBasedSampler{0.000000}", s8.GetDescription());

  TraceIdRatioBasedSampler s9(0.50);
  ASSERT_EQ("TraceIdRatioBasedSampler{0.500000}", s9.GetDescription());
}
