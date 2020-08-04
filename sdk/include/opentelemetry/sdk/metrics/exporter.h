#pragma once

#include <vector>

#include "opentelemetry/sdk/metrics/record.h"
#include "opentelemetry/sdk/metrics/return_code.h"
#include "opentelemetry/version.h"

/**
 * This class is an interface that defines all basic behaviors of an exporter.
 * This interface should be available to use for metrics exporters, and
 * the design of this interface has been based on the SpanExporter class.
 */

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class MetricsExporter
{
public:
  /**
   * Exports a batch of Metric Records.
   * @param records: a collection of records to export
   * @return: returns a ReturnCode detailing a success, or type of failure
   */
  virtual ReturnCode Export(std::vector<Record> &records) noexcept = 0;

  /**
   * Shuts down the exporter and does cleanup.
   */
  virtual void Shutdown() noexcept = 0;
};
}  // namespace metrics
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
