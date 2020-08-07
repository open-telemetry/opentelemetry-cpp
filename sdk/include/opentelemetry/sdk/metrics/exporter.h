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

#include <memory>
#include "opentelemetry/sdk/metrics/record.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/**
 * ExportResult is returned as result of exporting a batch of records.
 */
enum class ExportResult
{
  /**
   * Batch was successfully exported.
   */
  kSuccess = 0,
  /**
   * Exporting failed. The caller must not retry exporting the same batch; the
   * batch must be dropped.
   */
  kFailure = 1,

  /**
   * The collection does not have enough space to receive the export batch.
   */
  kFailureFull = 2,

  /**
   * The export has timed out.
   */
  kFailureTimeout = 3,

  /**
   * The export() function was passed an invalid argument.
   */
  kFailureInvalidArgument = 4,
};
/**
 * MetricsExporter defines the interface that protocol-specific span exporters must
 * implement.
 */
class MetricsExporter
{
public:
  virtual ~MetricsExporter() = default;

  /**
   * Exports a vector of Records. This method must not be called
   * concurrently for the same exporter instance.
   * @param records a vector of unique pointers to metric records
   */
  virtual ExportResult Export(const std::vector<Record> &records) noexcept = 0;

  /**
   * In the Metrics specification, there is no Shutdown function required for exporters
   * The Shutdown function can be implemented within exporters that wish to have one,
   * but will not be enforced through this header
   */
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
