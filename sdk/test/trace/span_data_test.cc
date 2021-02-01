#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::trace::SpanData;

TEST(SpanData, DefaultValues)
{
  opentelemetry::trace::TraceId zero_trace_id;
  opentelemetry::trace::SpanId zero_span_id;
  SpanData data;

  ASSERT_EQ(data.GetTraceId(), zero_trace_id);
  ASSERT_EQ(data.GetSpanId(), zero_span_id);
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
  opentelemetry::trace::TraceId trace_id;
  opentelemetry::trace::SpanId span_id;
  opentelemetry::trace::SpanId parent_span_id;
  opentelemetry::core::SystemTimestamp now(std::chrono::system_clock::now());

  SpanData data;
  data.SetIds(trace_id, span_id, parent_span_id);
  data.SetName("span name");
  data.SetSpanKind(opentelemetry::trace::SpanKind::kServer);
  data.SetStatus(opentelemetry::trace::StatusCode::kOk, "description");
  data.SetStartTime(now);
  data.SetDuration(std::chrono::nanoseconds(1000000));
  data.SetAttribute("attr1", (int64_t)314159);
  data.opentelemetry::sdk::trace::Recordable::AddEvent("event1", now);

  ASSERT_EQ(data.GetTraceId(), trace_id);
  ASSERT_EQ(data.GetSpanId(), span_id);
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

TEST(SpanData, Links)
{
  SpanData data;
  const int kNumAttributes                  = 3;
  std::string keys[kNumAttributes]          = {"attr1", "attr2", "attr3"};
  int64_t values[kNumAttributes]            = {4, 12, 33};
  std::map<std::string, int64_t> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  data.AddLink(
      opentelemetry::trace::SpanContext(false, false),
      opentelemetry::common::KeyValueIterableView<std::map<std::string, int64_t>>(attributes));

  for (int i = 0; i < kNumAttributes; i++)
  {
    EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.GetLinks().at(0).GetAttributes().at(keys[i])),
              values[i]);
  }
}
