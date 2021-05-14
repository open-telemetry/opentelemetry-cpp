// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opentelemetry/sdk/logs/log_record.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::logs::LogRecord;

// Test what a default LogRecord with no fields set holds
TEST(LogRecord, GetDefaultValues)
{
  opentelemetry::trace::TraceId zero_trace_id;
  opentelemetry::trace::SpanId zero_span_id;
  opentelemetry::trace::TraceFlags zero_trace_flags;
  LogRecord record;

  ASSERT_EQ(record.GetSeverity(), opentelemetry::logs::Severity::kInvalid);
  ASSERT_EQ(record.GetName(), "");
  ASSERT_EQ(record.GetBody(), "");
  ASSERT_EQ(record.GetResource().size(), 0);
  ASSERT_EQ(record.GetAttributes().size(), 0);
  ASSERT_EQ(record.GetTraceId(), zero_trace_id);
  ASSERT_EQ(record.GetSpanId(), zero_span_id);
  ASSERT_EQ(record.GetTraceFlags(), zero_trace_flags);
  ASSERT_EQ(record.GetTimestamp().time_since_epoch(), std::chrono::nanoseconds(0));
}

// Test LogRecord fields are properly set and get
TEST(LogRecord, SetAndGet)
{
  opentelemetry::trace::TraceId trace_id;
  opentelemetry::trace::SpanId span_id;
  opentelemetry::trace::TraceFlags trace_flags;
  opentelemetry::common::SystemTimestamp now(std::chrono::system_clock::now());

  // Set all fields of the LogRecord
  LogRecord record;
  record.SetSeverity(opentelemetry::logs::Severity::kInvalid);
  record.SetName("Log name");
  record.SetBody("Message");
  record.SetResource("res1", (bool)true);
  record.SetAttribute("attr1", (int64_t)314159);
  record.SetTraceId(trace_id);
  record.SetSpanId(span_id);
  record.SetTraceFlags(trace_flags);
  record.SetTimestamp(now);

  // Test that all fields match what was set
  ASSERT_EQ(record.GetSeverity(), opentelemetry::logs::Severity::kInvalid);
  ASSERT_EQ(record.GetName(), "Log name");
  ASSERT_EQ(record.GetBody(), "Message");
  ASSERT_EQ(opentelemetry::nostd::get<bool>(record.GetResource().at("res1")), 1);
  ASSERT_EQ(opentelemetry::nostd::get<int64_t>(record.GetAttributes().at("attr1")), 314159);
  ASSERT_EQ(record.GetTraceId(), trace_id);
  ASSERT_EQ(record.GetSpanId(), span_id);
  ASSERT_EQ(record.GetTraceFlags(), trace_flags);
  ASSERT_EQ(record.GetTimestamp().time_since_epoch(), now.time_since_epoch());
}
