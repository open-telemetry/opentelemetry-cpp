// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/jaeger/recordable.h"
#include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
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
using namespace opentelemetry::sdk::instrumentationlibrary;

TEST(JaegerSpanRecordable, SetIdentity)
{
  opentelemetry::exporter::jaeger::JaegerRecordable rec;

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
  EXPECT_EQ(span->traceIdLow, opentelemetry::exporter::jaeger::otel_bswap_64(trace_id_val[1]));
  EXPECT_EQ(span->traceIdHigh, opentelemetry::exporter::jaeger::otel_bswap_64(trace_id_val[0]));
  EXPECT_EQ(span->spanId, opentelemetry::exporter::jaeger::otel_bswap_64(span_id_val));
  EXPECT_EQ(span->parentSpanId, opentelemetry::exporter::jaeger::otel_bswap_64(parent_span_id_val));
#else
  EXPECT_EQ(span->traceIdLow, trace_id_val[0]);
  EXPECT_EQ(span->traceIdHigh, trace_id_val[1]);
  EXPECT_EQ(span->spanId, span_id_val);
  EXPECT_EQ(span->parentSpanId, parent_span_id_val);
#endif
}

TEST(JaegerSpanRecordable, SetName)
{
  opentelemetry::exporter::jaeger::JaegerRecordable rec;

  nostd::string_view name = "Test Span";
  rec.SetName(name);

  std::unique_ptr<thrift::Span> span{rec.Span()};

  EXPECT_EQ(span->operationName, name);
}

TEST(JaegerSpanRecordable, SetStartTime)
{
  opentelemetry::exporter::jaeger::JaegerRecordable rec;

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
  opentelemetry::exporter::jaeger::JaegerRecordable rec;

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
  opentelemetry::exporter::jaeger::JaegerRecordable rec;

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

TEST(JaegerSpanRecordable, AddEvent)
{
  opentelemetry::exporter::jaeger::JaegerRecordable rec;

  nostd::string_view name = "Test Event";

  std::chrono::system_clock::time_point event_time = std::chrono::system_clock::now();
  opentelemetry::common::SystemTimestamp event_timestamp(event_time);
  uint64_t epoch_us =
      std::chrono::duration_cast<std::chrono::microseconds>(event_time.time_since_epoch()).count();

  const int kNumAttributes                  = 3;
  std::string keys[kNumAttributes]          = {"attr1", "attr2", "attr3"};
  int64_t values[kNumAttributes]            = {4, 7, 23};
  std::map<std::string, int64_t> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  rec.AddEvent(
      "Test Event", event_timestamp,
      opentelemetry::common::KeyValueIterableView<std::map<std::string, int64_t>>(attributes));
  thrift::Log log = rec.Logs().at(0);
  EXPECT_EQ(log.timestamp, epoch_us);
  auto tags    = log.fields;
  size_t index = 0;
  EXPECT_EQ(tags[index].key, "event");
  EXPECT_EQ(tags[index++].vStr, "Test Event");
  while (index <= kNumAttributes)
  {
    EXPECT_EQ(tags[index].key, keys[index - 1]);
    EXPECT_EQ(tags[index].vLong, values[index - 1]);
    index++;
  }
}

TEST(JaegerSpanRecordable, SetInstrumentationLibrary)
{
  opentelemetry::exporter::jaeger::JaegerRecordable rec;

  std::string library_name     = "opentelemetry-cpp";
  std::string library_version  = "0.1.0";
  auto instrumentation_library = InstrumentationLibrary::Create(library_name, library_version);

  rec.SetInstrumentationLibrary(*instrumentation_library);

  auto tags = rec.Tags();
  EXPECT_EQ(tags.size(), 2);

  EXPECT_EQ(tags[0].key, "otel.library.name");
  EXPECT_EQ(tags[0].vType, thrift::TagType::STRING);
  EXPECT_EQ(tags[0].vStr, library_name);

  EXPECT_EQ(tags[1].key, "otel.library.version");
  EXPECT_EQ(tags[1].vType, thrift::TagType::STRING);
  EXPECT_EQ(tags[1].vStr, library_version);
}

TEST(JaegerSpanRecordable, SetResource)
{
  opentelemetry::exporter::jaeger::JaegerRecordable rec;

  const std::string service_name_key = "service.name";
  std::string service_name_value     = "test-jaeger-service-name";
  auto resource                      = opentelemetry::sdk::resource::Resource::Create(
      {{service_name_key, service_name_value}, {"key1", "value1"}, {"key2", "value2"}});
  rec.SetResource(resource);

  auto service_name  = rec.ServiceName();
  auto resource_tags = rec.ResourceTags();

  EXPECT_GE(resource_tags.size(), 2);
  EXPECT_EQ(service_name, service_name_value);

  bool found_key1 = false;
  bool found_key2 = false;
  for (const auto &tag : resource_tags)
  {
    if (tag.key == "key1")
    {
      found_key1 = true;
      EXPECT_EQ(tag.vType, thrift::TagType::STRING);
      EXPECT_EQ(tag.vStr, "value1");
    }
    else if (tag.key == "key2")
    {
      found_key2 = true;
      EXPECT_EQ(tag.vType, thrift::TagType::STRING);
      EXPECT_EQ(tag.vStr, "value2");
    }
  }
}
