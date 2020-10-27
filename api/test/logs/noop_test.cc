#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <string>

#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/logs/noop.h"

using opentelemetry::core::SystemTimestamp;
using opentelemetry::logs::Logger;
using opentelemetry::logs::NoopLogger;

TEST(NoopTest, UseNoopLoggers)
{
  std::shared_ptr<Logger> logger{new NoopLogger{}};
  SystemTimestamp t1;
  auto nlog = logger->log(LogRecord{.timestamp = t1, .name = "noop log event"});
  nlog->EXPECT_EQ(s1->GetLogger(), "Noop Logger");

  s1->GetContext();
}
