#include <gtest/gtest.h>
#include <memory>
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/parent.h"

using opentelemetry::sdk::trace::AlwaysOffSampler;
using opentelemetry::sdk::trace::AlwaysOnSampler;
using opentelemetry::sdk::trace::Decision;
using opentelemetry::sdk::trace::ParentBasedSampler;
namespace trace_api = opentelemetry::trace;

TEST(ParentBasedSampler, ShouldSample)
{
  ParentBasedSampler sampler_off(std::make_shared<AlwaysOffSampler>());
  ParentBasedSampler sampler_on(std::make_shared<AlwaysOnSampler>());

  // Set up parameters
  uint8_t trace_id_buffer[trace_api::TraceId::kSize] = {1};
  trace_api::TraceId trace_id{trace_id_buffer};
  uint8_t span_id_buffer[trace_api::SpanId::kSize] = {1};
  trace_api::SpanId span_id{span_id_buffer};

  opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;
  using M                                  = std::map<std::string, int>;
  M m1                                     = {{}};
  opentelemetry::trace::KeyValueIterableView<M> view{m1};
  trace_api::SpanContext parent_context_sampled(trace_id, span_id, trace_api::TraceFlags{1}, false);
  trace_api::SpanContext parent_context_nonsampled(trace_id, span_id, trace_api::TraceFlags{0},
                                                   false);

  // Case 1: Parent doesn't exist. Return result of delegateSampler()
  auto sampling_result =
      sampler_off.ShouldSample(trace_api::SpanContext::GetInvalid(), trace_id, "", span_kind, view);
  auto sampling_result2 =
      sampler_on.ShouldSample(trace_api::SpanContext::GetInvalid(), trace_id, "", span_kind, view);

  ASSERT_EQ(Decision::DROP, sampling_result.decision);
  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result2.decision);

  // Case 2: Parent exists and SampledFlag is true
  auto sampling_result3 =
      sampler_off.ShouldSample(parent_context_sampled, trace_id, "", span_kind, view);
  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result3.decision);

  // Case 3: Parent exists and SampledFlag is false
  auto sampling_result4 =
      sampler_on.ShouldSample(parent_context_nonsampled, trace_id, "", span_kind, view);
  ASSERT_EQ(Decision::DROP, sampling_result4.decision);
}

TEST(ParentBasedSampler, GetDescription)
{
  ParentBasedSampler sampler(std::make_shared<AlwaysOffSampler>());
  ASSERT_EQ("ParentBased{AlwaysOffSampler}", sampler.GetDescription());
  ParentBasedSampler sampler2(std::make_shared<AlwaysOnSampler>());
  ASSERT_EQ("ParentBased{AlwaysOnSampler}", sampler2.GetDescription());
}
