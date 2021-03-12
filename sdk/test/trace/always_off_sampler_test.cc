#include <gtest/gtest.h>
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"

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

  using L = std::vector<std::pair<SpanContext, std::map<std::string, std::string>>>;
  L l1    = {{SpanContext(false, false), {}}, {SpanContext(false, false), {}}};

  opentelemetry::common::KeyValueIterableView<M> view{m1};
  opentelemetry::trace::SpanContextKeyValueIterableView<L> links{l1};

  auto sampling_result =
      sampler.ShouldSample(SpanContext::GetInvalid(), trace_id, "", span_kind, view, links);

  ASSERT_EQ(Decision::DROP, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);
  ASSERT_EQ("", sampling_result.trace_state->ToHeader());
}

TEST(AlwaysOffSampler, GetDescription)
{
  AlwaysOffSampler sampler;

  ASSERT_EQ("AlwaysOffSampler", sampler.GetDescription());
}
