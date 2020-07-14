#include "exporters/otlp/recordable.h"

#include <gtest/gtest.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
TEST(Recordable, SetIds)
{
  const trace::TraceId trace_id(
    std::array<const uint8_t, trace::TraceId::kSize>(
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));

  const trace::SpanId span_id(
    std::array<const uint8_t, trace::SpanId::kSize>(
    {0, 0, 0, 0, 0, 0, 0, 2}));

  const trace::SpanId parent_span_id(
    std::array<const uint8_t, trace::SpanId::kSize>(
    {0, 0, 0, 0, 0, 0, 0, 3}));

  Recordable rec;

  rec.SetIds(trace_id, span_id, parent_span_id);

  EXPECT_EQ(rec.span().trace_id(),
            std::string(reinterpret_cast<const char *>(trace_id.Id().data()), trace::TraceId::kSize));
  EXPECT_EQ(rec.span().span_id(),
            std::string(reinterpret_cast<const char *>(span_id.Id().data()), trace::SpanId::kSize));
  EXPECT_EQ(rec.span().parent_span_id(),
            std::string(reinterpret_cast<const char *>(parent_span_id.Id().data()), trace::SpanId::kSize));
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
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
