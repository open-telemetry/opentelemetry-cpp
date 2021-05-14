// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include "opentelemetry/exporters/memory/in_memory_span_data.h"
#include "opentelemetry/sdk/trace/exporter.h"

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
      : data_(new opentelemetry::exporter::memory::InMemorySpanData(buffer_size))
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
    for (auto &recordable : recordables)
    {
      auto span = std::unique_ptr<sdk::trace::SpanData>(
          dynamic_cast<sdk::trace::SpanData *>(recordable.release()));
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
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override
  {
    return true;
  };

  /**
   * @return Returns a shared pointer to this exporters InMemorySpanData
   */
  std::shared_ptr<opentelemetry::exporter::memory::InMemorySpanData> GetData() noexcept
  {
    return data_;
  }

private:
  std::shared_ptr<opentelemetry::exporter::memory::InMemorySpanData> data_;
};
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
