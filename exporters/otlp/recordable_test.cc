#include "exporters/otlp/recordable.h"

#include <gtest/gtest.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
TEST(Recordable, SetIds)
{
  Recordable rec;

  const uint8_t trace_id_buff[trace::TraceId::kSize]      = {0, 0, 0, 0, 0, 0, 0, 0,
                                                        0, 0, 0, 0, 0, 0, 0, 1};
  const uint8_t span_id_buff[trace::SpanId::kSize]        = {0, 0, 0, 0, 0, 0, 0, 2};
  const uint8_t parent_span_id_buff[trace::SpanId::kSize] = {0, 0, 0, 0, 0, 0, 0, 3};

  const trace::TraceId trace_id(
      nostd::span<const uint8_t, trace::TraceId::kSize>(trace_id_buff, trace::TraceId::kSize));
  const trace::SpanId span_id(
      nostd::span<const uint8_t, trace::SpanId::kSize>(span_id_buff, trace::SpanId::kSize));
  const trace::SpanId parent_span_id(
      nostd::span<const uint8_t, trace::SpanId::kSize>(parent_span_id_buff, trace::SpanId::kSize));

  rec.SetIds(trace_id, span_id, parent_span_id);

  EXPECT_EQ(rec.span().trace_id(),
            std::string(reinterpret_cast<const char *>(trace_id_buff), trace::TraceId::kSize));
  EXPECT_EQ(rec.span().span_id(),
            std::string(reinterpret_cast<const char *>(span_id_buff), trace::SpanId::kSize));
  EXPECT_EQ(rec.span().parent_span_id(),
            std::string(reinterpret_cast<const char *>(parent_span_id_buff), trace::SpanId::kSize));
}

TEST(Recordable, SetName)
{
  Recordable rec;
  nostd::string_view name = "TestSpan";
  rec.SetName(name);
  ASSERT_EQ(rec.span().name(), name);
}

TEST(Recordable, SetStartTime)
{
  Recordable rec;
  std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
  core::SystemTimestamp start_timestamp(start_time);

  uint64_t unix_start =
      std::chrono::duration_cast<std::chrono::nanoseconds>(start_time.time_since_epoch()).count();

  rec.SetStartTime(start_timestamp);
  ASSERT_EQ(rec.span().start_time_unixnano(), unix_start);
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

  ASSERT_EQ(rec.span().end_time_unixnano(), unix_end);
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
