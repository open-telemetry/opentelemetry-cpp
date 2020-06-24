#include "exporters/otlp/recordable.h"

#include <gtest/gtest.h>

using namespace opentelemetry::exporter::otlp;

TEST(Recordable, SetIds)
{
    Recordable* rec = new Recordable();

    const uint8_t trace_id_buff[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    const uint8_t span_id_buff[8] = {0,0,0,0,0,0,0,2};
    const uint8_t parent_span_id_buff[8] = {0,0,0,0,0,0,0,3};

    opentelemetry::nostd::span<const uint8_t, 16>trace_id_span(trace_id_buff, 16);
    opentelemetry::nostd::span<const uint8_t, 8> span_id_span(span_id_buff, 8);
    opentelemetry::nostd::span<const uint8_t, 8> parent_span_id_span(parent_span_id_buff, 8);

    opentelemetry::trace::TraceId trace_id(trace_id_span);
    opentelemetry::trace::SpanId span_id(span_id_span);
    opentelemetry::trace::SpanId parent_span_id(parent_span_id_span);

    rec->SetIds(trace_id, span_id, parent_span_id);

    ASSERT_EQ(rec->span().trace_id(), (char*)trace_id_buff);
    ASSERT_EQ(rec->span().span_id(), (char*)span_id_buff);
    ASSERT_EQ(rec->span().parent_span_id(), (char*)parent_span_id_buff);
}

TEST(Recordable, SetName)
{
    Recordable* rec = new Recordable();
    opentelemetry::nostd::string_view name = "TestSpan";
    rec->SetName(name);
    ASSERT_EQ(rec->span().name(), name);
}

TEST(Recordable, SetStartTime)
{
    Recordable* rec = new Recordable();
    std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
    opentelemetry::core::SystemTimestamp start_timestamp(start_time);

    uint64_t unix_start = std::chrono::duration_cast<std::chrono::nanoseconds>(
                            start_time.time_since_epoch()).count();

    rec->SetStartTime(start_timestamp);
    ASSERT_EQ(rec->span().start_time_unixnano(), unix_start);
}

TEST(Recordable, SetDuration)
{
    Recordable* rec = new Recordable();
    // Start time is 0
    opentelemetry::core::SystemTimestamp start_timestamp;

    std::chrono::nanoseconds duration(10);
    uint64_t unix_end = duration.count();

    rec->SetStartTime(start_timestamp);
    rec->SetDuration(duration);

    ASSERT_EQ(rec->span().end_time_unixnano(), unix_end);
}
