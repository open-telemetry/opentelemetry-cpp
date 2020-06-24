#include "exporters/otlp/recordable.h"

#include <gtest/gtest.h>

using namespace opentelemetry::exporter::otlp;

OPENTELEMETRY_BEGIN_NAMESPACE

TEST(Recordable, SetIds)
{
    Recordable rec;

    opentelemetry::trace::TraceId trace_id;
    opentelemetry::trace::SpanId span_id;
    opentelemetry::trace::SpanId parent_span_id;

    rec.SetIds(trace_id, span_id, parent_span_id);

    char trace_id_buff[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    char span_id_buff[8] = {0,0,0,0,0,0,0,0};

    ASSERT_EQ(rec.span().trace_id(), trace_id_buff);
    ASSERT_EQ(rec.span().span_id(), span_id_buff);
    ASSERT_EQ(rec.span().parent_span_id(), span_id_buff);
}

TEST(Recordable, OtlpRecordable)
{
    Recordable rec;

    // For SetName
    nostd::string_view name = "TestSpan";

    // For SetStartTime
    std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
    core::SystemTimestamp start_timestamp(start_time);

    uint64_t unix_start = std::chrono::duration_cast<std::chrono::nanoseconds>(
                            start_time.time_since_epoch()).count();

    // For SetDuration
    std::chrono::nanoseconds duration(10);
    uint64_t unix_end = unix_start + duration.count();

    rec.SetName(name);
    rec.SetStartTime(start_timestamp);
    rec.SetDuration(duration);

    ASSERT_EQ(rec.span().name(), name);
    ASSERT_EQ(rec.span().start_time_unixnano(), unix_start);
    ASSERT_EQ(rec.span().end_time_unixnano(), unix_end);
}
OPENTELEMETRY_END_NAMESPACE