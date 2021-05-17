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
#ifdef ENABLE_METRICS_PREVIEW

#  include <memory>
#  include "opentelemetry/sdk/common/exporter_utils.h"
#  include "opentelemetry/sdk/metrics/record.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
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
  virtual sdk::common::ExportResult Export(const std::vector<Record> &records) noexcept = 0;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
