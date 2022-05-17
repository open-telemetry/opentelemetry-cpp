// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include <gtest/gtest.h>

#  include "opentelemetry/exporters/otlp/otlp_log_recordable.h"
#  include "opentelemetry/sdk/resource/experimental_semantic_conventions.h"
#  include "opentelemetry/sdk/resource/resource.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace resource = opentelemetry::sdk::resource;
namespace proto    = opentelemetry::proto;

TEST(OtlpLogRecordable, SetTimestamp)
{
  OtlpLogRecordable rec;
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  common::SystemTimestamp start_timestamp(now);

  uint64_t unix_start =
      std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

  rec.SetTimestamp(start_timestamp);
  EXPECT_EQ(rec.log_record().time_unix_nano(), unix_start);
}

TEST(OtlpLogRecordable, SetSeverity)
{
  OtlpLogRecordable rec;
  rec.SetSeverity(opentelemetry::logs::Severity::kError);

  EXPECT_EQ(rec.log_record().severity_number(), proto::logs::v1::SEVERITY_NUMBER_ERROR);
  EXPECT_EQ(rec.log_record().severity_text(), "ERROR");
}

TEST(OtlpLogRecordable, SetBody)
{
  OtlpLogRecordable rec;
  nostd::string_view name = "Test Log Message";
  rec.SetBody(name);
  EXPECT_EQ(rec.log_record().body().string_value(), name);
}

TEST(OtlpLogRecordable, SetTraceId)
{
  OtlpLogRecordable rec;
  uint8_t trace_id_bin[opentelemetry::trace::TraceId::kSize] = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  std::string expected_bytes;
  expected_bytes.assign(
      reinterpret_cast<char *>(trace_id_bin),
      reinterpret_cast<char *>(trace_id_bin) + opentelemetry::trace::TraceId::kSize);
  rec.SetTraceId(opentelemetry::trace::TraceId{trace_id_bin});
  EXPECT_EQ(rec.log_record().trace_id(), expected_bytes);
}

TEST(OtlpLogRecordable, SetSpanId)
{
  OtlpLogRecordable rec;
  uint8_t span_id_bin[opentelemetry::trace::SpanId::kSize] = {'7', '6', '5', '4',
                                                              '3', '2', '1', '0'};
  std::string expected_bytes;
  expected_bytes.assign(
      reinterpret_cast<char *>(span_id_bin),
      reinterpret_cast<char *>(span_id_bin) + opentelemetry::trace::SpanId::kSize);
  rec.SetSpanId(opentelemetry::trace::SpanId{span_id_bin});
  EXPECT_EQ(rec.log_record().span_id(), expected_bytes);
}

TEST(OtlpLogRecordable, SetResource)
{
  OtlpLogRecordable rec;
  const std::string service_name_key = "service.name";
  std::string service_name           = "test-otlp";
  auto resource =
      opentelemetry::sdk::resource::Resource::Create({{service_name_key, service_name}});
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

TEST(OtlpLogRecordable, DefaultResource)
{
  OtlpLogRecordable rec;

  auto proto_resource      = rec.ProtoResource();
  int found_resource_count = 0;
  for (int i = 0; i < proto_resource.attributes_size(); i++)
  {
    auto attr = proto_resource.attributes(static_cast<int>(i));
    if (attr.key() ==
        opentelemetry::sdk::resource::attr(OTEL_CPP_CONST_HASHCODE(AttrTelemetrySdkLanguage)))
    {
      EXPECT_EQ(attr.value().string_value(), "cpp");
      ++found_resource_count;
    }
    else if (attr.key() ==
             opentelemetry::sdk::resource::attr(OTEL_CPP_CONST_HASHCODE(AttrTelemetrySdkName)))
    {
      EXPECT_EQ(attr.value().string_value(), "opentelemetry");
      ++found_resource_count;
    }
    else if (attr.key() ==
             opentelemetry::sdk::resource::attr(OTEL_CPP_CONST_HASHCODE(AttrTelemetrySdkVersion)))
    {
      EXPECT_EQ(attr.value().string_value(), OPENTELEMETRY_SDK_VERSION);
      ++found_resource_count;
    }
  }
  EXPECT_EQ(3, found_resource_count);
}

// Test non-int single types. Int single types are tested using templates (see IntAttributeTest)
TEST(OtlpLogRecordable, SetSingleAttribute)
{
  OtlpLogRecordable rec;
  nostd::string_view bool_key = "bool_attr";
  common::AttributeValue bool_val(true);
  rec.SetAttribute(bool_key, bool_val);

  nostd::string_view double_key = "double_attr";
  common::AttributeValue double_val(3.3);
  rec.SetAttribute(double_key, double_val);

  nostd::string_view str_key = "str_attr";
  common::AttributeValue str_val(nostd::string_view("Test"));
  rec.SetAttribute(str_key, str_val);

  EXPECT_EQ(rec.log_record().attributes(0).key(), bool_key);
  EXPECT_EQ(rec.log_record().attributes(0).value().bool_value(), nostd::get<bool>(bool_val));

  EXPECT_EQ(rec.log_record().attributes(1).key(), double_key);
  EXPECT_EQ(rec.log_record().attributes(1).value().double_value(), nostd::get<double>(double_val));

  EXPECT_EQ(rec.log_record().attributes(2).key(), str_key);
  EXPECT_EQ(rec.log_record().attributes(2).value().string_value(),
            nostd::get<nostd::string_view>(str_val).data());
}

// Test non-int array types. Int array types are tested using templates (see IntAttributeTest)
TEST(OtlpLogRecordable, SetArrayAttribute)
{
  OtlpLogRecordable rec;
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
    EXPECT_EQ(rec.log_record().attributes(0).value().array_value().values(i).bool_value(),
              bool_span[i]);
    EXPECT_EQ(rec.log_record().attributes(1).value().array_value().values(i).double_value(),
              double_span[i]);
    EXPECT_EQ(rec.log_record().attributes(2).value().array_value().values(i).string_value(),
              str_span[i]);
  }
}

TEST(OtlpLogRecordable, SetInstrumentationLibrary)
{
  OtlpLogRecordable rec;
  auto inst_lib =
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create("test", "v1");
  rec.SetInstrumentationLibrary(*inst_lib);
  EXPECT_EQ(rec.GetInstrumentationLibrary(), *inst_lib);
}

/**
 * AttributeValue can contain different int types, such as int, int64_t,
 * unsigned int, and uint64_t. To avoid writing test cases for each, we can
 * use a template approach to test all int types.
 */
template <typename T>
struct OtlpLogRecordableIntAttributeTest : public testing::Test
{
  using IntParamType = T;
};

using IntTypes = testing::Types<int, int64_t, unsigned int, uint64_t>;
TYPED_TEST_SUITE(OtlpLogRecordableIntAttributeTest, IntTypes);

TYPED_TEST(OtlpLogRecordableIntAttributeTest, SetIntSingleAttribute)
{
  using IntType = typename TestFixture::IntParamType;
  IntType i     = 2;
  common::AttributeValue int_val(i);

  OtlpLogRecordable rec;
  rec.SetAttribute("int_attr", int_val);
  EXPECT_EQ(rec.log_record().attributes(0).value().int_value(), nostd::get<IntType>(int_val));
}

TYPED_TEST(OtlpLogRecordableIntAttributeTest, SetIntArrayAttribute)
{
  using IntType = typename TestFixture::IntParamType;

  const int kArraySize        = 3;
  IntType int_arr[kArraySize] = {4, 5, 6};
  nostd::span<const IntType> int_span(int_arr);

  OtlpLogRecordable rec;
  rec.SetAttribute("int_arr_attr", int_span);

  for (int i = 0; i < kArraySize; i++)
  {
    EXPECT_EQ(rec.log_record().attributes(0).value().array_value().values(i).int_value(),
              int_span[i]);
  }
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
