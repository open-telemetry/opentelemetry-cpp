// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_recordable.h"
#include <gtest/gtest.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace resource  = opentelemetry::sdk::resource;
namespace proto     = opentelemetry::proto;

TEST(OtlpRecordable, SetIdentity)
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

  OtlpRecordable rec;

  rec.SetIdentity(span_context, parent_span_id);

  EXPECT_EQ(rec.span().trace_id(), std::string(reinterpret_cast<const char *>(trace_id.Id().data()),
                                               trace::TraceId::kSize));
  EXPECT_EQ(rec.span().span_id(),
            std::string(reinterpret_cast<const char *>(span_id.Id().data()), trace::SpanId::kSize));
  EXPECT_EQ(rec.span().trace_state(), "key1=value");
  EXPECT_EQ(rec.span().parent_span_id(),
            std::string(reinterpret_cast<const char *>(parent_span_id.Id().data()),
                        trace::SpanId::kSize));

  OtlpRecordable rec_invalid_parent;

  constexpr uint8_t invalid_parent_span_id_buf[] = {0, 0, 0, 0, 0, 0, 0, 0};
  trace_api::SpanId invalid_parent_span_id{invalid_parent_span_id_buf};
  rec_invalid_parent.SetIdentity(span_context, invalid_parent_span_id);

  EXPECT_EQ(rec_invalid_parent.span().parent_span_id(), std::string{});
}

TEST(OtlpRecordable, SetName)
{
  OtlpRecordable rec;
  nostd::string_view name = "Test Span";
  rec.SetName(name);
  EXPECT_EQ(rec.span().name(), name);
}
TEST(OtlpRecordable, SetSpanKind)
{
  OtlpRecordable rec;
  trace_api::SpanKind span_kind = trace_api::SpanKind::kServer;
  rec.SetSpanKind(span_kind);
  EXPECT_EQ(rec.span().kind(), proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_SERVER);
}

TEST(OtlpRecordable, SetInstrumentationLibrary)
{
  OtlpRecordable rec;
  auto inst_lib = trace_sdk::InstrumentationLibrary::Create("test", "v1");
  rec.SetInstrumentationLibrary(*inst_lib);
  auto proto_instr_libr = rec.GetProtoInstrumentationLibrary();
  EXPECT_EQ(proto_instr_libr.name(), inst_lib->GetName());
  EXPECT_EQ(proto_instr_libr.version(), inst_lib->GetVersion());
}

TEST(OtlpRecordable, SetInstrumentationLibraryWithSchemaURL)
{
  OtlpRecordable rec;
  const std::string expected_schema_url{"https://opentelemetry.io/schemas/1.2.0"};
  auto inst_lib = trace_sdk::InstrumentationLibrary::Create("test", "v1", expected_schema_url);
  rec.SetInstrumentationLibrary(*inst_lib);
  EXPECT_EQ(expected_schema_url, rec.GetInstrumentationLibrarySchemaURL());
}

TEST(OtlpRecordable, SetStartTime)
{
  OtlpRecordable rec;
  std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
  common::SystemTimestamp start_timestamp(start_time);

  uint64_t unix_start =
      std::chrono::duration_cast<std::chrono::nanoseconds>(start_time.time_since_epoch()).count();

  rec.SetStartTime(start_timestamp);
  EXPECT_EQ(rec.span().start_time_unix_nano(), unix_start);
}

TEST(OtlpRecordable, SetDuration)
{
  OtlpRecordable rec;
  // Start time is 0
  common::SystemTimestamp start_timestamp;

  std::chrono::nanoseconds duration(10);
  uint64_t unix_end = duration.count();

  rec.SetStartTime(start_timestamp);
  rec.SetDuration(duration);

  EXPECT_EQ(rec.span().end_time_unix_nano(), unix_end);
}

TEST(OtlpRecordable, SetStatus)
{
  OtlpRecordable rec1;
  trace::StatusCode code_error(trace::StatusCode::kError);
  nostd::string_view description = "For test";
  rec1.SetStatus(code_error, description);

  EXPECT_EQ(rec1.span().status().code(), proto::trace::v1::Status_StatusCode(code_error));
  EXPECT_EQ(rec1.span().status().message(), description);

  OtlpRecordable rec2;
  trace::StatusCode code_ok(trace::StatusCode::kOk);
  rec2.SetStatus(code_ok, description);
  EXPECT_EQ(rec2.span().status().code(), proto::trace::v1::Status_StatusCode(code_ok));
  EXPECT_EQ(rec2.span().status().message(), "");
}

TEST(OtlpRecordable, AddEventDefault)
{
  OtlpRecordable rec;
  nostd::string_view name = "Test Event";

  std::chrono::system_clock::time_point event_time = std::chrono::system_clock::now();
  common::SystemTimestamp event_timestamp(event_time);

  rec.sdk::trace::Recordable::AddEvent(name, event_timestamp);

  uint64_t unix_event_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(event_time.time_since_epoch()).count();

  EXPECT_EQ(rec.span().events(0).name(), name);
  EXPECT_EQ(rec.span().events(0).time_unix_nano(), unix_event_time);
  EXPECT_EQ(rec.span().events(0).attributes().size(), 0);
}

TEST(OtlpRecordable, AddEventWithAttributes)
{
  OtlpRecordable rec;
  const int kNumAttributes              = 3;
  std::string keys[kNumAttributes]      = {"attr1", "attr2", "attr3"};
  int values[kNumAttributes]            = {4, 7, 23};
  std::map<std::string, int> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  rec.AddEvent("Test Event", std::chrono::system_clock::now(),
               common::KeyValueIterableView<std::map<std::string, int>>(attributes));

  for (int i = 0; i < kNumAttributes; i++)
  {
    EXPECT_EQ(rec.span().events(0).attributes(i).key(), keys[i]);
    EXPECT_EQ(rec.span().events(0).attributes(i).value().int_value(), values[i]);
  }
}

TEST(OtlpRecordable, AddLink)
{
  OtlpRecordable rec;
  const int kNumAttributes              = 3;
  std::string keys[kNumAttributes]      = {"attr1", "attr2", "attr3"};
  int values[kNumAttributes]            = {5, 12, 40};
  std::map<std::string, int> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  auto trace_id = rec.span().trace_id();
  auto span_id  = rec.span().span_id();

  trace::TraceFlags flags;
  std::string trace_state_header = "k1=v1,k2=v2";
  auto ts                        = trace::TraceState::FromHeader(trace_state_header);

  rec.AddLink(trace::SpanContext(trace::TraceId(), trace::SpanId(), flags, false, ts),
              common::KeyValueIterableView<std::map<std::string, int>>(attributes));

  EXPECT_EQ(rec.span().trace_id(), trace_id);
  EXPECT_EQ(rec.span().span_id(), span_id);
  EXPECT_EQ(rec.span().links(0).trace_state(), trace_state_header);
  for (int i = 0; i < kNumAttributes; i++)
  {
    EXPECT_EQ(rec.span().links(0).attributes(i).key(), keys[i]);
    EXPECT_EQ(rec.span().links(0).attributes(i).value().int_value(), values[i]);
  }
}

TEST(OtlpRecordable, SetResource)
{
  OtlpRecordable rec;
  const std::string service_name_key = "service.name";
  std::string service_name           = "test-otlp";
  auto resource = resource::Resource::Create({{service_name_key, service_name}});
  rec.SetResource(resource);

  auto proto_resource     = rec.ProtoResource();
  bool found_service_name = false;
  for (int i = 0; i < proto_resource.attributes_size(); i++)
  {
    auto attr = proto_resource.attributes(static_cast<int>(i));
    if (attr.key() == service_name_key && attr.value().string_value() == service_name)
    {
      found_service_name = true;
    }
  }
  EXPECT_TRUE(found_service_name);
}

TEST(OtlpRecordable, SetResourceWithSchemaURL)
{
  OtlpRecordable rec;
  const std::string service_name_key    = "service.name";
  const std::string service_name        = "test-otlp";
  const std::string expected_schema_url = "https://opentelemetry.io/schemas/1.2.0";
  auto resource =
      resource::Resource::Create({{service_name_key, service_name}}, expected_schema_url);
  rec.SetResource(resource);

  EXPECT_EQ(expected_schema_url, rec.GetResourceSchemaURL());
}

// Test non-int single types. Int single types are tested using templates (see IntAttributeTest)
TEST(OtlpRecordable, SetSingleAttribute)
{
  OtlpRecordable rec;
  nostd::string_view bool_key = "bool_attr";
  common::AttributeValue bool_val(true);
  rec.SetAttribute(bool_key, bool_val);

  nostd::string_view double_key = "double_attr";
  common::AttributeValue double_val(3.3);
  rec.SetAttribute(double_key, double_val);

  nostd::string_view str_key = "str_attr";
  common::AttributeValue str_val(nostd::string_view("Test"));
  rec.SetAttribute(str_key, str_val);

  EXPECT_EQ(rec.span().attributes(0).key(), bool_key);
  EXPECT_EQ(rec.span().attributes(0).value().bool_value(), nostd::get<bool>(bool_val));

  EXPECT_EQ(rec.span().attributes(1).key(), double_key);
  EXPECT_EQ(rec.span().attributes(1).value().double_value(), nostd::get<double>(double_val));

  EXPECT_EQ(rec.span().attributes(2).key(), str_key);
  EXPECT_EQ(rec.span().attributes(2).value().string_value(),
            nostd::get<nostd::string_view>(str_val).data());
}

// Test non-int array types. Int array types are tested using templates (see IntAttributeTest)
TEST(OtlpRecordable, SetArrayAttribute)
{
  OtlpRecordable rec;
  const int kArraySize = 3;

  bool bool_arr[kArraySize] = {true, false, true};
  nostd::span<const bool> bool_span(bool_arr);
  rec.SetAttribute("bool_arr_attr", bool_span);

  double double_arr[kArraySize] = {22.3, 33.4, 44.5};
  nostd::span<const double> double_span(double_arr);
  rec.SetAttribute("double_arr_attr", double_span);

  nostd::string_view str_arr[kArraySize] = {"Hello", "World", "Test"};
  nostd::span<const nostd::string_view> str_span(str_arr);
  rec.SetAttribute("str_arr_attr", str_span);

  for (int i = 0; i < kArraySize; i++)
  {
    EXPECT_EQ(rec.span().attributes(0).value().array_value().values(i).bool_value(), bool_span[i]);
    EXPECT_EQ(rec.span().attributes(1).value().array_value().values(i).double_value(),
              double_span[i]);
    EXPECT_EQ(rec.span().attributes(2).value().array_value().values(i).string_value(), str_span[i]);
  }
}

/**
 * AttributeValue can contain different int types, such as int, int64_t,
 * unsigned int, and uint64_t. To avoid writing test cases for each, we can
 * use a template approach to test all int types.
 */
template <typename T>
struct IntAttributeTest : public testing::Test
{
  using IntParamType = T;
};

using IntTypes = testing::Types<int, int64_t, unsigned int, uint64_t>;
TYPED_TEST_SUITE(IntAttributeTest, IntTypes);

TYPED_TEST(IntAttributeTest, SetIntSingleAttribute)
{
  using IntType = typename TestFixture::IntParamType;
  IntType i     = 2;
  common::AttributeValue int_val(i);

  OtlpRecordable rec;
  rec.SetAttribute("int_attr", int_val);
  EXPECT_EQ(rec.span().attributes(0).value().int_value(), nostd::get<IntType>(int_val));
}

TYPED_TEST(IntAttributeTest, SetIntArrayAttribute)
{
  using IntType = typename TestFixture::IntParamType;

  const int kArraySize        = 3;
  IntType int_arr[kArraySize] = {4, 5, 6};
  nostd::span<const IntType> int_span(int_arr);

  OtlpRecordable rec;
  rec.SetAttribute("int_arr_attr", int_span);

  for (int i = 0; i < kArraySize; i++)
  {
    EXPECT_EQ(rec.span().attributes(0).value().array_value().values(i).int_value(), int_span[i]);
  }
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
