/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::trace::SpanData;

TEST(SpanData, DefaultValues)
{
  opentelemetry::trace::SpanContext empty_span_context{false, false};
  opentelemetry::trace::SpanId zero_span_id;
  SpanData data;

  ASSERT_EQ(data.GetTraceId(), empty_span_context.trace_id());
  ASSERT_EQ(data.GetSpanId(), empty_span_context.span_id());
  ASSERT_EQ(data.GetSpanContext(), empty_span_context);
  ASSERT_EQ(data.GetParentSpanId(), zero_span_id);
  ASSERT_EQ(data.GetName(), "");
  ASSERT_EQ(data.GetStatus(), opentelemetry::trace::StatusCode::kUnset);
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
  opentelemetry::trace::TraceId trace_id{trace_id_buf};
  opentelemetry::trace::SpanId span_id{span_id_buf};
  opentelemetry::trace::SpanId parent_span_id{parent_span_id_buf};
  const auto trace_state = opentelemetry::trace::TraceState::GetDefault()->Set("key1", "value");
  const opentelemetry::trace::SpanContext span_context{
      trace_id, span_id,
      opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled}, true,
      trace_state};
  opentelemetry::common::SystemTimestamp now(std::chrono::system_clock::now());

  SpanData data;
  data.SetIdentity(span_context, parent_span_id);
  data.SetName("span name");
  data.SetSpanKind(opentelemetry::trace::SpanKind::kServer);
  data.SetStatus(opentelemetry::trace::StatusCode::kOk, "description");
  data.SetStartTime(now);
  data.SetDuration(std::chrono::nanoseconds(1000000));
  data.SetAttribute("attr1", (int64_t)314159);
  data.AddEvent("event1", now);

  ASSERT_EQ(data.GetTraceId(), trace_id);
  ASSERT_EQ(data.GetSpanId(), span_id);
  ASSERT_EQ(data.GetSpanContext(), span_context);
  std::string trace_state_key1_value;
  ASSERT_EQ(data.GetSpanContext().trace_state()->Get("key1", trace_state_key1_value), true);
  ASSERT_EQ(trace_state_key1_value, "value");
  ASSERT_EQ(data.GetParentSpanId(), parent_span_id);
  ASSERT_EQ(data.GetName(), "span name");
  ASSERT_EQ(data.GetSpanKind(), opentelemetry::trace::SpanKind::kServer);
  ASSERT_EQ(data.GetStatus(), opentelemetry::trace::StatusCode::kOk);
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

  data.AddEvent(
      "Test Event", std::chrono::system_clock::now(),
      opentelemetry::common::KeyValueIterableView<std::map<std::string, int64_t>>(attributes));

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
  auto resource   = opentelemetry::sdk::resource::Resource::Create({});
  auto input_attr = resource.GetAttributes();
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
  uint8_t span_id_buf[opentelemetry::trace::SpanId::kSize] = {
      1,
  };
  opentelemetry::trace::SpanId span_id{span_id_buf};
  uint8_t trace_id_buf[opentelemetry::trace::TraceId::kSize] = {
      2,
  };
  opentelemetry::trace::TraceId trace_id{trace_id_buf};
  const auto span_context = opentelemetry::trace::SpanContext(
      trace_id, span_id,
      opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled}, true);

  data.AddLink(
      span_context,
      opentelemetry::common::KeyValueIterableView<std::map<std::string, int64_t>>(attributes));

  EXPECT_EQ(data.GetLinks().at(0).GetSpanContext(), span_context);
  for (int i = 0; i < kNumAttributes; i++)
  {
    EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.GetLinks().at(0).GetAttributes().at(keys[i])),
              values[i]);
  }
}
