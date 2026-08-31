// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "opentelemetry/common/attribute_value.h"
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
#include "opentelemetry/sdk/trace/samplers/rule_based_predicate.h"
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
using opentelemetry::sdk::trace::RuleBasedPredicate;
using opentelemetry::sdk::trace::RuleBasedPredicateOptions;
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

trace_api::SpanContext MakeParent(bool remote)
{
  constexpr uint8_t trace_bytes[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  constexpr uint8_t span_bytes[8]   = {1, 2, 3, 4, 5, 6, 7, 8};
  return trace_api::SpanContext(trace_api::TraceId(trace_bytes), trace_api::SpanId(span_bytes),
                                trace_api::TraceFlags(trace_api::TraceFlags::kIsSampled), remote);
}

bool Matches(const RuleBasedPredicate &pred,
             const std::map<std::string, common::AttributeValue> &attrs,
             const trace_api::SpanContext &parent = trace_api::SpanContext::GetInvalid(),
             trace_api::SpanKind kind             = trace_api::SpanKind::kInternal)
{
  common::KeyValueIterableView<std::map<std::string, common::AttributeValue>> attributes(attrs);
  LinkVec link_vec;
  trace_api::SpanContextKeyValueIterableView<LinkVec> links(link_vec);
  return pred.SpanMatches(parent, "span", kind, attributes, links);
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

TEST(ComposableSampler, ThresholdOmittedOverSizeLimit)
{
  auto sampler = CompositeSamplerFactory::Create(std::make_shared<ComposableAlwaysOnSampler>());
  // Fits in 256 on input, but once th:0 is prepended the value no longer
  // fits; the inherited subkeys must be preserved, so th is omitted.
  std::string parent_ot = "rv:ffffffffffffff;x:" + std::string(236, 'a');
  auto parent           = MakeParent(false, parent_ot);
  opentelemetry::sdk::trace::SamplingResult result;
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE, Sample(*sampler, parent, MakeTraceId(0x00), &result));
  std::string ot = OtOf(result);
  EXPECT_EQ(std::string::npos, ot.find("th:"));
  EXPECT_EQ(parent_ot, ot);
}

TEST(ComposableSampler, InvalidRatioUsesDefault)
{
  for (double ratio : {2.0, -1.0, 1e-20})
  {
    ComposableProbabilitySampler sampler(ratio);
    EXPECT_EQ(std::string{sampler.GetDescription()}, "ComposableProbabilitySampler{1.000000}");
  }
  auto invalid =
      CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(-1.0));
  EXPECT_EQ(Decision::RECORD_AND_SAMPLE,
            Sample(*invalid, trace_api::SpanContext::GetInvalid(), MakeTraceId(0xFF)));
}

TEST(ComposableSampler, ZeroRatioIsValidAndDrops)
{
  ComposableProbabilitySampler sampler(0.0);
  EXPECT_EQ(std::string{sampler.GetDescription()}, "ComposableProbabilitySampler{0.000000}");
  auto zero = CompositeSamplerFactory::Create(std::make_shared<ComposableProbabilitySampler>(0.0));
  EXPECT_EQ(Decision::DROP, Sample(*zero, trace_api::SpanContext::GetInvalid(), MakeTraceId(0xFF)));
}

TEST(ComposableSampler, ProbabilityNanRatioUsesDefault)
{
  ComposableProbabilitySampler sampler(std::numeric_limits<double>::quiet_NaN());
  EXPECT_EQ(std::string{sampler.GetDescription()}, "ComposableProbabilitySampler{1.000000}");
}

TEST(RuleBasedPredicate, EmptyOptionsMatchEverySpan)
{
  RuleBasedPredicate pred(RuleBasedPredicateOptions{});
  EXPECT_TRUE(Matches(pred, {}));
  EXPECT_TRUE(Matches(pred, {{"any", "value"}}, MakeParent(true), trace_api::SpanKind::kServer));
  EXPECT_EQ(std::string{pred.GetDescription()}, "RuleBasedPredicate");
}

TEST(RuleBasedPredicate, ParentMatching)
{
  RuleBasedPredicateOptions none;
  none.match_parent_none = true;
  RuleBasedPredicate root_only(std::move(none));
  EXPECT_TRUE(Matches(root_only, {}));
  EXPECT_FALSE(Matches(root_only, {}, MakeParent(true)));

  RuleBasedPredicateOptions remote;
  remote.match_parent_remote = true;
  RuleBasedPredicate remote_only(std::move(remote));
  EXPECT_TRUE(Matches(remote_only, {}, MakeParent(true)));
  EXPECT_FALSE(Matches(remote_only, {}, MakeParent(false)));
  EXPECT_FALSE(Matches(remote_only, {}));
}

TEST(RuleBasedPredicate, SpanKindMatching)
{
  struct KindCase
  {
    trace_api::SpanKind kind;
    bool RuleBasedPredicateOptions::*flag;
  };
  const KindCase cases[] = {
      {trace_api::SpanKind::kInternal, &RuleBasedPredicateOptions::match_span_kind_internal},
      {trace_api::SpanKind::kServer, &RuleBasedPredicateOptions::match_span_kind_server},
      {trace_api::SpanKind::kClient, &RuleBasedPredicateOptions::match_span_kind_client},
      {trace_api::SpanKind::kProducer, &RuleBasedPredicateOptions::match_span_kind_producer},
      {trace_api::SpanKind::kConsumer, &RuleBasedPredicateOptions::match_span_kind_consumer}};

  RuleBasedPredicate any_kind(RuleBasedPredicateOptions{});
  for (const auto &selected : cases)
  {
    EXPECT_TRUE(Matches(any_kind, {}, trace_api::SpanContext::GetInvalid(), selected.kind));

    RuleBasedPredicateOptions options;
    options.*selected.flag = true;
    RuleBasedPredicate pred(std::move(options));
    for (const auto &other : cases)
    {
      EXPECT_EQ(selected.kind == other.kind,
                Matches(pred, {}, trace_api::SpanContext::GetInvalid(), other.kind))
          << "span kind " << static_cast<int>(other.kind);
    }
  }
}

TEST(RuleBasedPredicate, AttributeValues)
{
  RuleBasedPredicateOptions options;
  options.match_values = true;
  options.values_key   = "service";
  options.values       = {"a",          "42",
                          "true",       "-2147483648",
                          "4294967295", "9223372036854775807",
                          "0.5",        "18446744073709551615"};
  RuleBasedPredicate pred(std::move(options));
  EXPECT_TRUE(Matches(pred, {{"service", "a"}}));
  EXPECT_FALSE(Matches(pred, {{"service", "b"}}));
  EXPECT_FALSE(Matches(pred, {{"other", "a"}}));
  EXPECT_FALSE(Matches(pred, {{"service", static_cast<const char *>(nullptr)}}));
  EXPECT_TRUE(Matches(pred, {{"service", opentelemetry::nostd::string_view("a")}}));
  EXPECT_FALSE(Matches(pred, {{"service", opentelemetry::nostd::string_view("b")}}));
  EXPECT_TRUE(Matches(pred, {{"service", true}}));
  EXPECT_FALSE(Matches(pred, {{"service", false}}));
  EXPECT_TRUE(Matches(pred, {{"service", static_cast<int32_t>(42)}}));
  EXPECT_TRUE(Matches(pred, {{"service", std::numeric_limits<int32_t>::min()}}));
  EXPECT_FALSE(Matches(pred, {{"service", static_cast<int32_t>(43)}}));
  EXPECT_TRUE(Matches(pred, {{"service", std::numeric_limits<int64_t>::max()}}));
  EXPECT_FALSE(Matches(pred, {{"service", std::numeric_limits<int64_t>::min()}}));
  EXPECT_TRUE(Matches(pred, {{"service", std::numeric_limits<uint32_t>::max()}}));
  EXPECT_FALSE(Matches(pred, {{"service", static_cast<uint32_t>(0)}}));
  EXPECT_TRUE(Matches(pred, {{"service", std::numeric_limits<uint64_t>::max()}}));
  EXPECT_FALSE(Matches(pred, {{"service", static_cast<uint64_t>(0)}}));
  EXPECT_TRUE(Matches(pred, {{"service", 0.5}}));
  EXPECT_FALSE(Matches(pred, {{"service", 0.25}}));

  // Arrays match when any item matches.
  static const bool bools[]          = {false, true};
  static const bool no_bools[]       = {false};
  static const int32_t int32s[]      = {1, 42};
  static const int32_t no_int32s[]   = {1, 2};
  static const int64_t int64s[]      = {1, std::numeric_limits<int64_t>::max()};
  static const int64_t no_int64s[]   = {1, 2};
  static const uint32_t uint32s[]    = {1, std::numeric_limits<uint32_t>::max()};
  static const uint32_t no_uint32s[] = {1, 2};
  static const uint64_t uint64s[]    = {1, std::numeric_limits<uint64_t>::max()};
  static const uint64_t no_uint64s[] = {1, 2};
  static const double doubles[]      = {0.25, 0.5};
  static const double no_doubles[]   = {0.25, 0.75};
  static const opentelemetry::nostd::string_view items[]    = {"x", "a"};
  static const opentelemetry::nostd::string_view no_items[] = {"x", "y"};
  EXPECT_TRUE(Matches(pred, {{"service", opentelemetry::nostd::span<const bool>(bools)}}));
  EXPECT_FALSE(Matches(pred, {{"service", opentelemetry::nostd::span<const bool>(no_bools)}}));
  EXPECT_TRUE(Matches(pred, {{"service", opentelemetry::nostd::span<const int32_t>(int32s)}}));
  EXPECT_FALSE(Matches(pred, {{"service", opentelemetry::nostd::span<const int32_t>(no_int32s)}}));
  EXPECT_TRUE(Matches(pred, {{"service", opentelemetry::nostd::span<const int64_t>(int64s)}}));
  EXPECT_FALSE(Matches(pred, {{"service", opentelemetry::nostd::span<const int64_t>(no_int64s)}}));
  EXPECT_TRUE(Matches(pred, {{"service", opentelemetry::nostd::span<const uint32_t>(uint32s)}}));
  EXPECT_FALSE(
      Matches(pred, {{"service", opentelemetry::nostd::span<const uint32_t>(no_uint32s)}}));
  EXPECT_TRUE(Matches(pred, {{"service", opentelemetry::nostd::span<const uint64_t>(uint64s)}}));
  EXPECT_FALSE(
      Matches(pred, {{"service", opentelemetry::nostd::span<const uint64_t>(no_uint64s)}}));
  EXPECT_TRUE(Matches(pred, {{"service", opentelemetry::nostd::span<const double>(doubles)}}));
  EXPECT_FALSE(Matches(pred, {{"service", opentelemetry::nostd::span<const double>(no_doubles)}}));
  EXPECT_TRUE(Matches(
      pred,
      {{"service", opentelemetry::nostd::span<const opentelemetry::nostd::string_view>(items)}}));
  EXPECT_FALSE(Matches(
      pred, {{"service",
              opentelemetry::nostd::span<const opentelemetry::nostd::string_view>(no_items)}}));

  // A byte array has no string representation, so it never matches.
  static const uint8_t bytes[] = {42};
  EXPECT_FALSE(Matches(pred, {{"service", opentelemetry::nostd::span<const uint8_t>(bytes)}}));
}

TEST(RuleBasedPredicate, AttributePatterns)
{
  RuleBasedPredicateOptions options;
  options.match_patterns = true;
  options.patterns_key   = "url";
  options.included       = {"/api/*"};
  options.excluded       = {"/api/health"};
  RuleBasedPredicate pred(std::move(options));
  EXPECT_TRUE(Matches(pred, {{"url", "/api/users"}}));
  EXPECT_FALSE(Matches(pred, {{"url", "/api/health"}}));
  EXPECT_FALSE(Matches(pred, {{"url", "/other"}}));
  EXPECT_TRUE(Matches(pred, {{"url", opentelemetry::nostd::string_view("/api/users")}}));
  EXPECT_FALSE(Matches(pred, {{"url", opentelemetry::nostd::string_view("/other")}}));

  static const opentelemetry::nostd::string_view urls[]    = {"/other", "/api/users"};
  static const opentelemetry::nostd::string_view no_urls[] = {"/other", "/api/health"};
  EXPECT_TRUE(Matches(
      pred, {{"url", opentelemetry::nostd::span<const opentelemetry::nostd::string_view>(urls)}}));
  EXPECT_FALSE(Matches(
      pred,
      {{"url", opentelemetry::nostd::span<const opentelemetry::nostd::string_view>(no_urls)}}));

  RuleBasedPredicateOptions excluded_only;
  excluded_only.match_patterns = true;
  excluded_only.patterns_key   = "url";
  excluded_only.excluded       = {"/internal/*"};
  RuleBasedPredicate not_internal(std::move(excluded_only));
  EXPECT_TRUE(Matches(not_internal, {{"url", "/public"}}));
  EXPECT_FALSE(Matches(not_internal, {{"url", "/internal/x"}}));

  // A pattern of "4*" matches any http.response.status_code in 400-499, whatever numeric type
  // carries it.
  RuleBasedPredicateOptions status;
  status.match_patterns = true;
  status.patterns_key   = "http.response.status_code";
  status.included       = {"4*"};
  RuleBasedPredicate client_error(std::move(status));
  auto status_code = [&client_error](common::AttributeValue value) {
    return Matches(client_error, {{"http.response.status_code", value}});
  };
  EXPECT_TRUE(status_code(static_cast<int32_t>(404)));
  EXPECT_TRUE(status_code(static_cast<int64_t>(451)));
  EXPECT_TRUE(status_code(static_cast<uint32_t>(400)));
  EXPECT_TRUE(status_code(static_cast<uint64_t>(499)));
  EXPECT_TRUE(status_code(404.0));
  EXPECT_FALSE(status_code(static_cast<int32_t>(200)));
  EXPECT_FALSE(status_code(200.0));

  // Arrays match when any item matches.
  static const int32_t int32s[]      = {200, 404};
  static const int32_t no_int32s[]   = {200, 500};
  static const int64_t int64s[]      = {200, 451};
  static const int64_t no_int64s[]   = {200, 500};
  static const uint32_t uint32s[]    = {200, 400};
  static const uint32_t no_uint32s[] = {200, 500};
  static const uint64_t uint64s[]    = {200, 499};
  static const uint64_t no_uint64s[] = {200, 500};
  static const double doubles[]      = {200.0, 404.0};
  static const double no_doubles[]   = {200.0, 500.0};
  EXPECT_TRUE(status_code(opentelemetry::nostd::span<const int32_t>(int32s)));
  EXPECT_FALSE(status_code(opentelemetry::nostd::span<const int32_t>(no_int32s)));
  EXPECT_TRUE(status_code(opentelemetry::nostd::span<const int64_t>(int64s)));
  EXPECT_FALSE(status_code(opentelemetry::nostd::span<const int64_t>(no_int64s)));
  EXPECT_TRUE(status_code(opentelemetry::nostd::span<const uint32_t>(uint32s)));
  EXPECT_FALSE(status_code(opentelemetry::nostd::span<const uint32_t>(no_uint32s)));
  EXPECT_TRUE(status_code(opentelemetry::nostd::span<const uint64_t>(uint64s)));
  EXPECT_FALSE(status_code(opentelemetry::nostd::span<const uint64_t>(no_uint64s)));
  EXPECT_TRUE(status_code(opentelemetry::nostd::span<const double>(doubles)));
  EXPECT_FALSE(status_code(opentelemetry::nostd::span<const double>(no_doubles)));

  static const uint8_t bytes[] = {4};
  EXPECT_FALSE(status_code(opentelemetry::nostd::span<const uint8_t>(bytes)));

  RuleBasedPredicateOptions truthy;
  truthy.match_patterns = true;
  truthy.patterns_key   = "enabled";
  truthy.included       = {"tr*"};
  RuleBasedPredicate enabled(std::move(truthy));
  EXPECT_TRUE(Matches(enabled, {{"enabled", true}}));
  EXPECT_FALSE(Matches(enabled, {{"enabled", false}}));

  static const bool flags[]    = {false, true};
  static const bool no_flags[] = {false, false};
  EXPECT_TRUE(Matches(enabled, {{"enabled", opentelemetry::nostd::span<const bool>(flags)}}));
  EXPECT_FALSE(Matches(enabled, {{"enabled", opentelemetry::nostd::span<const bool>(no_flags)}}));

  // Doubles are formatted with %g at 6 significant digits by default, so 404.0 is "404" rather
  // than "404.000000", and negative zero keeps its sign.
  RuleBasedPredicateOptions numbers;
  numbers.match_patterns = true;
  numbers.patterns_key   = "value";
  numbers.included       = {"404",      "0.1",          "1.23457e+06", "1e+20",    "1e-20",    "-0",
                            "0.666667", "1.79769e+308", "NaN",         "Infinity", "-Infinity"};
  RuleBasedPredicate number(std::move(numbers));
  EXPECT_TRUE(Matches(number, {{"value", 404.0}}));
  EXPECT_TRUE(Matches(number, {{"value", 0.1}}));
  EXPECT_TRUE(Matches(number, {{"value", 1234567.89}}));
  EXPECT_TRUE(Matches(number, {{"value", 1e20}}));
  EXPECT_TRUE(Matches(number, {{"value", 1e-20}}));
  EXPECT_TRUE(Matches(number, {{"value", -0.0}}));
  EXPECT_FALSE(Matches(number, {{"value", 0.0}}));
  EXPECT_TRUE(Matches(number, {{"value", 2.0 / 3.0}}));
  EXPECT_TRUE(Matches(number, {{"value", std::numeric_limits<double>::max()}}));
  EXPECT_TRUE(Matches(number, {{"value", std::numeric_limits<double>::quiet_NaN()}}));
  EXPECT_TRUE(Matches(number, {{"value", std::numeric_limits<double>::infinity()}}));
  EXPECT_TRUE(Matches(number, {{"value", -std::numeric_limits<double>::infinity()}}));
  EXPECT_FALSE(Matches(number, {{"value", 0.2}}));

  // double_precision widens the match beyond the 6 significant digit default.
  RuleBasedPredicateOptions precise;
  precise.match_patterns   = true;
  precise.patterns_key     = "value";
  precise.included         = {"1234567.89"};
  precise.double_precision = 15;
  RuleBasedPredicate wide(std::move(precise));
  EXPECT_TRUE(Matches(wide, {{"value", 1234567.89}}));
  EXPECT_FALSE(Matches(wide, {{"value", 1234567.0}}));

  // A double_precision of zero means one significant digit, not zero.
  RuleBasedPredicateOptions coarse;
  coarse.match_patterns   = true;
  coarse.patterns_key     = "value";
  coarse.included         = {"4e+02"};
  coarse.double_precision = 0;
  RuleBasedPredicate single_digit(std::move(coarse));
  EXPECT_TRUE(Matches(single_digit, {{"value", 404.0}}));
  EXPECT_TRUE(Matches(single_digit, {{"value", 444.0}}));

  // Precision past the digits a double carries is capped at 17, not passed on to snprintf.
  RuleBasedPredicateOptions unbounded;
  unbounded.match_patterns   = true;
  unbounded.patterns_key     = "value";
  unbounded.included         = {"0.10000000000000001"};
  unbounded.double_precision = std::numeric_limits<uint32_t>::max();
  RuleBasedPredicate capped(std::move(unbounded));
  EXPECT_TRUE(Matches(capped, {{"value", 0.1}}));
}

TEST(RuleBasedPredicate, ActiveGroupsAreAnded)
{
  RuleBasedPredicateOptions options;
  options.match_values           = true;
  options.values_key             = "service";
  options.values                 = {"a"};
  options.match_span_kind_server = true;
  RuleBasedPredicate pred(std::move(options));
  EXPECT_TRUE(Matches(pred, {{"service", "a"}}, trace_api::SpanContext::GetInvalid(),
                      trace_api::SpanKind::kServer));
  EXPECT_FALSE(Matches(pred, {{"service", "a"}}, trace_api::SpanContext::GetInvalid(),
                       trace_api::SpanKind::kClient));
}
