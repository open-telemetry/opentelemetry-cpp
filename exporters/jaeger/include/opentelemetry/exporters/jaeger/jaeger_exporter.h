// Copyright 2021, OpenTelemetry Authors
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

#include <opentelemetry/sdk/trace/exporter.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{
enum class TransportFormat
{
  kThriftUdp,
  kThriftHttp,
  kProtobufGrpc,
};

class ThriftSender;

/**
 * Struct to hold Jaeger exporter options.
 */
struct JaegerExporterOptions
{
  // The endpoint to export to.
  std::string server_addr          = "localhost";
  uint16_t server_port             = 6831;
  TransportFormat transport_format = TransportFormat::kThriftUdp;
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
   * Create a JaegerExporter using the given options.
   */
  explicit JaegerExporter(const JaegerExporterOptions &options);

  /**
   * Create a span recordable.
   * @return a new initialized Recordable object.
   */
  std::unique_ptr<trace_sdk::Recordable> MakeRecordable() noexcept override;

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
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override
  {
    return true;
  }

private:
  void InitializeEndpoint();

private:
  // The configuration options associated with this exporter.
  bool is_shutdown_ = false;
  JaegerExporterOptions options_;
  std::unique_ptr<ThriftSender> sender_;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
