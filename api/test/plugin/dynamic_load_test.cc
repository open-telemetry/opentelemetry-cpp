// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/plugin/dynamic_load.h"
#ifdef _WIN32
#  include "opentelemetry/plugin/detail/dynamic_load_windows.h"
#else
#  include "opentelemetry/plugin/detail/dynamic_load_unix.h"
#endif
#include "opentelemetry/plugin/factory.h"

#include <gtest/gtest.h>

TEST(LoadFactoryTest, FailureTest)
{
  std::string error_message;
  auto factory = opentelemetry::plugin::LoadFactory("no-such-plugin", error_message);
  EXPECT_EQ(factory, nullptr);
  EXPECT_FALSE(error_message.empty());
}
