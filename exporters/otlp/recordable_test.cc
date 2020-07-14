#include "exporters/otlp/recordable.h"

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
  nostd::string_view name = "TestSpan";
  rec.SetName(name);
  EXPECT_EQ(rec.span().name(), name);
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
  trace::CanonicalCode code(trace::CanonicalCode::OK);
  nostd::string_view description = "For test";
  rec.SetStatus(code, description);

  EXPECT_EQ(rec.span().status().code(), opentelemetry::proto::trace::v1::Status_StatusCode(code));
  EXPECT_EQ(rec.span().status().message(), description);
}

TEST(Recordable, SetSingleAtrribute)
{
  Recordable rec;
  common::AttributeValue bool_val(true);
  rec.SetAttribute("bool_attr", bool_val);

  common::AttributeValue int_val(22);
  rec.SetAttribute("int_attr", int_val);

  common::AttributeValue double_val(3.3);
  rec.SetAttribute("double_attr", double_val);

  common::AttributeValue str_val(nostd::string_view("Test"));
  rec.SetAttribute("str_attr", str_val);

  EXPECT_EQ(rec.span().attributes(0).value().bool_value(), nostd::get<bool>(bool_val));
  EXPECT_EQ(rec.span().attributes(1).value().int_value(), nostd::get<int>(int_val));
  EXPECT_EQ(rec.span().attributes(2).value().double_value(), nostd::get<double>(double_val));
  EXPECT_EQ(rec.span().attributes(3).value().string_value(), nostd::get<nostd::string_view>(str_val).data());
}

TEST(Recordable, SetArrayAtrributes)
{
  Recordable rec;
  bool bool_arr[2] = {true, false};
  nostd::span<const bool> bool_span(bool_arr);
  common::AttributeValue array_bool_val(bool_span);
  rec.SetAttribute("array_bool_val", array_bool_val);

  EXPECT_EQ(rec.span().attributes(0).value().array_value().values(0).bool_value(), bool_span[0]);
  EXPECT_EQ(rec.span().attributes(0).value().array_value().values(1).bool_value(), bool_span[1]);
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
