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

#include "opentelemetry/logs/logger.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/processor.h"

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
  const opentelemetry::nostd::string_view GetName() noexcept override;

  /**
   * Writes a log record into the processor.
   * @param severity the severity level of the log event.
   * @param name the name of the log event.
   * @param message the string message of the log (perhaps support std::fmt or fmt-lib format).
   * @param resource the resources, stored as a 2D list of key/value pairs, that are associated
   * with the log event.
   * @param attributes the attributes, stored as a 2D list of key/value pairs, that are associated
   * with the log event.
   * @param trace_id the trace id associated with the log event.
   * @param span_id the span id associate with the log event.
   * @param trace_flags the trace flags associated with the log event.
   * @param timestamp the timestamp the log record was created.
   * @throws No exceptions under any circumstances.   */
  void Log(opentelemetry::logs::Severity severity,
           nostd::string_view name,
           nostd::string_view body,
           const opentelemetry::common::KeyValueIterable &resource,
           const opentelemetry::common::KeyValueIterable &attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags,
           core::SystemTimestamp timestamp) noexcept override;

private:
  // The logger provider of this Logger. Uses a weak_ptr to avoid cyclic dependancy issues the with
  // logger provider
  std::weak_ptr<LoggerProvider> logger_provider_;

  // The name of this logger
  std::string logger_name_;
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
