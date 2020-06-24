#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/nostd/variant.h"
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
  ASSERT_EQ(data.GetStatus(), opentelemetry::trace::CanonicalCode::OK);
  ASSERT_EQ(data.GetDescription(), "");
  ASSERT_EQ(data.GetStartTime().time_since_epoch(), std::chrono::nanoseconds(0));
  ASSERT_EQ(data.GetDuration(), std::chrono::nanoseconds(0));
  ASSERT_EQ(data.GetAttributes().size(), 0);
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
  data.SetStatus(opentelemetry::trace::CanonicalCode::UNKNOWN, "description");
  data.SetStartTime(now);
  data.SetDuration(std::chrono::nanoseconds(1000000));
  data.SetAttribute("attr1", 314159);
  data.AddEvent("event1", now);

  ASSERT_EQ(data.GetTraceId(), trace_id);
  ASSERT_EQ(data.GetSpanId(), span_id);
  ASSERT_EQ(data.GetParentSpanId(), parent_span_id);
  ASSERT_EQ(data.GetName(), "span name");
  ASSERT_EQ(data.GetStatus(), opentelemetry::trace::CanonicalCode::UNKNOWN);
  ASSERT_EQ(data.GetDescription(), "description");
  ASSERT_EQ(data.GetStartTime().time_since_epoch(), now.time_since_epoch());
  ASSERT_EQ(data.GetDuration(), std::chrono::nanoseconds(1000000));
  ASSERT_EQ(opentelemetry::nostd::get<int>(data.GetAttributes().at("attr1")), 314159);
}
