// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace test_common
{

class ScopedTestLogHandler final
{
public:
  struct Entry
  {
    sdk::common::internal_log::LogLevel level;
    std::string msg;
  };

private:
  class LogHandlerImpl : public sdk::common::internal_log::LogHandler
  {
  public:
    void Handle(sdk::common::internal_log::LogLevel level,
                const char * /*file*/,
                int /*line*/,
                const char *msg,
                const sdk::common::AttributeMap & /*attrs*/) noexcept override
    {
      std::lock_guard<std::mutex> lk(mu_);
      entries_.push_back({level, msg ? msg : ""});
    }

    std::vector<Entry> Drain()
    {
      std::lock_guard<std::mutex> lk(mu_);
      return std::exchange(entries_, {});
    }

  private:
    std::mutex mu_;
    std::vector<Entry> entries_;
  };

public:
  explicit ScopedTestLogHandler(sdk::common::internal_log::LogLevel level)
      : previous_handler_(sdk::common::internal_log::GlobalLogHandler::GetLogHandler()),
        previous_level_(sdk::common::internal_log::GlobalLogHandler::GetLogLevel())
  {
    opentelemetry::nostd::shared_ptr<LogHandlerImpl> handler{new LogHandlerImpl{}};
    handler_ = handler.get();
    sdk::common::internal_log::GlobalLogHandler::SetLogHandler(std::move(handler));
    sdk::common::internal_log::GlobalLogHandler::SetLogLevel(level);
  }

  ~ScopedTestLogHandler()
  {
    handler_ = nullptr;
    sdk::common::internal_log::GlobalLogHandler::SetLogHandler(previous_handler_);
    sdk::common::internal_log::GlobalLogHandler::SetLogLevel(previous_level_);
  }

  ScopedTestLogHandler(const ScopedTestLogHandler &)            = delete;
  ScopedTestLogHandler &operator=(const ScopedTestLogHandler &) = delete;
  ScopedTestLogHandler(ScopedTestLogHandler &&)                 = delete;
  ScopedTestLogHandler &operator=(ScopedTestLogHandler &&)      = delete;

  std::vector<Entry> Drain() { return handler_->Drain(); }

private:
  LogHandlerImpl *handler_{nullptr};
  opentelemetry::nostd::shared_ptr<sdk::common::internal_log::LogHandler> previous_handler_;
  sdk::common::internal_log::LogLevel previous_level_;
};

}  // namespace test_common
OPENTELEMETRY_END_NAMESPACE
