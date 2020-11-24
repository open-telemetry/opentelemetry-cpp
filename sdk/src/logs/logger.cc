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

#include "opentelemetry/sdk/logs/logger.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
Logger::Logger(std::shared_ptr<LoggerProvider> logger_provider) noexcept
    : logger_provider_(logger_provider)
{}

void Logger::Log(opentelemetry::nostd::shared_ptr<opentelemetry::logs::LogRecord> record) noexcept
{
  // If this logger does not have a processor, no need to create a log record
  auto processor = logger_provider_.lock()->GetProcessor();
  if (processor == nullptr)
  {
    return;
  }

  // TODO: Sampler logic (should include check for minSeverity)
  auto record_pointer = std::unique_ptr<opentelemetry::logs::LogRecord>(
      new opentelemetry::logs::LogRecord(*record.get()));
  // TODO: Do not want to overwrite user-set timestamp if there already is one -
  // add a flag in the API to check if timestamp is set by user already before setting timestamp

  // Inject timestamp if none is set
  record_pointer->timestamp = core::SystemTimestamp(std::chrono::system_clock::now());
  // TODO: inject traceid/spanid later

  // Send the log record to the processor
  processor->OnReceive(std::move(record_pointer));
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
