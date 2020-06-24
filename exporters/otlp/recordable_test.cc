#include "exporters/otlp/recordable.h"

#include <gtest/gtest.h>

using namespace opentelemetry::exporter::otlp;

OPENTELEMETRY_BEGIN_NAMESPACE

TEST(Recordable, SetIds)
{
    Recordable rec;

    // trace::TraceId trace_id();
    // trace::SpanId span_id();
    // trace::SpanId parent_span_id();

    // nostd::span<const uint8_t, 16> span_trace_id(0, 16);
    // nostd::span<const uint8_t, 8> span_span_id(0, 8);
    // nostd::span<const uint8_t, 8> span_parent_span_id(0, 8);

    // trace::TraceId trace_id(span_trace_id);
    // trace::SpanId span_id(span_span_id);
    // trace::SpanId parent_span_id(span_parent_span_id);

    // constexpr uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8};
    // trace::TraceId trace_id(buf);
    // trace::SpanId span_id(buf);
    // trace::SpanId parent_span_id(buf);

    opentelemetry::trace::TraceId trace_id;
    opentelemetry::trace::SpanId span_id;
    opentelemetry::trace::SpanId parent_span_id;

    char trace_id_buff[32] = {0};
    char span_id_buff[16] = {0};
    char parent_span_id_buff[16] = {0};

    trace_id.ToLowerBase16(trace_id_buff);
    span_id.ToLowerBase16(span_id_buff);
    parent_span_id.ToLowerBase16(parent_span_id_buff);

    rec.SetIds(trace_id, span_id, parent_span_id);

    ASSERT_EQ(rec.span().trace_id(), trace_id_buff);
    ASSERT_EQ(rec.span().span_id(), span_id_buff);
    ASSERT_EQ(rec.span().parent_span_id(), parent_span_id_buff);
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