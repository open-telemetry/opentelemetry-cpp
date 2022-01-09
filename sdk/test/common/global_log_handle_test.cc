// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"

#include <gtest/gtest.h>

#include <cstring>

class CustomLogHandler : public opentelemetry::sdk::common::internal_log::LogHandler
{
public:
  void Handle(opentelemetry::sdk::common::internal_log::LogLevel level,
              const char *,
              int,
              const char *msg,
              const opentelemetry::sdk::common::AttributeMap &) noexcept override
  {
    if (level == opentelemetry::sdk::common::internal_log::LogLevel::Debug)
    {
      EXPECT_EQ(0, strncmp(msg, "Debug message", 13));
    }
    else if (level == opentelemetry::sdk::common::internal_log::LogLevel::Error)
    {
      EXPECT_EQ(0, strncmp(msg, "Error message", 13));
    }
    ++count;
  }

  size_t count = 0;
};

TEST(GlobalLogHandleTest, CustomLogHandler)
{
  using opentelemetry::sdk::common::internal_log::LogHandler;
  auto backup_log_handle =
      opentelemetry::sdk::common::internal_log::GlobalLogHandler::GetLogHandler();
  auto backup_log_level = opentelemetry::sdk::common::internal_log::GlobalLogHandler::GetLogLevel();

  auto custom_log_handler = opentelemetry::nostd::shared_ptr<LogHandler>(new CustomLogHandler{});
  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler(custom_log_handler);
  auto before_count = static_cast<CustomLogHandler *>(custom_log_handler.get())->count;
  opentelemetry::sdk::common::AttributeMap attributes = {
      {"url", "https://opentelemetry.io/"}, {"content-length", 0}, {"content-type", "text/html"}};
  OTEL_INTERNAL_LOG_ERROR("Error message");
  OTEL_INTERNAL_LOG_DEBUG("Debug message. Headers:", attributes);
  EXPECT_EQ(before_count + 1, static_cast<CustomLogHandler *>(custom_log_handler.get())->count);

  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogLevel(
      opentelemetry::sdk::common::internal_log::LogLevel::Debug);
  OTEL_INTERNAL_LOG_ERROR("Error message");
  OTEL_INTERNAL_LOG_DEBUG("Debug message. Headers:", attributes);
  EXPECT_EQ(before_count + 3, static_cast<CustomLogHandler *>(custom_log_handler.get())->count);

  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler(backup_log_handle);
  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogLevel(backup_log_level);
}
