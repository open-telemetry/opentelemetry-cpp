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
  kFailure
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
