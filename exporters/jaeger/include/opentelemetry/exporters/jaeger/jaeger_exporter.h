// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <opentelemetry/common/spin_lock_mutex.h>
#include <opentelemetry/ext/http/client/http_client.h>
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
  TransportFormat transport_format = TransportFormat::kThriftUdpCompact;
  std::string endpoint             = "localhost";
  uint16_t server_port             = 6831;
  // Only applicable when using kThriftHttp transport.
  ext::http::client::Headers headers;
};

class JaegerExporter final : public opentelemetry::sdk::trace::SpanExporter
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
  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override;

  /**
   * Export a batch of spans.
   * @param spans a span of unique pointers to span recordables.
   */
  opentelemetry::sdk::common::ExportResult Export(
      const nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> &spans) noexcept
      override;

  /**
   * Shutdown the exporter.
   * @param timeout an option timeout, default to max.
   */
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

private:
  void InitializeEndpoint();

private:
  // The configuration options associated with this exporter.
  bool is_shutdown_ = false;
  JaegerExporterOptions options_;
  std::unique_ptr<ThriftSender> sender_;
  mutable opentelemetry::common::SpinLockMutex lock_;
  bool isShutdown() const noexcept;
  // For testing
  friend class JaegerExporterTestPeer;
  /**
   * Create an JaegerExporter using the specified thrift sender.
   * Only tests can call this constructor directly.
   * @param sender the thrift sender to be used for exporting
   */
  JaegerExporter(std::unique_ptr<ThriftSender> sender);
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
