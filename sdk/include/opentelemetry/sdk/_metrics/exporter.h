// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include <memory>
#  include "opentelemetry/sdk/_metrics/record.h"
#  include "opentelemetry/sdk/common/exporter_utils.h"

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
