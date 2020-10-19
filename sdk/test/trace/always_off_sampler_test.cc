#include "opentelemetry/sdk/trace/samplers/always_off.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::trace::AlwaysOffSampler;
using opentelemetry::sdk::trace::Decision;
using opentelemetry::trace::SpanContext;

TEST(AlwaysOffSampler, ShouldSample)
{
  AlwaysOffSampler sampler;

  opentelemetry::trace::TraceId trace_id;
  opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kInternal;

  using M = std::map<std::string, int>;
  M m1    = {{}};
  opentelemetry::common::KeyValueIterableView<M> view{m1};

  auto sampling_result =
      sampler.ShouldSample(SpanContext::GetInvalid(), trace_id, "", span_kind, view);

  ASSERT_EQ(Decision::DROP, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(AlwaysOffSampler, GetDescription)
{
  AlwaysOffSampler sampler;

  ASSERT_EQ("AlwaysOffSampler", sampler.GetDescription());
}
