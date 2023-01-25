// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/trace/recordable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * SpanExporter defines the interface that protocol-specific span exporters must
 * implement.
 */
class OPENTELEMETRY_EXPORT SpanExporter
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
   * @param spans a span of unique pointers to span recordables
   */
  virtual sdk::common::ExportResult Export(
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

#if defined(OPENTELEMETRY_EXPORT)

namespace std
{

//
// Partial specialization of default_delete used by unique_ptr.
// This makes the delete of the type in unique_ptr happening in the DLL where it
// is allocated.
//
template <>
struct OPENTELEMETRY_EXPORT default_delete<OPENTELEMETRY_NAMESPACE::sdk::trace::SpanExporter>
{
public:
  void operator()(OPENTELEMETRY_NAMESPACE::sdk::trace::SpanExporter *span_exporter)
  {
    delete span_exporter;
  }
};

}  // namespace std

#endif  // OPENTELEMETRY_EXPORT
