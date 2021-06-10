// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/jaeger/recordable.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include <gtest/gtest.h>

namespace trace    = opentelemetry::trace;
namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;

using namespace jaegertracing;
using namespace opentelemetry::exporter::jaeger;

TEST(JaegerSpanRecordable, SetIdentity)
{
  opentelemetry::exporter::jaeger::Recordable rec;

  int64_t trace_id_val[2]    = {0x0000000000000000, 0x1000000000000000};
  int64_t span_id_val        = 0x2000000000000000;
  int64_t parent_span_id_val = 0x3000000000000000;

  const trace::TraceId trace_id{
      nostd::span<uint8_t, 16>(reinterpret_cast<uint8_t *>(trace_id_val), 16)};

  const trace::SpanId span_id(
      nostd::span<uint8_t, 8>(reinterpret_cast<uint8_t *>(&span_id_val), 8));

  const trace::SpanId parent_span_id(
      nostd::span<uint8_t, 8>(reinterpret_cast<uint8_t *>(&parent_span_id_val), 8));

  const opentelemetry::trace::SpanContext span_context{
      trace_id, span_id,
      opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled}, true};
  rec.SetIdentity(span_context, parent_span_id);

  std::unique_ptr<thrift::Span> span{rec.Span()};

#if JAEGER_IS_LITTLE_ENDIAN == 1
  EXPECT_EQ(span->traceIdLow, opentelemetry::exporter::jaeger::bswap_64(trace_id_val[1]));
  EXPECT_EQ(span->traceIdHigh, opentelemetry::exporter::jaeger::bswap_64(trace_id_val[0]));
  EXPECT_EQ(span->spanId, opentelemetry::exporter::jaeger::bswap_64(span_id_val));
  EXPECT_EQ(span->parentSpanId, opentelemetry::exporter::jaeger::bswap_64(parent_span_id_val));
#else
  EXPECT_EQ(span->traceIdLow, trace_id_val[0]);
  EXPECT_EQ(span->traceIdHigh, trace_id_val[1]);
  EXPECT_EQ(span->spanId, span_id_val);
  EXPECT_EQ(span->parentSpanId, parent_span_id_val);
#endif
}

TEST(JaegerSpanRecordable, SetName)
{
  opentelemetry::exporter::jaeger::Recordable rec;

  nostd::string_view name = "Test Span";
  rec.SetName(name);

  std::unique_ptr<thrift::Span> span{rec.Span()};

  EXPECT_EQ(span->operationName, name);
}

TEST(JaegerSpanRecordable, SetStartTime)
{
  opentelemetry::exporter::jaeger::Recordable rec;

  std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
  opentelemetry::common::SystemTimestamp start_timestamp(start_time);
  uint64_t unix_start =
      std::chrono::duration_cast<std::chrono::microseconds>(start_time.time_since_epoch()).count();
  rec.SetStartTime(start_timestamp);

  std::unique_ptr<thrift::Span> span{rec.Span()};

  EXPECT_EQ(span->startTime, unix_start);
}

TEST(JaegerSpanRecordable, SetDuration)
{
  opentelemetry::exporter::jaeger::Recordable rec;

  opentelemetry::common::SystemTimestamp start_timestamp;

  std::chrono::microseconds duration(10);
  uint64_t unix_end = duration.count();

  rec.SetStartTime(start_timestamp);
  rec.SetDuration(duration);

  std::unique_ptr<thrift::Span> span{rec.Span()};

  EXPECT_EQ(span->startTime, 0);
  EXPECT_EQ(span->duration, unix_end);
}

TEST(JaegerSpanRecordable, SetStatus)
{
  opentelemetry::exporter::jaeger::Recordable rec;

  const char *error_description = "Error test";
  rec.SetStatus(trace::StatusCode::kError, error_description);

  auto tags = rec.Tags();
  EXPECT_EQ(tags.size(), 3);

  EXPECT_EQ(tags[0].key, "otel.status_code");
  EXPECT_EQ(tags[0].vType, thrift::TagType::STRING);
  EXPECT_EQ(tags[0].vStr, "ERROR");

  EXPECT_EQ(tags[1].key, "error");
  EXPECT_EQ(tags[1].vType, thrift::TagType::BOOL);
  EXPECT_EQ(tags[1].vBool, true);

  EXPECT_EQ(tags[2].key, "otel.status_description");
  EXPECT_EQ(tags[2].vType, thrift::TagType::STRING);
  EXPECT_EQ(tags[2].vStr, error_description);
}
