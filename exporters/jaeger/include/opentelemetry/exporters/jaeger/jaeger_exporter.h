// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

OPENTELEMETRY_BEGIN_NANESPACE
namespace exporter
{
namespace jaeger
{
enum class TransportFormat
{
    THRIFT,
};

/**
 * Struct to hold Jaeger exporter options.
 */
struct JaegerExporterOptions
{

};

namespace trace_sdk = opentelemetry::sdk::trace;

class JaegerExporter final : public trace_sdk::SpanExporter
{
public:
  /**
   * Create a JaegerExporter using all default options.
   */
  JaegerExporter();

  /**
   * Export a batch of spans.
   * @param spans a span of unique pointers to span recordables.
   */
  trace_sdk::ExportResult Export(
      const nostd::span<std::unique_ptr<trace_sdk::Recordable>> &spans) noexcept override;

  /**
   * Shutdown the exporter.
   * @param timeout an option timeout, default to max.
   */
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max() noexcept override;
      )
  {
      return true;
  }

private:
  void InitializeLocalEndpoint();

private:
  // The configuration options associated with this exporter.
  bool is_shutdown_ = false;
  JaegerExporterOptions options_;

};
}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
