// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <vector>
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
namespace common   = opentelemetry::common;

using namespace jaegertracing;
using namespace opentelemetry::exporter::jaeger;
using namespace opentelemetry::sdk::instrumentationlibrary;
using std::vector;

using Attributes = std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>;

TEST(JaegerSpanRecordable, SetIdentity)
{
  JaegerRecordable rec;

  int64_t trace_id_val[2]    = {0x0000000000000000, 0x1000000000000000};
  int64_t span_id_val        = 0x2000000000000000;
  int64_t parent_span_id_val = 0x3000000000000000;

  const trace::TraceId trace_id{
      nostd::span<uint8_t, 16>(reinterpret_cast<uint8_t *>(trace_id_val), 16)};

  const trace::SpanId span_id(
      nostd::span<uint8_t, 8>(reinterpret_cast<uint8_t *>(&span_id_val), 8));

  const trace::SpanId parent_span_id(
      nostd::span<uint8_t, 8>(reinterpret_cast<uint8_t *>(&parent_span_id_val), 8));

  const trace::SpanContext span_context{trace_id, span_id,
                                        trace::TraceFlags{trace::TraceFlags::kIsSampled}, true};
  rec.SetIdentity(span_context, parent_span_id);

  std::unique_ptr<thrift::Span> span{rec.Span()};

#if JAEGER_IS_LITTLE_ENDIAN == 1
  EXPECT_EQ(span->traceIdLow, otel_bswap_64(trace_id_val[1]));
  EXPECT_EQ(span->traceIdHigh, otel_bswap_64(trace_id_val[0]));
  EXPECT_EQ(span->spanId, otel_bswap_64(span_id_val));
  EXPECT_EQ(span->parentSpanId, otel_bswap_64(parent_span_id_val));
#else
  EXPECT_EQ(span->traceIdLow, trace_id_val[0]);
  EXPECT_EQ(span->traceIdHigh, trace_id_val[1]);
  EXPECT_EQ(span->spanId, span_id_val);
  EXPECT_EQ(span->parentSpanId, parent_span_id_val);
#endif
}

TEST(JaegerSpanRecordable, SetName)
{
  JaegerRecordable rec;

  nostd::string_view name = "Test Span";
  rec.SetName(name);

  std::unique_ptr<thrift::Span> span{rec.Span()};

  EXPECT_EQ(span->operationName, name);
}

TEST(JaegerSpanRecordable, SetStartTime)
{
  JaegerRecordable rec;

  std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
  common::SystemTimestamp start_timestamp(start_time);
  uint64_t unix_start =
      std::chrono::duration_cast<std::chrono::microseconds>(start_time.time_since_epoch()).count();
  rec.SetStartTime(start_timestamp);

  std::unique_ptr<thrift::Span> span{rec.Span()};

  EXPECT_EQ(span->startTime, unix_start);
}

TEST(JaegerSpanRecordable, SetDuration)
{
  JaegerRecordable rec;

  common::SystemTimestamp start_timestamp;

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
  JaegerRecordable rec;

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
  JaegerRecordable rec;

  std::chrono::system_clock::time_point event_time = std::chrono::system_clock::now();
  common::SystemTimestamp event_timestamp(event_time);
  uint64_t epoch_us =
      std::chrono::duration_cast<std::chrono::microseconds>(event_time.time_since_epoch()).count();

  const int kNumAttributes                  = 3;
  std::string keys[kNumAttributes]          = {"attr1", "attr2", "attr3"};
  int64_t values[kNumAttributes]            = {4, 7, 23};
  std::map<std::string, int64_t> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  rec.AddEvent("Test Event", event_timestamp,
               common::KeyValueIterableView<std::map<std::string, int64_t>>(attributes));
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

template <typename value_type>
void addTag(thrift::TagType::type tag_type,
            const std::string &key,
            value_type value,
            vector<thrift::Tag> &tags)
{
  thrift::Tag tag;

  tag.__set_key(key);
  tag.__set_vType(tag_type);
  if (tag_type == thrift::TagType::LONG)
  {
    tag.__set_vLong(static_cast<int64_t>(value));
  }
  else if (tag_type == thrift::TagType::DOUBLE)
  {
    tag.__set_vDouble(static_cast<double>(value));
  }
  else if (tag_type == thrift::TagType::BOOL)
  {
    tag.__set_vBool(static_cast<bool>(value));
  }

  tags.push_back(tag);
}

void addTag(const std::string &key, std::string value, vector<thrift::Tag> &tags)
{
  thrift::Tag tag;

  tag.__set_key(key);
  tag.__set_vType(thrift::TagType::STRING);
  tag.__set_vStr(value);

  tags.push_back(tag);
}

TEST(JaegerSpanRecordable, SetAttributes)
{
  JaegerRecordable rec;
  std::string string_val{"string_val"};
  vector<common::AttributeValue> values{
      bool{false},
      int32_t{-32},
      int64_t{-64},
      uint32_t{32},
      double{3.14},
      string_val.c_str(),
      nostd::string_view{"string_view"},
  };
  for (const auto &val : values)
  {
    rec.SetAttribute("key1", val);
  }
  rec.SetAttribute("key2", nostd::span<const bool>{{false, true}});
  rec.SetAttribute("key3", nostd::span<const int32_t>{{-320, 320}});
  rec.SetAttribute("key4", nostd::span<const int64_t>{{-640, 640}});
  rec.SetAttribute("key5", nostd::span<const uint32_t>{{320, 322}});
  rec.SetAttribute("key6", nostd::span<const double>{{4.15, 5.15}});
  rec.SetAttribute("key7", nostd::span<const nostd::string_view>{{"string_v1", "string_v2"}});

  auto tags = rec.Tags();
  EXPECT_EQ(tags.size(), values.size() + 12);

  vector<thrift::Tag> expected_tags;
  addTag(thrift::TagType::BOOL, "key1", bool{false}, expected_tags);
  addTag(thrift::TagType::LONG, "key1", int32_t{-32}, expected_tags);
  addTag(thrift::TagType::LONG, "key1", int64_t{-64}, expected_tags);
  addTag(thrift::TagType::LONG, "key1", int32_t{32}, expected_tags);
  addTag(thrift::TagType::DOUBLE, "key1", double{3.14}, expected_tags);
  addTag("key1", string_val, expected_tags);
  addTag("key1", std::string{"string_view"}, expected_tags);

  addTag(thrift::TagType::BOOL, "key2", bool{false}, expected_tags);
  addTag(thrift::TagType::BOOL, "key2", bool{true}, expected_tags);
  addTag(thrift::TagType::LONG, "key3", int32_t{-320}, expected_tags);
  addTag(thrift::TagType::LONG, "key3", int32_t{320}, expected_tags);
  addTag(thrift::TagType::LONG, "key4", int64_t{-640}, expected_tags);
  addTag(thrift::TagType::LONG, "key4", int64_t{640}, expected_tags);
  addTag(thrift::TagType::LONG, "key5", uint32_t{320}, expected_tags);
  addTag(thrift::TagType::LONG, "key5", uint32_t{322}, expected_tags);
  addTag(thrift::TagType::DOUBLE, "key6", double{4.15}, expected_tags);
  addTag(thrift::TagType::DOUBLE, "key6", double{5.15}, expected_tags);
  addTag("key7", std::string{"string_v1"}, expected_tags);
  addTag("key7", std::string{"string_v2"}, expected_tags);

  EXPECT_EQ(tags, expected_tags);
}

TEST(JaegerSpanRecordable, SetInstrumentationLibrary)
{
  JaegerRecordable rec;

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
  JaegerRecordable rec;

  const std::string service_name_key = "service.name";
  std::string service_name_value     = "test-jaeger-service-name";
  auto resource                      = opentelemetry::sdk::resource::Resource::Create(
      {{service_name_key, service_name_value}, {"key1", "value1"}, {"key2", "value2"}});
  rec.SetResource(resource);

  auto service_name  = rec.ServiceName();
  auto resource_tags = rec.ResourceTags();

  EXPECT_GE(resource_tags.size(), 2);
  EXPECT_EQ(service_name, service_name_value);

  for (const auto &tag : resource_tags)
  {
    if (tag.key == "key1")
    {
      EXPECT_EQ(tag.vType, thrift::TagType::STRING);
      EXPECT_EQ(tag.vStr, "value1");
    }
    else if (tag.key == "key2")
    {
      EXPECT_EQ(tag.vType, thrift::TagType::STRING);
      EXPECT_EQ(tag.vStr, "value2");
    }
  }
}

TEST(JaegerSpanRecordable, AddLink)
{
  JaegerRecordable rec;

  int64_t trace_id_val[2] = {0x0000000000000000, 0x1000000000000000};
  int64_t span_id_val     = 0x2000000000000000;

  const trace::TraceId trace_id{
      nostd::span<uint8_t, 16>(reinterpret_cast<uint8_t *>(trace_id_val), 16)};

  const trace::SpanId span_id(
      nostd::span<uint8_t, 8>(reinterpret_cast<uint8_t *>(&span_id_val), 8));

  const trace::SpanContext span_context{trace_id, span_id,
                                        trace::TraceFlags{trace::TraceFlags::kIsSampled}, true};
  rec.AddLink(span_context, common::KeyValueIterableView<Attributes>({{"attr1", "string"}}));

  auto references = rec.References();
  EXPECT_EQ(references.size(), 1);

  auto reference = references.front();

#if JAEGER_IS_LITTLE_ENDIAN == 1
  EXPECT_EQ(reference.traceIdLow, otel_bswap_64(trace_id_val[1]));
  EXPECT_EQ(reference.traceIdHigh, otel_bswap_64(trace_id_val[0]));
  EXPECT_EQ(reference.spanId, otel_bswap_64(span_id_val));
#else
  EXPECT_EQ(reference.traceIdLow, trace_id_val[0]);
  EXPECT_EQ(reference.traceIdHigh, trace_id_val[1]);
  EXPECT_EQ(reference.spanId, span_id_val);
#endif
}
