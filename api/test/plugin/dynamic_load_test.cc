#include "opentelemetry/plugin/dynamic_load.h"

#include <gtest/gtest.h>

TEST(LoadFactoryTest, FailureTest)
{
  std::string error_message;
  auto factory = opentelemetry::plugin::LoadFactory("no-such-plugin", error_message);
  EXPECT_EQ(factory, nullptr);
  EXPECT_FALSE(error_message.empty());
}
