#pragma once

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/span_data.h"

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
  // Batch was successfully exported.
  rSuccess = 0,
  // Exporting failed. The caller must not retry exporting the same batch; the
  // batch must be dropped.
  rFailedNotRetryable
};
/**
 * SpanExporter defines the interface that protocol-specific span exporters must
 * implement.
 */
class SpanExporter
{
public:
  virtual ~SpanExporter() = default;

  // Exports a batch of spans.
  //
  // This method must not be called concurrently for the same exporter
  // instance.
  virtual ExportResult Export(
      nostd::span<nostd::shared_ptr<opentelemetry::sdk::trace::SpanData>> &spans) noexcept = 0;

  virtual void Shutdown() noexcept = 0;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
