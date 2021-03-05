#include "opentelemetry/exporters/otlp/recordable.h"
#include <gtest/gtest.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
TEST(Recordable, SetIds)
{
  const trace::TraceId trace_id(std::array<const uint8_t, trace::TraceId::kSize>(
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));

  const trace::SpanId span_id(
      std::array<const uint8_t, trace::SpanId::kSize>({0, 0, 0, 0, 0, 0, 0, 2}));

  const trace::SpanId parent_span_id(
      std::array<const uint8_t, trace::SpanId::kSize>({0, 0, 0, 0, 0, 0, 0, 3}));

  Recordable rec;

  rec.SetIds(trace_id, span_id, parent_span_id);

  EXPECT_EQ(rec.span().trace_id(), std::string(reinterpret_cast<const char *>(trace_id.Id().data()),
                                               trace::TraceId::kSize));
  EXPECT_EQ(rec.span().span_id(),
            std::string(reinterpret_cast<const char *>(span_id.Id().data()), trace::SpanId::kSize));
  EXPECT_EQ(rec.span().parent_span_id(),
            std::string(reinterpret_cast<const char *>(parent_span_id.Id().data()),
                        trace::SpanId::kSize));
}

TEST(Recordable, SetName)
{
  Recordable rec;
  nostd::string_view name = "Test Span";
  rec.SetName(name);
  EXPECT_EQ(rec.span().name(), name);
}
TEST(Recordable, SetSpanKind)
{
  Recordable rec;
  opentelemetry::trace::SpanKind span_kind = opentelemetry::trace::SpanKind::kServer;
  rec.SetSpanKind(span_kind);
  EXPECT_EQ(rec.span().kind(),
            opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_SERVER);
}

TEST(Recordable, SetStartTime)
{
  Recordable rec;
  std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
  core::SystemTimestamp start_timestamp(start_time);

  uint64_t unix_start =
      std::chrono::duration_cast<std::chrono::nanoseconds>(start_time.time_since_epoch()).count();

  rec.SetStartTime(start_timestamp);
  EXPECT_EQ(rec.span().start_time_unix_nano(), unix_start);
}

TEST(Recordable, SetDuration)
{
  Recordable rec;
  // Start time is 0
  core::SystemTimestamp start_timestamp;

  std::chrono::nanoseconds duration(10);
  uint64_t unix_end = duration.count();

  rec.SetStartTime(start_timestamp);
  rec.SetDuration(duration);

  EXPECT_EQ(rec.span().end_time_unix_nano(), unix_end);
}

TEST(Recordable, SetStatus)
{
  Recordable rec;
  trace::StatusCode code(trace::StatusCode::kOk);
  nostd::string_view description = "For test";
  rec.SetStatus(code, description);

  EXPECT_EQ(rec.span().status().code(), opentelemetry::proto::trace::v1::Status_StatusCode(code));
  EXPECT_EQ(rec.span().status().message(), description);
}

TEST(Recordable, AddEventDefault)
{
  Recordable rec;
  nostd::string_view name = "Test Event";

  std::chrono::system_clock::time_point event_time = std::chrono::system_clock::now();
  core::SystemTimestamp event_timestamp(event_time);

  rec.sdk::trace::Recordable::AddEvent(name, event_timestamp);

  uint64_t unix_event_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(event_time.time_since_epoch()).count();

  EXPECT_EQ(rec.span().events(0).name(), name);
  EXPECT_EQ(rec.span().events(0).time_unix_nano(), unix_event_time);
  EXPECT_EQ(rec.span().events(0).attributes().size(), 0);
}

TEST(Recordable, AddEventWithAttributes)
{
  Recordable rec;
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

TEST(Recordable, AddLink)
{
  Recordable rec;
  const int kNumAttributes              = 3;
  std::string keys[kNumAttributes]      = {"attr1", "attr2", "attr3"};
  int values[kNumAttributes]            = {5, 12, 40};
  std::map<std::string, int> attributes = {
      {keys[0], values[0]}, {keys[1], values[1]}, {keys[2], values[2]}};

  auto trace_id = rec.span().trace_id();
  auto span_id  = rec.span().span_id();

  rec.AddLink(trace::SpanContext(false, false),
              common::KeyValueIterableView<std::map<std::string, int>>(attributes));

  EXPECT_EQ(rec.span().trace_id(), trace_id);
  EXPECT_EQ(rec.span().span_id(), span_id);
  for (int i = 0; i < kNumAttributes; i++)
  {
    EXPECT_EQ(rec.span().links(0).attributes(i).key(), keys[i]);
    EXPECT_EQ(rec.span().links(0).attributes(i).value().int_value(), values[i]);
  }
}

// Test non-int single types. Int single types are tested using templates (see IntAttributeTest)
TEST(Recordable, SetSingleAtrribute)
{
  Recordable rec;
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
TEST(Recordable, SetArrayAtrribute)
{
  Recordable rec;
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
TYPED_TEST_CASE(IntAttributeTest, IntTypes);

TYPED_TEST(IntAttributeTest, SetIntSingleAttribute)
{
  using IntType = typename TestFixture::IntParamType;
  IntType i     = 2;
  common::AttributeValue int_val(i);

  Recordable rec;
  rec.SetAttribute("int_attr", int_val);
  EXPECT_EQ(rec.span().attributes(0).value().int_value(), nostd::get<IntType>(int_val));
}

TYPED_TEST(IntAttributeTest, SetIntArrayAttribute)
{
  using IntType = typename TestFixture::IntParamType;

  const int kArraySize        = 3;
  IntType int_arr[kArraySize] = {4, 5, 6};
  nostd::span<const IntType> int_span(int_arr);

  Recordable rec;
  rec.SetAttribute("int_arr_attr", int_span);

  for (int i = 0; i < kArraySize; i++)
  {
    EXPECT_EQ(rec.span().attributes(0).value().array_value().values(i).int_value(), int_span[i]);
  }
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
