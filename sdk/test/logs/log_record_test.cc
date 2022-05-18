// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/log_record.h"
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/trace/span_id.h"
#  include "opentelemetry/trace/trace_id.h"

#  include <gtest/gtest.h>

using opentelemetry::sdk::logs::LogRecord;
namespace trace_api = opentelemetry::trace;
namespace logs_api  = opentelemetry::logs;
namespace nostd     = opentelemetry::nostd;

// Test what a default LogRecord with no fields set holds
TEST(LogRecord, GetDefaultValues)
{
  trace_api::TraceId zero_trace_id;
  trace_api::SpanId zero_span_id;
  trace_api::TraceFlags zero_trace_flags;
  LogRecord record;

  ASSERT_EQ(record.GetSeverity(), logs_api::Severity::kInvalid);
  ASSERT_EQ(record.GetBody(), "");
  ASSERT_NE(record.GetResource().GetAttributes().size(), 0);
  ASSERT_EQ(record.GetAttributes().size(), 0);
  ASSERT_EQ(record.GetTraceId(), zero_trace_id);
  ASSERT_EQ(record.GetSpanId(), zero_span_id);
  ASSERT_EQ(record.GetTraceFlags(), zero_trace_flags);
  ASSERT_EQ(record.GetTimestamp().time_since_epoch(), std::chrono::nanoseconds(0));
}

// Test LogRecord fields are properly set and get
TEST(LogRecord, SetAndGet)
{
  trace_api::TraceId trace_id;
  trace_api::SpanId span_id;
  trace_api::TraceFlags trace_flags;
  opentelemetry::common::SystemTimestamp now(std::chrono::system_clock::now());

  // Set all fields of the LogRecord
  LogRecord record;
  auto resource = opentelemetry::sdk::resource::Resource::Create({{"res1", true}});
  record.SetSeverity(logs_api::Severity::kInvalid);
  record.SetBody("Message");
  record.SetResource(resource);
  record.SetAttribute("attr1", (int64_t)314159);
  record.SetTraceId(trace_id);
  record.SetSpanId(span_id);
  record.SetTraceFlags(trace_flags);
  record.SetTimestamp(now);

  // Test that all fields match what was set
  ASSERT_EQ(record.GetSeverity(), logs_api::Severity::kInvalid);
  ASSERT_EQ(record.GetBody(), "Message");
  ASSERT_TRUE(nostd::get<bool>(record.GetResource().GetAttributes().at("res1")));
  ASSERT_EQ(nostd::get<int64_t>(record.GetAttributes().at("attr1")), 314159);
  ASSERT_EQ(record.GetTraceId(), trace_id);
  ASSERT_EQ(record.GetSpanId(), span_id);
  ASSERT_EQ(record.GetTraceFlags(), trace_flags);
  ASSERT_EQ(record.GetTimestamp().time_since_epoch(), now.time_since_epoch());
}
#endif
