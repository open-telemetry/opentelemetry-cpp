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

#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/log_record.h"
#include "opentelemetry/version.h"

#include <iostream>
#include <sstream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{
/**
 * The OStreamLogExporter exports logs through an ostream (default set to std::cout)
 */
class OStreamLogExporter final : public opentelemetry::sdk::logs::LogExporter
{
public:
  /**
   * Create an OStreamLogExporter. This constructor takes in a reference to an ostream that the
   * Export() method will send log data into. The default ostream is set to stdout.
   */
  explicit OStreamLogExporter(std::ostream &sout = std::cout) noexcept;

  std::unique_ptr<sdk::logs::Recordable> MakeRecordable() noexcept override;

  /**
   * Exports a span of logs sent from the processor.
   */
  opentelemetry::sdk::logs::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<sdk::logs::Recordable>> &records) noexcept
      override;

  /**
   * Marks the OStream Log Exporter as shut down.
   */
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

private:
  // The OStream to send the logs to
  std::ostream &sout_;
  // Whether this exporter has been shut down
  bool is_shutdown_ = false;
};
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
