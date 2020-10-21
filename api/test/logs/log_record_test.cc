#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"

#include <map>

#include <gtest/gtest.h>

using opentelemetry::logs::LogRecord;

TEST(LogRecord, TimestampStore)
{
  // Create a record at the current time
  LogRecord r;

  // Do a large computation
  for (int i = 0; i < 1000; i++)
  {
  }

  // Create a second record at the current time
  LogRecord r2;

  // By default, the timestamp should be set to now()
  EXPECT_NE(r.timestamp, r2.timestamp);
}

TEST(LogRecord, TraceIdStore)
{
  LogRecord r;
  constexpr uint8_t input[] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 6, 5, 4, 3, 2, 1};
  opentelemetry::trace::TraceId traceId(input);

  // Make sure the traceId was written correctly
  EXPECT_TRUE(traceId.IsValid());

  // Convert the traceId back to hex
  char output[32];
  traceId.ToLowerBase16(output);
  EXPECT_EQ("01020304050607080807060504030201", std::string(output, sizeof(output)));
}

TEST(LogRecord, SpanIdStore)
{
  LogRecord r;
  constexpr uint8_t input[] = {1, 2, 3, 4, 5, 6, 7, 8};
  opentelemetry::trace::SpanId spanId(input);

  // Make sure the spanId was written correctly
  EXPECT_TRUE(spanId.IsValid());

  // Convert the spanId back to hex
  char output[16];
  spanId.ToLowerBase16(output);
  EXPECT_EQ("0102030405060708", std::string(output, sizeof(output)));
}

TEST(LogRecord, TraceFlagsStore)
{
  LogRecord r;

  // By default, flag should be 0
  EXPECT_EQ(0, r.trace_flag.flags());

  // Set the traceflag to 1
  opentelemetry::trace::TraceFlags flags{opentelemetry::trace::TraceFlags::kIsSampled};
  r.trace_flag = flags;
  EXPECT_EQ(1, r.trace_flag.flags());
}

TEST(LogRecord, SeverityStore)
{
  LogRecord r;
  r.severity = opentelemetry::logs::Severity::kError2;
  EXPECT_EQ(r.severity, opentelemetry::logs::Severity::kError2);
}

TEST(LogRecord, NameStore)
{
  LogRecord r;
  r.name = "My Name";
  EXPECT_EQ(r.name, "My Name");
}

// Implement once Body field is established
// TEST(LogRecord, BodyStore)
//{
//
//}

/*TEST(LogRecord, ResourceStore)
{
    using M = std::map<std::string, std::string>;
    M m1 = {{"abc", "123"}, {"xyz", "456"}};

    LogRecord r;
    r.SetResource(m1);

    //EXPECT_EQ(common::KeyValueIterableView<M>(m1), r.resource);
}

TEST(LogRecord, AttributesStore)
{

}*/
