#include "opentelemetry/sdk/trace/span_data.h"
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
}

TEST(SpanData, Set)
{
  opentelemetry::trace::TraceId trace_id;
  opentelemetry::trace::SpanId span_id;
  opentelemetry::trace::SpanId parent_span_id;
  opentelemetry::core::SystemTimestamp now(std::chrono::system_clock::now());

  SpanData data;
  data.SetTraceId(trace_id);
  data.SetSpanId(span_id);
  data.SetParentSpanId(parent_span_id);
  data.SetName("span name");
  data.SetStatus(opentelemetry::trace::CanonicalCode::UNKNOWN, "description");
  data.SetStartTime(now);
  data.SetDuration(std::chrono::nanoseconds(1000000));
  data.AddEvent("event1", now);

  ASSERT_EQ(data.GetTraceId(), trace_id);
  ASSERT_EQ(data.GetSpanId(), span_id);
  ASSERT_EQ(data.GetParentSpanId(), parent_span_id);
  ASSERT_EQ(data.GetName(), "span name");
  ASSERT_EQ(data.GetStatus(), opentelemetry::trace::CanonicalCode::UNKNOWN);
  ASSERT_EQ(data.GetDescription(), "description");
  ASSERT_EQ(data.GetStartTime().time_since_epoch(), now.time_since_epoch());
  ASSERT_EQ(data.GetDuration(), std::chrono::nanoseconds(1000000));
}
