// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/zpages/threadsafe_span_data.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"

#include <gtest/gtest.h>
#include <thread>

using opentelemetry::ext::zpages::ThreadsafeSpanData;
using opentelemetry::sdk::common::AttributeConverter;
using opentelemetry::sdk::common::OwnedAttributeValue;

namespace trace_api = opentelemetry::trace;

TEST(ThreadsafeSpanData, DefaultValues)
{
  trace_api::SpanContext empty_span_context{false, false};
  trace_api::SpanId zero_span_id;
  ThreadsafeSpanData data;

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
}

TEST(ThreadsafeSpanData, Set)
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
  opentelemetry::common::SystemTimestamp now(std::chrono::system_clock::now());

  ThreadsafeSpanData data;
  data.SetIdentity(span_context, parent_span_id);
  data.SetName("span name");
  data.SetSpanKind(trace_api::SpanKind::kServer);
  data.SetStatus(trace_api::StatusCode::kOk, "description");
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
  ASSERT_EQ(data.GetStatus(), trace_api::StatusCode::kOk);
  ASSERT_EQ(data.GetDescription(), "description");
  ASSERT_EQ(data.GetStartTime().time_since_epoch(), now.time_since_epoch());
  ASSERT_EQ(data.GetDuration(), std::chrono::nanoseconds(1000000));
  ASSERT_EQ(opentelemetry::nostd::get<int64_t>(data.GetAttributes().at("attr1")), 314159);
}
