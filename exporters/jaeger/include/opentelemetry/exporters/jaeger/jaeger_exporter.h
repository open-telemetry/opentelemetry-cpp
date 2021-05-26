// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

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
  kThriftUdpCompact,
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
  TransportFormat transport_format = TransportFormat::kThriftUdpCompact;
};

namespace trace_sdk  = opentelemetry::sdk::trace;
namespace sdk_common = opentelemetry::sdk::common;

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
  sdk_common::ExportResult Export(
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
