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
   * @param logger_provider The logger provider that owns this logger.
   */
  explicit Logger(std::shared_ptr<LoggerProvider> logger_provider) noexcept;

  /**
   * Writes a log record into the processor.
   * @param timestamp the timestamp the log record was created.
   * @param name the name of the log event.
   * @param severity the severity level of the log event.
   * @param message the string message of the log (perhaps support std::fmt or fmt-lib format).
   * @param record the log record (object type LogRecord) that is logged.
   * @param attributes the attributes, stored as a 2D list of key/value pairs, that are associated wtih the log event. 
   * @param trace_id the trace id associated with the log event.
   * @param span_id the span id associate with the log event.
   * @param trace_flags the trace flags associated with the log event.
   * @throws No exceptions under any circumstances.   */
  void Log(core::SystemTimestamp timestamp, opentelemetry::logs::Severity severity,  nostd::string_view name, nostd::string_view body, const common::KeyValueIterable &resource, const common::KeyValueIterable &attributes, trace::TraceId trace_id, trace::SpanId span_id, trace::TraceFlags trace_flags) noexcept override;
 
private:
  // The logger provider of this Logger. Uses a weak_ptr to avoid cyclic dependancy issues the with
  // logger provider
  std::weak_ptr<LoggerProvider> logger_provider_;
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
