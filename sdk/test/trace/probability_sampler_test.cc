// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stdint.h>
#include <cmath>
#include <initializer_list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/probability.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/trace_state.h"
#include "src/common/random.h"

using opentelemetry::sdk::common::Random;
using opentelemetry::sdk::trace::Decision;
using opentelemetry::sdk::trace::ProbabilitySampler;
namespace trace_api = opentelemetry::trace;
namespace common    = opentelemetry::common;

namespace
{
/*
 * Helper function for building a trace id whose rightmost 7 bytes hold the
 * given 56-bit randomness value.
 */
trace_api::TraceId TraceIdWithRandomness(uint64_t randomness)
{
  uint8_t buf[trace_api::TraceId::kSize] = {0};
  for (int i = 0; i < 7; ++i)
  {
    buf[15 - i] = static_cast<uint8_t>(randomness >> (8 * i));
  }
  return trace_api::TraceId(buf);
}

/*
 * Helper function for running ProbabilitySampler tests.
 * Given a span context, sampler, and number of iterations this function
 * will return the number of RECORD_AND_SAMPLE decision based on randomly
 * generated traces.
 */
int RunShouldSampleCountDecision(trace_api::SpanContext &context,
                                 ProbabilitySampler &sampler,
                                 int iterations)
{
  int actual_count = 0;

  trace_api::SpanKind span_kind = trace_api::SpanKind::kInternal;

  using M = std::map<std::string, int>;
  M m1    = {{}};

  using L = std::vector<std::pair<trace_api::SpanContext, std::map<std::string, std::string>>>;
  L l1 = {{trace_api::SpanContext(false, false), {}}, {trace_api::SpanContext(false, false), {}}};

  common::KeyValueIterableView<M> view{m1};
  trace_api::SpanContextKeyValueIterableView<L> links{l1};

  for (int i = 0; i < iterations; ++i)
  {
    uint8_t buf[16] = {0};
    Random::GenerateRandomBuffer(buf);

    trace_api::TraceId trace_id(buf);

    auto result = sampler.ShouldSample(context, trace_id, "", span_kind, view, links);
    if (result.decision == Decision::RECORD_AND_SAMPLE)
    {
      ++actual_count;
    }
  }

  return actual_count;
}

opentelemetry::sdk::trace::SamplingResult SampleWithContext(ProbabilitySampler &sampler,
                                                            const trace_api::SpanContext &context,
                                                            trace_api::TraceId trace_id)
{
  trace_api::SpanKind span_kind = trace_api::SpanKind::kInternal;

  using M = std::map<std::string, int>;
  M m1    = {{}};

  using L = std::vector<std::pair<trace_api::SpanContext, std::map<std::string, std::string>>>;
  L l1 = {{trace_api::SpanContext(false, false), {}}, {trace_api::SpanContext(false, false), {}}};

  common::KeyValueIterableView<M> view{m1};
  trace_api::SpanContextKeyValueIterableView<L> links{l1};

  return sampler.ShouldSample(context, trace_id, "", span_kind, view, links);
}
}  // namespace

TEST(ProbabilitySampler, ShouldSampleAlways)
{
  ProbabilitySampler s1(1.0);

  auto sampling_result =
      SampleWithContext(s1, trace_api::SpanContext::GetInvalid(), TraceIdWithRandomness(0));

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);
  ASSERT_NE(nullptr, sampling_result.trace_state);

  std::string th_value;
  ASSERT_TRUE(sampling_result.trace_state->Get("ot", th_value));
  ASSERT_EQ("th:0", th_value);
}

TEST(ProbabilitySampler, ShouldSampleNever)
{
  ProbabilitySampler s1(0.0);

  auto sampling_result = SampleWithContext(s1, trace_api::SpanContext::GetInvalid(),
                                           TraceIdWithRandomness(0xffffffffffffff));

  ASSERT_EQ(Decision::DROP, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);
  ASSERT_NE(nullptr, sampling_result.trace_state);
  std::string ot_value;
  ASSERT_FALSE(sampling_result.trace_state->Get("ot", ot_value));
}

TEST(ProbabilitySampler, InvalidRatioFallsBackToDefault)
{
  // Invalid ratios (out of range, below the 2^-56 minimum, or NaN) fall back
  // to the default of 1.0.
  for (double ratio : {-0.5, 1.5, std::ldexp(1.0, -57), std::nan("")})
  {
    ProbabilitySampler s1(ratio);
    ASSERT_EQ("ProbabilitySampler{1.000000}", s1.GetDescription());

    auto sampling_result =
        SampleWithContext(s1, trace_api::SpanContext::GetInvalid(), TraceIdWithRandomness(0));
    ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  }
}

TEST(ProbabilitySampler, ShouldSampleAtThreshold)
{
  // For ratio 0.5 the rejection threshold is 2^55 (0x80000000000000).
  ProbabilitySampler s1(0.5);

  auto sampling_result = SampleWithContext(s1, trace_api::SpanContext::GetInvalid(),
                                           TraceIdWithRandomness(0x80000000000000));

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);

  std::string th_value;
  ASSERT_TRUE(sampling_result.trace_state->Get("ot", th_value));
  ASSERT_EQ("th:8", th_value);

  sampling_result = SampleWithContext(s1, trace_api::SpanContext::GetInvalid(),
                                      TraceIdWithRandomness(0x7fffffffffffff));

  ASSERT_EQ(Decision::DROP, sampling_result.decision);
  ASSERT_NE(nullptr, sampling_result.trace_state);
  std::string dropped_ot;
  ASSERT_FALSE(sampling_result.trace_state->Get("ot", dropped_ot));
}

TEST(ProbabilitySampler, ExplicitRandomnessTakesPrecedence)
{
  ProbabilitySampler s1(0.5);

  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  // The rv sub-key wins over the trace id randomness, which would drop here.
  auto trace_state = trace_api::TraceState::FromHeader("ot=rv:ffffffffffffff");
  trace_api::SpanContext context(trace_id, span_id, trace_api::TraceFlags{0}, false, trace_state);

  auto sampling_result = SampleWithContext(s1, context, TraceIdWithRandomness(0));

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);

  std::string ot_value;
  ASSERT_TRUE(sampling_result.trace_state->Get("ot", ot_value));
  ASSERT_EQ("th:8;rv:ffffffffffffff", ot_value);

  // An rv of zero drops even when the trace id randomness would sample.
  trace_state = trace_api::TraceState::FromHeader("ot=rv:00000000000000");
  trace_api::SpanContext context2(trace_id, span_id, trace_api::TraceFlags{0}, false, trace_state);

  sampling_result = SampleWithContext(s1, context2, TraceIdWithRandomness(0xffffffffffffff));

  ASSERT_EQ(Decision::DROP, sampling_result.decision);
}

TEST(ProbabilitySampler, InvalidExplicitRandomnessIsIgnored)
{
  ProbabilitySampler s1(0.5);

  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  // Wrong length, invalid characters: fall back to the trace id randomness.
  for (const char *header : {"ot=rv:ffffffffffff", "ot=rv:ffffffffffffffff", "ot=rv:FFFFFFFFFFFFFF",
                             "ot=rv:gggggggggggggg", "ot=th:8"})
  {
    auto trace_state = trace_api::TraceState::FromHeader(header);
    trace_api::SpanContext context(trace_id, span_id, trace_api::TraceFlags{0}, false, trace_state);

    auto sampling_result = SampleWithContext(s1, context, TraceIdWithRandomness(0xffffffffffffff));
    ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision) << header;

    sampling_result = SampleWithContext(s1, context, TraceIdWithRandomness(0));
    ASSERT_EQ(Decision::DROP, sampling_result.decision) << header;
  }
}

TEST(ProbabilitySampler, ThresholdReplacesExistingValue)
{
  // For ratio 0.25 the rejection threshold is 0xc0000000000000.
  ProbabilitySampler s1(0.25);

  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  auto trace_state = trace_api::TraceState::FromHeader("ot=th:8;rv:ffffffffffffff,foo=bar");
  trace_api::SpanContext context(trace_id, span_id, trace_api::TraceFlags{0}, false, trace_state);

  auto sampling_result = SampleWithContext(s1, context, TraceIdWithRandomness(0));

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);

  std::string ot_value;
  ASSERT_TRUE(sampling_result.trace_state->Get("ot", ot_value));
  ASSERT_EQ("th:c;rv:ffffffffffffff", ot_value);

  std::string foo_value;
  ASSERT_TRUE(sampling_result.trace_state->Get("foo", foo_value));
  ASSERT_EQ("bar", foo_value);
}

TEST(ProbabilitySampler, DropClearsStaleThreshold)
{
  ProbabilitySampler s1(0.5);

  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  // rv of zero is below the 0.5 threshold, so the span is dropped.
  auto trace_state =
      trace_api::TraceState::FromHeader("ot=th:8;rv:00000000000000;vendor:xyz,foo=bar");
  trace_api::SpanContext context(trace_id, span_id, trace_api::TraceFlags{0}, false, trace_state);

  auto sampling_result = SampleWithContext(s1, context, TraceIdWithRandomness(0));

  ASSERT_EQ(Decision::DROP, sampling_result.decision);
  ASSERT_NE(nullptr, sampling_result.trace_state);

  std::string ot_value;
  ASSERT_TRUE(sampling_result.trace_state->Get("ot", ot_value));
  ASSERT_EQ("rv:00000000000000;vendor:xyz", ot_value);

  std::string foo_value;
  ASSERT_TRUE(sampling_result.trace_state->Get("foo", foo_value));
  ASSERT_EQ("bar", foo_value);
}

TEST(ProbabilitySampler, DropDeletesOtWhenOnlyThreshold)
{
  // ot=th:8 with no other sub-keys: stripping th leaves an empty value, so the ot key is deleted.
  ProbabilitySampler s1(0.0);

  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  auto trace_state = trace_api::TraceState::FromHeader("ot=th:8,foo=bar");
  trace_api::SpanContext context(trace_id, span_id, trace_api::TraceFlags{0}, false, trace_state);

  auto sampling_result = SampleWithContext(s1, context, TraceIdWithRandomness(0));

  ASSERT_EQ(Decision::DROP, sampling_result.decision);
  ASSERT_NE(nullptr, sampling_result.trace_state);

  std::string ot_value;
  ASSERT_FALSE(sampling_result.trace_state->Get("ot", ot_value));

  std::string foo_value;
  ASSERT_TRUE(sampling_result.trace_state->Get("foo", foo_value));
  ASSERT_EQ("bar", foo_value);
}

TEST(ProbabilitySampler, MalformedSubKeysAreDropped)
{
  ProbabilitySampler s1(0.5);

  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  // Empty, key-only, and duplicate th sub-keys are dropped from the output.
  for (auto p : {std::make_pair("ot=a:1;;rv:ffffffffffffff", "th:8;rv:ffffffffffffff;a:1"),
                 std::make_pair("ot=foo", "th:8"), std::make_pair("ot=th:1;th:2", "th:8")})
  {
    auto trace_state = trace_api::TraceState::FromHeader(p.first);
    trace_api::SpanContext context(trace_id, span_id, trace_api::TraceFlags{0}, false, trace_state);

    auto sampling_result = SampleWithContext(s1, context, TraceIdWithRandomness(0xffffffffffffff));

    ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision) << p.first;

    std::string ot_value;
    ASSERT_TRUE(sampling_result.trace_state->Get("ot", ot_value)) << p.first;
    ASSERT_EQ(p.second, ot_value) << p.first;
  }
}

TEST(ProbabilitySampler, FullTraceStateKeepsParentTraceState)
{
  ProbabilitySampler s1(0.5);

  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  // 32 entries and no ot entry: there is no room to add th, so the span is
  // sampled and the parent entries are preserved without an ot entry.
  std::string header = "k0=0";
  for (int i = 1; i < 32; ++i)
  {
    header += ",k" + std::to_string(i) + "=" + std::to_string(i);
  }
  auto trace_state = trace_api::TraceState::FromHeader(header);
  trace_api::SpanContext context(trace_id, span_id, trace_api::TraceFlags{0}, false, trace_state);

  auto sampling_result = SampleWithContext(s1, context, TraceIdWithRandomness(0xffffffffffffff));

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_NE(nullptr, sampling_result.trace_state);

  std::string ot_value;
  ASSERT_FALSE(sampling_result.trace_state->Get("ot", ot_value));

  std::string k0_value;
  ASSERT_TRUE(sampling_result.trace_state->Get("k0", k0_value));
  ASSERT_EQ("0", k0_value);
}

TEST(ProbabilitySampler, ThresholdOmittedWhenSubKeyWouldOverflow)
{
  ProbabilitySampler s1(0.5);

  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  // Adding th would push the ot value past the 256 char limit; the inherited
  // foreign sub-key must be preserved, so th is omitted.
  std::string ot_value = "a:" + std::string(253, 'b');
  auto trace_state     = trace_api::TraceState::FromHeader("ot=" + ot_value);
  trace_api::SpanContext context(trace_id, span_id, trace_api::TraceFlags{0}, false, trace_state);

  auto sampling_result = SampleWithContext(s1, context, TraceIdWithRandomness(0xffffffffffffff));

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_NE(nullptr, sampling_result.trace_state);

  std::string result_ot;
  ASSERT_TRUE(sampling_result.trace_state->Get("ot", result_ot));
  ASSERT_EQ(ot_value, result_ot);
}

TEST(ProbabilitySampler, StaleThresholdRemovedWhenSubKeyWouldOverflow)
{
  ProbabilitySampler s1(0.1);

  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  // The inherited th is stale and must go; this sampler's own th does not fit
  // next to the foreign sub-key, which must be preserved, so no th is
  // recorded.
  std::string rest = "x:" + std::string(245, 'b');
  auto trace_state = trace_api::TraceState::FromHeader("ot=th:8;" + rest);
  trace_api::SpanContext context(trace_id, span_id, trace_api::TraceFlags{0}, false, trace_state);

  auto sampling_result = SampleWithContext(s1, context, TraceIdWithRandomness(0xffffffffffffff));

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_NE(nullptr, sampling_result.trace_state);

  std::string result_ot;
  ASSERT_TRUE(sampling_result.trace_state->Get("ot", result_ot));
  ASSERT_EQ(std::string::npos, result_ot.find("th:"));
  ASSERT_EQ(rest, result_ot);
}

TEST(ProbabilitySampler, IgnoresParentSampledFlag)
{
  ProbabilitySampler s1(1.0);

  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  // Parent not sampled, ratio 1: the parent flag must be ignored.
  trace_api::SpanContext context(trace_id, span_id, trace_api::TraceFlags{0}, true);

  auto sampling_result = SampleWithContext(s1, context, TraceIdWithRandomness(0));
  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);

  // Parent sampled, ratio 0: same.
  ProbabilitySampler s2(0.0);
  trace_api::SpanContext context2(trace_id, span_id,
                                  trace_api::TraceFlags{trace_api::TraceFlags::kIsSampled}, true);

  sampling_result = SampleWithContext(s2, context2, TraceIdWithRandomness(0xffffffffffffff));
  ASSERT_EQ(Decision::DROP, sampling_result.decision);
}

TEST(ProbabilitySampler, ProbabilitySamplerHalf)
{
  double ratio       = 0.5;
  int iterations     = 100000;
  int expected_count = static_cast<int>(iterations * ratio);
  int variance       = static_cast<int>(iterations * 0.01);

  trace_api::SpanContext c(true, true);
  ProbabilitySampler s(ratio);

  int actual_count = RunShouldSampleCountDecision(c, s, iterations);

  ASSERT_TRUE(actual_count < (expected_count + variance));
  ASSERT_TRUE(actual_count > (expected_count - variance));
}

TEST(ProbabilitySampler, ProbabilitySamplerOnePercent)
{
  double ratio       = 0.01;
  int iterations     = 100000;
  int expected_count = static_cast<int>(iterations * ratio);
  int variance       = static_cast<int>(iterations * 0.01);

  trace_api::SpanContext c(true, true);
  ProbabilitySampler s(ratio);

  int actual_count = RunShouldSampleCountDecision(c, s, iterations);

  ASSERT_TRUE(actual_count < (expected_count + variance));
  ASSERT_TRUE(actual_count > (expected_count - variance));
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
  ASSERT_EQ("ProbabilitySampler{1.000000}", s5.GetDescription());

  ProbabilitySampler s6(std::nan(""));
  ASSERT_EQ("ProbabilitySampler{1.000000}", s6.GetDescription());
}
