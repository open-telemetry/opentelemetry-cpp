#pragma once

#include <memory>
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/recordable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * ExportResult is returned as result of exporting a span batch.
 */
enum class ExportResult
{
  /**
   * Batch was successfully exported.
   */
  rSuccess = 0,
  /**
   * Exporting failed. The caller must not retry exporting the same batch; the
   * batch must be dropped.
   */
  rFailure
};
/**
 * SpanExporter defines the interface that protocol-specific span exporters must
 * implement.
 */
class SpanExporter
{
public:
  virtual ~SpanExporter() = default;

  /**
   * Create a span recordable. This object will be used to record span data and
   * will subsequently be passed to SpanExporter::Export. Vendors can implement
   * custom recordables or use the default SpanData recordable provided by the
   * SDK.
   * @return a newly initialized Recordable object
   */
  virtual std::unique_ptr<Recordable> MakeRecordable() noexcept = 0;

  /**
   * Exports a batch of span recordables. This method must not be called
   * concurrently for the same exporter instance.
   * @param spans a span of shared pointers to span recordables
   */
  virtual ExportResult Export(
      nostd::span<std::shared_ptr<opentelemetry::sdk::trace::Recordable>> &spans) noexcept = 0;

  /**
   * Shut down the exporter.
   */
  virtual void Shutdown() noexcept = 0;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
