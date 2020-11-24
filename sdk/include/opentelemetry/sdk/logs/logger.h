/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/logs/logger.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/processor.h"

#include <unordered_map>
#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
class LoggerProvider;

class Logger final : public opentelemetry::logs::Logger
{
public:
  /**
   * Initialize a new logger.
   * @param name The name of this logger instance
   * @param logger_provider The logger provider that owns this logger.
   */
  explicit Logger(opentelemetry::nostd::string_view name,
                  std::shared_ptr<LoggerProvider> logger_provider) noexcept;

  /**
   * Returns the name of this logger.
   */
  opentelemetry::nostd::string_view GetName() noexcept override;

  /**
   * Writes a log record into the processor.
   * @param record The record to write into the processor.
   */
  void Log(
      opentelemetry::nostd::shared_ptr<opentelemetry::logs::LogRecord> record) noexcept override;

private:
  // The logger provider of this Logger. Uses a weak_ptr to avoid cyclic dependancy issues the with
  // logger provider
  std::weak_ptr<LoggerProvider> logger_provider_;

  // The name of this logger
  opentelemetry::nostd::string_view logger_name_;
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
