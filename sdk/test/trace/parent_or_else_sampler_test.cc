#include <gtest/gtest.h>
#include <memory>
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"

using opentelemetry::sdk::trace::AlwaysOffSampler;
using opentelemetry::sdk::trace::AlwaysOnSampler;
using opentelemetry::sdk::trace::Decision;
using opentelemetry::sdk::trace::ParentOrElseSampler;

TEST(ParentOrElseSampler, ShouldSample)
{
  // Initialize a sampler with a AlwaysOffSampler as its delegateSampler
  ParentOrElseSampler sampler_off(std::make_shared<AlwaysOffSampler>());
  // Initialize a sampler with a AlwaysOnSampler as its delegateSampler
  ParentOrElseSampler sampler_on(std::make_shared<AlwaysOnSampler>());

  // Set up parameters
  opentelemetry::trace::TraceId trace_id;
  opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;
  using M = std::map<std::string, int>;
  M m1 = {{}};
  opentelemetry::trace::KeyValueIterableView<M> view{m1};
  opentelemetry::sdk::trace::Sampler::SpanContext parent_context_sampled(true, true);
  opentelemetry::sdk::trace::Sampler::SpanContext parent_context_nonsampled(true, false);

  // Case 1: Parent doesn't exist. Return result of delegateSampler()
  auto sampling_result = sampler_off.ShouldSample(nullptr, trace_id, "", span_kind, view);

  ASSERT_EQ(Decision::NOT_RECORD, sampling_result.decision);

  // Case 2: Parent exists and SampledFlag is true
  auto sampling_result2 =
      sampler_off.ShouldSample(&parent_context_sampled, trace_id, "", span_kind, view);
  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result2.decision);

  // Case 3: Parent exists and SampledFlag is false
  auto sampling_result3 =
      sampler_on.ShouldSample(&parent_context_nonsampled, trace_id, "", span_kind, view);
  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result2.decision);
}

TEST(ParentOrElseSampler, GetDescription)
{
  ParentOrElseSampler sampler(std::make_shared<AlwaysOffSampler>());

  ASSERT_EQ("ParentOrElse{AlwaysOffSampler}", sampler.GetDescription());
}
