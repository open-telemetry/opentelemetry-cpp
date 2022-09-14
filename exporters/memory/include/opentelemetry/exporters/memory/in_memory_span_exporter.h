// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include <mutex>
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/exporters/memory/in_memory_span_data.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk_config.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace memory
{
const size_t MAX_BUFFER_SIZE = 100;

/**
 * A in memory exporter that switches a flag once a valid recordable was received
 * and keeps track of all received spans in memory.
 */
class InMemorySpanExporter final : public opentelemetry::sdk::trace::SpanExporter
{
public:
  /**
   * @param buffer_size an optional value that sets the size of the InMemorySpanData
   */
  InMemorySpanExporter(size_t buffer_size = MAX_BUFFER_SIZE)
      : data_(new InMemorySpanData(buffer_size))
  {}

  /**
   * @return Returns a unique pointer to an empty recordable object
   */
  std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<sdk::trace::Recordable>(new sdk::trace::SpanData());
  }

  /**
   * @param recordables a required span containing unique pointers to the data
   * to add to the InMemorySpanData
   * @return Returns the result of the operation
   */
  sdk::common::ExportResult Export(
      const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &recordables) noexcept override
  {
    if (isShutdown())
    {
      OTEL_INTERNAL_LOG_ERROR("[In Memory Span Exporter] Exporting "
                              << recordables.size() << " span(s) failed, exporter is shutdown");
      return sdk::common::ExportResult::kFailure;
    }
    for (auto &recordable : recordables)
    {
      auto span = std::unique_ptr<sdk::trace::SpanData>(
          static_cast<sdk::trace::SpanData *>(recordable.release()));
      if (span != nullptr)
      {
        data_->Add(std::move(span));
      }
    }

    return sdk::common::ExportResult::kSuccess;
  }

  /**
   * @param timeout an optional value containing the timeout of the exporter
   * note: passing custom timeout values is not currently supported for this exporter
   * @return Returns the status of the operation
   */
  bool Shutdown(std::chrono::microseconds /* timeout */) noexcept override
  {
    const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
    is_shutdown_ = true;
    return true;
  }

  /**
   * @return Returns a shared pointer to this exporters InMemorySpanData
   */
  std::shared_ptr<InMemorySpanData> GetData() noexcept { return data_; }

private:
  std::shared_ptr<InMemorySpanData> data_;
  bool is_shutdown_ = false;
  mutable opentelemetry::common::SpinLockMutex lock_;
  const bool isShutdown() const noexcept
  {
    const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
    return is_shutdown_;
  }
};
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
