#pragma once

#include <memory>
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/resource/resource.h"
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
  kSuccess = 0,
  /**
   * Exporting failed. The caller must not retry exporting the same batch; the
   * batch must be dropped.
   */
  kFailure
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
   *
   * Note: This method must be callable from multiple threads.
   */
  virtual std::unique_ptr<Recordable> MakeRecordable() noexcept = 0;

  /**
   * Exports a batch of span recordables. This method must not be called
   * concurrently for the same exporter instance.
   * @param Resource the resource associated with all exported spans.
   * @param spans a span of unique pointers to span recordables
   */
  virtual ExportResult Export(
      const opentelemetry::sdk::resource::Resource& resource,
      const nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>>
          &spans) noexcept = 0;

  /**
   * Shut down the exporter.
   * @param timeout an optional timeout.
   * @return return the status of the operation.
   */
  virtual bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept = 0;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
