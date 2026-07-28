// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/span_limits.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/trace_state.h"

using opentelemetry::sdk::trace::SpanData;
namespace trace_api = opentelemetry::trace;
namespace common    = opentelemetry::common;

TEST(SpanData, DefaultValues)
{
  trace_api::SpanContext empty_span_context{false, false};
  trace_api::SpanId zero_span_id;
  SpanData data;

  ASSERT_EQ(data.GetTraceId(), empty_span_context.trace_id());
  ASSERT_EQ(data.GetSpanId(), empty_span_context.span_id());
  ASSERT_EQ(data.GetSpanContext(), empty_span_context);
  ASSERT_EQ(data.GetParentSpanId(), zero_span_id);
  ASSERT_EQ(data.GetName(), "");
  ASSERT_EQ(data.GetStatus(), trace_api::StatusCode::kUnset);
  ASSERT_EQ(data.GetDescription(), "");
  ASSERT_EQ(data.GetStartTime().time_since_epoch(), std::chrono::nanoseconds(0));
  ASSERT_EQ(data.GetDuration(), std::chrono::nanoseconds(0));
  ASSERT_EQ(data.GetAttributes().size(), 0);
  ASSERT_EQ(data.GetEvents().size(), 0);
}

TEST(SpanData, Set)
{
  constexpr uint8_t trace_id_buf[]       = {1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8};
  constexpr uint8_t span_id_buf[]        = {1, 2, 3, 4, 5, 6, 7, 8};
  constexpr uint8_t parent_span_id_buf[] = {8, 7, 6, 5, 4, 3, 2, 1};
  trace_api::TraceId trace_id{trace_id_buf};
  trace_api::SpanId span_id{span_id_buf};
  trace_api::SpanId parent_span_id{parent_span_id_buf};
  const auto trace_state = trace_api::TraceState::GetDefault()->Set("key1", "value");
  const trace_api::SpanContext span_context{
      trace_id, span_id, trace_api::TraceFlags{trace_api::TraceFlags::kIsSampled}, true,
      trace_state};
  common::SystemTimestamp now(std::chrono::system_clock::now());

  SpanData data;
  data.SetIdentity(span_context, parent_span_id);
  data.SetName("span name");
  data.SetSpanKind(trace_api::SpanKind::kServer);
  data.SetStatus(trace_api::StatusCode::kOk, "description");
  data.SetStartTime(now);
  data.SetDuration(std::chrono::nanoseconds(1000000));
  data.SetAttribute("attr1", static_cast<int64_t>(314159));
  data.AddEvent("event1", now);

  ASSERT_EQ(data.GetTraceId(), trace_id);
  ASSERT_EQ(data.GetSpanId(), span_id);
  ASSERT_EQ(data.GetSpanContext(), span_context);
  std::string trace_state_key1_value;
  ASSERT_EQ(data.GetSpanContext().trace_state()->Get("key1", trace_state_key1_value), true);
  ASSERT_EQ(trace_state_key1_value, "value");
  ASSERT_EQ(data.GetParentSpanId(), parent_span_id);
  ASSERT_EQ(data.GetName(), "span name");
  ASSERT_EQ(data.GetSpanKind(), trace_api::SpanKind::kServer);
  ASSERT_EQ(data.GetStatus(), trace_api::StatusCode::kOk);
  ASSERT_EQ(data.GetDescription(), "description");
  ASSERT_EQ(data.GetStartTime().time_since_epoch(), now.time_since_epoch());
  ASSERT_EQ(data.GetDuration(), std::chrono::nanoseconds(1000000));
  ASSERT_EQ(opentelemetry::nostd::get<int64_t>(data.GetAttributes().at("attr1")), 314159);
  ASSERT_EQ(data.GetEvents().at(0).GetName(), "event1");
  ASSERT_EQ(data.GetEvents().at(0).GetTimestamp(), now);
}

TEST(SpanData, EventAttributes)
{
  SpanData data;
  const int kNumAttributes                  = 3;
  std::string keys[kNumAttributes]          = {"attr1", "attr2", "attr3"};
  int64_t values[kNumAttributes]            = {3, 5, 20};
  std::map<std::string, int64_t> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  data.AddEvent("Test Event", std::chrono::system_clock::now(),
                common::KeyValueIterableView<std::map<std::string, int64_t>>(attributes));

  for (int i = 0; i < kNumAttributes; i++)
  {
    EXPECT_EQ(
        opentelemetry::nostd::get<int64_t>(data.GetEvents().at(0).GetAttributes().at(keys[i])),
        values[i]);
  }
}

TEST(SpanData, Resources)
{
  SpanData data;
  auto resource          = opentelemetry::sdk::resource::Resource::Create({});
  const auto &input_attr = resource.GetAttributes();
  data.SetResource(resource);
  auto output_attr = data.GetResource().GetAttributes();
  EXPECT_EQ(input_attr, output_attr);
}

TEST(SpanData, Links)
{
  SpanData data;
  const int kNumAttributes                  = 3;
  std::string keys[kNumAttributes]          = {"attr1", "attr2", "attr3"};
  int64_t values[kNumAttributes]            = {4, 12, 33};
  std::map<std::string, int64_t> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  // produce valid SpanContext with pseudo span and trace Id.
  uint8_t span_id_buf[trace_api::SpanId::kSize] = {
      1,
  };
  trace_api::SpanId span_id{span_id_buf};
  uint8_t trace_id_buf[trace_api::TraceId::kSize] = {
      2,
  };
  trace_api::TraceId trace_id{trace_id_buf};
  const auto span_context = trace_api::SpanContext(
      trace_id, span_id, trace_api::TraceFlags{trace_api::TraceFlags::kIsSampled}, true);

  data.AddLink(span_context,
               common::KeyValueIterableView<std::map<std::string, int64_t>>(attributes));

  EXPECT_EQ(data.GetLinks().at(0).GetSpanContext(), span_context);
  for (int i = 0; i < kNumAttributes; i++)
  {
    EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.GetLinks().at(0).GetAttributes().at(keys[i])),
              values[i]);
  }
}

TEST(SpanData, SpanLimits)
{
  SpanData recordable;
  opentelemetry::sdk::trace::SpanLimits limits;
  limits.attribute_count_limit        = 1;
  limits.attribute_value_length_limit = 2;
  limits.link_count_limit             = 1;
  limits.link_attribute_count_limit   = 1;
  limits.event_count_limit            = 1;
  limits.event_attribute_count_limit  = 1;

  constexpr const char *kKey1   = "one";
  constexpr const char *kKey2   = "two";
  constexpr const char *kValue1 = "1234";
  constexpr const char *kValue2 = "5678";
  constexpr const char *kValue3 = "9012";

  std::map<std::string, std::string> attribute_collection{{kKey1, kValue1}, {kKey2, kValue2}};

  recordable.SetSpanLimits(limits);

  // span attribute count and length limits
  recordable.SetAttribute(kKey1, kValue1);
  recordable.SetAttribute(kKey2, kValue2);
  recordable.SetAttribute(kKey1, kValue3);  // overwrite existing attribute

  EXPECT_EQ(recordable.GetAttributes().size(), 1);
  EXPECT_EQ(recordable.GetDroppedAttributesCount(), 1);
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(recordable.GetAttributes().at(kKey1)), "90");

  // event count and per-event attribute count limits
  recordable.AddEvent("event1", std::chrono::system_clock::now(),
                      common::MakeAttributes(attribute_collection));
  recordable.AddEvent("event2", std::chrono::system_clock::now(),
                      common::MakeAttributes(attribute_collection));

  ASSERT_EQ(recordable.GetEvents().size(), 1);
  EXPECT_EQ(recordable.GetDroppedEventsCount(), 1);
  const auto event = recordable.GetEvents().at(0);
  EXPECT_EQ(event.GetDroppedAttributesCount(), 1);
  EXPECT_EQ(event.GetName(), "event1");
  const auto &event_attributes = event.GetAttributes();
  EXPECT_EQ(event_attributes.size(), 1);
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(event_attributes.at(kKey1)), "12");

  // link count and per-link attribute count limits
  recordable.AddLink(trace_api::SpanContext::GetInvalid(),
                     common::MakeAttributes(attribute_collection));
  recordable.AddLink(trace_api::SpanContext::GetInvalid(),
                     common::MakeAttributes(attribute_collection));
  ASSERT_EQ(recordable.GetLinks().size(), 1);
  EXPECT_EQ(recordable.GetDroppedLinksCount(), 1);
  const auto link = recordable.GetLinks().at(0);
  EXPECT_EQ(link.GetDroppedAttributesCount(), 1);
  const auto &link_attributes = link.GetAttributes();
  EXPECT_EQ(link_attributes.size(), 1);
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(link_attributes.at(kKey1)), "12");
}

TEST(SpanData, SpanLimitsNoLimitDefault)
{
  constexpr std::uint32_t kMaxCount = 500;
  SpanData recordable;
  std::map<std::string, std::string> attributes;

  for (std::uint32_t i = 0; i < kMaxCount; ++i)
  {
    attributes["attribute_" + std::to_string(i)] = std::to_string(i);
  }

  for (std::uint32_t i = 0; i < kMaxCount; ++i)
  {
    recordable.SetAttribute("attribute_" + std::to_string(i), i);
    recordable.AddEvent("event_" + std::to_string(i), std::chrono::system_clock::now(),
                        common::MakeAttributes(attributes));
    recordable.AddLink(trace_api::SpanContext::GetInvalid(), common::MakeAttributes(attributes));
  }

  EXPECT_EQ(recordable.GetAttributes().size(), kMaxCount);
  EXPECT_EQ(recordable.GetDroppedAttributesCount(), 0u);
  EXPECT_EQ(recordable.GetEvents().size(), kMaxCount);
  EXPECT_EQ(recordable.GetDroppedEventsCount(), 0u);
  EXPECT_EQ(recordable.GetLinks().size(), kMaxCount);
  EXPECT_EQ(recordable.GetDroppedLinksCount(), 0u);
  EXPECT_EQ(recordable.GetEvents().at(0).GetDroppedAttributesCount(), 0u);
  EXPECT_EQ(recordable.GetLinks().at(0).GetDroppedAttributesCount(), 0u);
}

TEST(SpanData, SpanLimitsDuplicateAttributeKeys)
{
  SpanData recordable;
  opentelemetry::sdk::trace::SpanLimits limits;
  limits.attribute_count_limit        = 2;
  limits.attribute_value_length_limit = 2;
  limits.link_count_limit             = 1;
  limits.link_attribute_count_limit   = 2;
  limits.event_count_limit            = 1;
  limits.event_attribute_count_limit  = 2;

  recordable.SetSpanLimits(limits);

  // Create three attributes. One has a duplicate key.
  std::vector<std::pair<std::string, std::string>> attributes{
      {"attribute_one", "AA"}, {"attribute_two", "BB"}, {"attribute_one", "CC"}};

  for (const auto &keyvalue : attributes)
  {
    recordable.SetAttribute(keyvalue.first, keyvalue.second);
  }

  recordable.AddEvent("event", std::chrono::system_clock::now(),
                      common::MakeAttributes(attributes));

  recordable.AddLink(trace_api::SpanContext::GetInvalid(), common::MakeAttributes(attributes));

  EXPECT_EQ(recordable.GetAttributes().size(), 2u);
  EXPECT_EQ(recordable.GetDroppedAttributesCount(), 0u);
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(recordable.GetAttributes().at("attribute_one")),
            "CC");
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(recordable.GetAttributes().at("attribute_two")),
            "BB");

  EXPECT_EQ(recordable.GetEvents().size(), 1u);
  EXPECT_EQ(recordable.GetDroppedEventsCount(), 0u);
  const auto &event = recordable.GetEvents().at(0);
  EXPECT_EQ(event.GetAttributes().size(), 2u);
  EXPECT_EQ(event.GetDroppedAttributesCount(), 0u);
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(event.GetAttributes().at("attribute_one")),
            "CC");
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(event.GetAttributes().at("attribute_two")),
            "BB");

  EXPECT_EQ(recordable.GetLinks().size(), 1u);
  EXPECT_EQ(recordable.GetDroppedLinksCount(), 0u);
  const auto &link = recordable.GetLinks().at(0);
  EXPECT_EQ(link.GetAttributes().size(), 2u);
  EXPECT_EQ(link.GetDroppedAttributesCount(), 0u);
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(link.GetAttributes().at("attribute_one")), "CC");
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(link.GetAttributes().at("attribute_two")), "BB");
}
