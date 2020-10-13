#include "opentelemetry/logs/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"

#include <gtest/gtest.h>

using opentelemetry::logs::Logger;
using opentelemetry::logs::LoggerProvider;
using opentelemetry::logs::Provider;

// TODO
TEST(LogRecord, TimestampStore) {}
TEST(LogRecord, TraceIdStore){}
TEST(LogRecord, SpanIdStore){}
TEST(LogRecord, TraceFlagsStore){}
TEST(LogRecord, SeverityTextStore){}
TEST(LogRecord, SeverityNumberStore){}
TEST(LogRecord, NameStore){}
TEST(LogRecord, BodyStore){}
TEST(LogRecord, ResourceStore){}
TEST(LogRecord, AttributesStore){}
