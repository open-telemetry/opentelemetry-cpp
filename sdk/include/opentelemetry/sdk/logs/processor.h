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

#include <chrono>
#include <memory>
#include "opentelemetry/logs/log_record.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
/**
 * This Log Processor is responsible for conversion of logs to exportable
 * representation and passing them to exporters.
 */
class LogProcessor
{
public:
  virtual ~LogProcessor() = default;

  virtual void OnReceive(std::unique_ptr<opentelemetry::logs::LogRecord> &&record) noexcept = 0;

  virtual void ForceFlush(
      std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept = 0;

  virtual void Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept = 0;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
