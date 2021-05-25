// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <array>

#include "opentelemetry/logs/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"

using opentelemetry::logs::Logger;
using opentelemetry::logs::LoggerProvider;
using opentelemetry::logs::Provider;
using opentelemetry::nostd::shared_ptr;
using opentelemetry::nostd::span;
using opentelemetry::nostd::string_view;

class TestProvider : public LoggerProvider
{
  shared_ptr<Logger> GetLogger(string_view library_name, string_view options) override
  {
    return shared_ptr<Logger>(nullptr);
  }

  shared_ptr<Logger> GetLogger(string_view library_name, span<string_view> args) override
  {
    return shared_ptr<Logger>(nullptr);
  }
};

TEST(Provider, GetLoggerProviderDefault)
{
  auto tf = Provider::GetLoggerProvider();
  EXPECT_NE(nullptr, tf);
}

TEST(Provider, SetLoggerProvider)
{
  auto tf = shared_ptr<LoggerProvider>(new TestProvider());
  Provider::SetLoggerProvider(tf);
  ASSERT_EQ(tf, Provider::GetLoggerProvider());
}

TEST(Provider, MultipleLoggerProviders)
{
  auto tf = shared_ptr<LoggerProvider>(new TestProvider());
  Provider::SetLoggerProvider(tf);
  auto tf2 = shared_ptr<LoggerProvider>(new TestProvider());
  Provider::SetLoggerProvider(tf2);

  ASSERT_NE(Provider::GetLoggerProvider(), tf);
}

TEST(Provider, GetLogger)
{
  auto tf = shared_ptr<LoggerProvider>(new TestProvider());
  // tests GetLogger(name, options)
  auto logger = tf->GetLogger("logger1");
  EXPECT_EQ(nullptr, logger);

  // tests GetLogger(name, arguments)

  std::array<string_view, 1> sv{"string"};
  span<string_view> args{sv};
  auto logger2 = tf->GetLogger("logger2", args);
  EXPECT_EQ(nullptr, logger2);
}
