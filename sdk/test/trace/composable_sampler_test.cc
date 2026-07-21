// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/composable_always_off.h"
#include "opentelemetry/sdk/trace/samplers/composable_always_on.h"
#include "opentelemetry/sdk/trace/samplers/composable_parent_threshold.h"
#include "opentelemetry/sdk/trace/samplers/composable_probability.h"
#include "opentelemetry/sdk/trace/samplers/composable_rule_based.h"
#include "opentelemetry/sdk/trace/samplers/composable_sampler.h"
#include "opentelemetry/sdk/trace/samplers/composite_sampler.h"
#include "opentelemetry/sdk/trace/samplers/composite_sampler_factory.h"
#include "opentelemetry/sdk/trace/samplers/predicate.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/trace_state.h"

namespace trace_api = opentelemetry::trace;
namespace common    = opentelemetry::common;

using opentelemetry::sdk::trace::ComposableAlwaysOffSampler;
using opentelemetry::sdk::trace::ComposableAlwaysOnSampler;
using opentelemetry::sdk::trace::ComposableParentThresholdSampler;
using opentelemetry::sdk::trace::ComposableProbabilitySampler;
using opentelemetry::sdk::trace::ComposableRuleBasedSampler;
using opentelemetry::sdk::trace::ComposableSampler;
using opentelemetry::sdk::trace::CompositeSampler;
using opentelemetry::sdk::trace::CompositeSamplerFactory;
using opentelemetry::sdk::trace::Decision;
using opentelemetry::sdk::trace::Predicate;
using opentelemetry::sdk::trace::PredicatedSampler;
using opentelemetry::sdk::trace::SamplingIntent;

namespace
{

using AttrMap = std::map<std::string, int>;
using LinkVec = std::vector<std::pair<trace_api::SpanContext, std::map<std::string, std::string>>>;

// Builds a trace id whose least significant 56 bits all equal the given byte.
// 0xFF yields the maximum randomness (always sampled), 0x00 the minimum.
trace_api::TraceId MakeTraceId(uint8_t low_bytes)
{
  uint8_t buf[trace_api::TraceId::kSize] = {0};
  for (size_t i = trace_api::TraceId::kSize - 7; i < trace_api::TraceId::kSize; ++i)
  {
    buf[i] = low_bytes;
  }
  return trace_api::TraceId(buf);
}

trace_api::SpanContext MakeParent(bool sampled, const std::string &ot_value)
{
  uint8_t trace_buf[trace_api::TraceId::kSize] = {1};
  uint8_t span_buf[trace_api::SpanId::kSize]   = {1};
  auto trace_state                             = trace_api::TraceState::GetDefault();
  if (!ot_value.empty())
  {
    trace_state = trace_state->Set("ot", ot_value);
  }
  return trace_api::SpanContext(trace_api::TraceId(trace_buf), trace_api::SpanId(span_buf),
                                trace_api::TraceFlags(sampled ? 1 : 0), true, trace_state);
}

std::string OtOf(const opentelemetry::sdk::trace::SamplingResult &result)
{
  std::string value;
  if (result.trace_state)
  {
    result.trace_state->Get("ot", value);
  }
  return value;
}

Decision Sample(opentelemetry::sdk::trace::Sampler &sampler,
                const trace_api::SpanContext &parent,
                const trace_api::TraceId &trace_id,
                opentelemetry::sdk::trace::SamplingResult *out = nullptr)
{
  AttrMap attrs;
  LinkVec links;
  common::KeyValueIterableView<AttrMap> attrs_view{attrs};
  trace_api::SpanContextKeyValueIterableView<LinkVec> links_view{links};
  auto result       = sampler.ShouldSample(parent, trace_id, "span", trace_api::SpanKind::kInternal,
                                           attrs_view, links_view);
  Decision decision = result.decision;
  if (out != nullptr)
  {
    *out = std::move(result);
  }
  return decision;
}

// Matches a span by exact name.
class NamePredicate : public Predicate
{
public:
  explicit NamePredicate(std::string name) : name_(std::move(name)) {}

  bool SpanMatches(const trace_api::SpanContext & /*parent_context*/,
                   opentelemetry::nostd::string_view name,
                   trace_api::SpanKind /*span_kind*/,
                   const common::KeyValueIterable & /*attributes*/,
                   const trace_api::SpanContextKeyValueIterable & /*links*/) const noexcept override
  {
    return name == name_;
  }

  opentelemetry::nostd::string_view GetDescription() const noexcept override
  {
    return "NamePredicate";
  }

private:
  std::string name_;
};

class MatchAll : public Predicate
{
public:
  bool SpanMatches(const trace_api::SpanContext & /*parent_context*/,
                   opentelemetry::nostd::string_view /*name*/,
                   trace_api::SpanKind /*span_kind*/,
                   const common::KeyValueIterable & /*attributes*/,
                   const trace_api::SpanContextKeyValueIterable & /*links*/) const noexcept override
  {
    return true;
  }

  opentelemetry::nostd::string_view GetDescription() const noexcept override { return "MatchAll"; }
};

// Keeps every span and exercises the optional attribute and trace state
// providers of SamplingIntent.
class AnnotatingSampler : public ComposableSampler
{
public:
  SamplingIntent GetSamplingIntent(
      const trace_api::SpanContext & /*parent_context*/,
      trace_api::TraceId /*trace_id*/,
      opentelemetry::nostd::string_view /*name*/,
      trace_api::SpanKind /*span_kind*/,
      const common::KeyValueIterable & /*attributes*/,
      const trace_api::SpanContextKeyValueIterable & /*links*/) noexcept override
  {
    SamplingIntent intent;
    intent.has_threshold       = true;
    intent.threshold_value     = 0;
    intent.threshold_reliable  = true;
    intent.attributes_provider = []() {
      opentelemetry::sdk::trace::SamplingIntentAttributes attrs;
      attrs["annotated"] = true;
      return attrs;
    };
    intent.trace_state_provider =
        [](const opentelemetry::nostd::shared_ptr<trace_api::TraceState> &trace_state) {
          return trace_state->Set("p", "1");
        };
    return intent;
  }

  opentelemetry::nostd::string_view GetDescription() const noexcept override
  {
    return "AnnotatingSampler";
  }
};

}  // namespace

TEST(ComposableSampler, AlwaysOnSamplesAndWritesThreshold)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<ComposableAlwaysOnSampler>());
  opentelemetry::sdk::trace::SamplingResult result;
  Decision decision =
      Sample(*sampler, trace_api::SpanContext::GetInvalid(), MakeTraceId(0x00), &result);
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE, decision);
  // threshold 0 (sample everything) is encoded as th:0
  EXPECT_EQ("th:0", OtOf(result));
}

TEST(ComposableSampler, AlwaysOffDrops)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<ComposableAlwaysOffSampler>());
  Decision decision = Sample(*sampler, trace_api::SpanContext::GetInvalid(), MakeTraceId(0xFF));
  EXPECT_EQ(Decision::DROP, decision);
}

TEST(ComposableSampler, TraceIdRatioBoundaries)
{
  auto always =
      CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(1.0));
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE,
            Sample(*always, trace_api::SpanContext::GetInvalid(), MakeTraceId(0x00)));

  auto never = CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(0.0));
  EXPECT_EQ(Decision::DROP,
            Sample(*never, trace_api::SpanContext::GetInvalid(), MakeTraceId(0xFF)));
}

TEST(ComposableSampler, TraceIdRatioUsesTraceRandomness)
{
  auto sampler =
      CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(0.5));

  // Threshold for p=0.5 is 2^55, encoded as th:8. Maximum randomness keeps.
  opentelemetry::sdk::trace::SamplingResult kept;
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE,
            Sample(*sampler, trace_api::SpanContext::GetInvalid(), MakeTraceId(0xFF), &kept));
  EXPECT_EQ("th:8", OtOf(kept));

  // Minimum randomness (R = 0 < T) drops.
  EXPECT_EQ(Decision::DROP,
            Sample(*sampler, trace_api::SpanContext::GetInvalid(), MakeTraceId(0x00)));
}

TEST(ComposableSampler, ExplicitRandomnessOverridesTraceId)
{
  auto sampler =
      CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(0.5));

  // rv present and at maximum: keep regardless of the (zero) trace id bits, and
  // the explicit randomness must be preserved in the outgoing tracestate.
  auto parent = MakeParent(false, "rv:ffffffffffffff");
  opentelemetry::sdk::trace::SamplingResult kept;
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE, Sample(*sampler, parent, MakeTraceId(0x00), &kept));
  EXPECT_EQ("th:8;rv:ffffffffffffff", OtOf(kept));

  // rv present and at minimum: drop, threshold cleared, rv preserved.
  auto parent_low = MakeParent(false, "rv:00000000000000");
  opentelemetry::sdk::trace::SamplingResult dropped;
  EXPECT_EQ(Decision::DROP, Sample(*sampler, parent_low, MakeTraceId(0xFF), &dropped));
  EXPECT_EQ("rv:00000000000000", OtOf(dropped));
}

TEST(ComposableSampler, ParentThresholdDelegatesForRoot)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<ComposableParentThresholdSampler>(
      std::make_shared<ComposableAlwaysOnSampler>()));
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE,
            Sample(*sampler, trace_api::SpanContext::GetInvalid(), MakeTraceId(0x00)));
}

TEST(ComposableSampler, ParentThresholdPropagatesParentThreshold)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<ComposableParentThresholdSampler>(
      std::make_shared<ComposableAlwaysOffSampler>()));

  // Parent carries th:0 (keep everything). Even with the AlwaysOff root, a
  // non-root span follows the parent threshold.
  auto parent = MakeParent(true, "th:0");
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE, Sample(*sampler, parent, MakeTraceId(0x00)));
}

TEST(ComposableSampler, ParentThresholdFallsBackToSampledFlag)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<ComposableParentThresholdSampler>(
      std::make_shared<ComposableAlwaysOffSampler>()));

  // Sampled parent, no threshold: keep.
  auto sampled_parent = MakeParent(true, "");
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE, Sample(*sampler, sampled_parent, MakeTraceId(0x00)));

  // Non-sampled parent, no threshold: drop.
  auto unsampled_parent = MakeParent(false, "");
  EXPECT_EQ(Decision::DROP, Sample(*sampler, unsampled_parent, MakeTraceId(0xFF)));
}

TEST(ComposableSampler, RuleBasedFirstMatchWins)
{
  std::vector<PredicatedSampler> rules;
  rules.push_back(
      {std::make_shared<NamePredicate>("span"), std::make_shared<ComposableAlwaysOffSampler>()});
  rules.push_back({std::make_shared<MatchAll>(), std::make_shared<ComposableAlwaysOnSampler>()});

  auto sampler = CompositeSamplerFactory::Create(
      std::make_shared<ComposableRuleBasedSampler>(std::move(rules)));

  // The span is named "span", so the first (AlwaysOff) rule matches and drops.
  EXPECT_EQ(Decision::DROP,
            Sample(*sampler, trace_api::SpanContext::GetInvalid(), MakeTraceId(0xFF)));
}

TEST(ComposableSampler, RuleBasedNoMatchDrops)
{
  std::vector<PredicatedSampler> rules;
  rules.push_back(
      {std::make_shared<NamePredicate>("other"), std::make_shared<ComposableAlwaysOnSampler>()});

  auto sampler = CompositeSamplerFactory::Create(
      std::make_shared<ComposableRuleBasedSampler>(std::move(rules)));

  EXPECT_EQ(Decision::DROP,
            Sample(*sampler, trace_api::SpanContext::GetInvalid(), MakeTraceId(0xFF)));
}

TEST(ComposableSampler, Descriptions)
{
  ComposableProbabilitySampler ratio(0.5);
  EXPECT_EQ("ComposableProbabilitySampler{0.500000}", std::string(ratio.GetDescription()));

  CompositeSampler composite(std::make_shared<ComposableAlwaysOnSampler>());
  EXPECT_EQ("CompositeSampler{ComposableAlwaysOnSampler}", std::string(composite.GetDescription()));
}

TEST(ComposableSampler, IntentProvidersAreApplied)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<AnnotatingSampler>());
  opentelemetry::sdk::trace::SamplingResult result;
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE,
            Sample(*sampler, trace_api::SpanContext::GetInvalid(), MakeTraceId(0x00), &result));
  ASSERT_NE(nullptr, result.attributes);
  EXPECT_EQ(1u, result.attributes->count("annotated"));
  std::string p;
  ASSERT_TRUE(result.trace_state->Get("p", p));
  EXPECT_EQ("1", p);
  EXPECT_EQ("th:0", OtOf(result));
}

TEST(ComposableSampler, PreservesOtherSubkeysAndIgnoresInvalidValues)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<ComposableAlwaysOnSampler>());
  // th and rv carry invalid hex (both ignored); keep:me is an unknown subkey
  // that must be preserved.
  auto parent = MakeParent(false, "th:xx;rv:zzzzzzzzzzzzzz;keep:me");
  opentelemetry::sdk::trace::SamplingResult result;
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE, Sample(*sampler, parent, MakeTraceId(0x00), &result));
  std::string ot = OtOf(result);
  EXPECT_NE(std::string::npos, ot.find("th:0"));
  EXPECT_NE(std::string::npos, ot.find("keep:me"));
}

TEST(ComposableSampler, DropsTrailingSubkeysOverSizeLimit)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<ComposableAlwaysOnSampler>());
  // Fits in 256 on input, but once th:0 is prepended the trailing subkey no
  // longer fits and is dropped.
  std::string parent_ot = "rv:ffffffffffffff;x:" + std::string(236, 'a');
  auto parent           = MakeParent(false, parent_ot);
  opentelemetry::sdk::trace::SamplingResult result;
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE, Sample(*sampler, parent, MakeTraceId(0x00), &result));
  std::string ot = OtOf(result);
  EXPECT_NE(std::string::npos, ot.find("th:0"));
  EXPECT_EQ(std::string::npos, ot.find("x:aaa"));
}

TEST(ComposableSampler, RatioClampedToValidRange)
{
  auto over = CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(2.0));
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE,
            Sample(*over, trace_api::SpanContext::GetInvalid(), MakeTraceId(0x00)));

  auto under =
      CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(-1.0));
  EXPECT_EQ(Decision::DROP,
            Sample(*under, trace_api::SpanContext::GetInvalid(), MakeTraceId(0xFF)));
}
