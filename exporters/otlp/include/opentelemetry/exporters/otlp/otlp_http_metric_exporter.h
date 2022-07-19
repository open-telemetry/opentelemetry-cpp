// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/sdk/metrics/metric_exporter.h"

#  include "opentelemetry/exporters/otlp/otlp_http_client.h"

#  include "opentelemetry/exporters/otlp/otlp_environment.h"

#  include <chrono>
#  include <cstddef>
#  include <memory>
#  include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Struct to hold OTLP exporter options.
 */
struct OtlpHttpMetricExporterOptions
{
  // The endpoint to export to. By default
  // @see
  // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/protocol/otlp.md
  // @see https://github.com/open-telemetry/opentelemetry-collector/tree/main/receiver/otlpreceiver
  std::string url = GetOtlpDefaultHttpMetricEndpoint();

  // By default, post json data
  HttpRequestContentType content_type = HttpRequestContentType::kJson;

  // If convert bytes into hex. By default, we will convert all bytes but id into base64
  // This option is ignored if content_type is not kJson
  JsonBytesMappingKind json_bytes_mapping = JsonBytesMappingKind::kHexId;

  // If using the json name of protobuf field to set the key of json. By default, we will use the
  // field name just like proto files.
  bool use_json_name = false;

  // Whether to print the status of the exporter in the console
  bool console_debug = false;

  // TODO: Enable/disable to verify SSL certificate
  std::chrono::system_clock::duration timeout = GetOtlpDefaultMetricTimeout();

  // Additional HTTP headers
  OtlpHeaders http_headers = GetOtlpDefaultMetricHeaders();

#  ifdef ENABLE_ASYNC_EXPORT
  // Concurrent requests
  // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/protocol/otlp.md#otlpgrpc-concurrent-requests
  std::size_t max_concurrent_requests = 64;

  // Requests per connections
  std::size_t max_requests_per_connection = 8;
#  endif
};

/**
 * The OTLP exporter exports metrics data in OpenTelemetry Protocol (OTLP) format in HTTP.
 */
class OtlpHttpMetricExporter final : public opentelemetry::sdk::metrics::MetricExporter
{
public:
  /**
   * Create an OtlpHttpMetricExporter with default exporter options.
   */
  OtlpHttpMetricExporter();

  /**
   * Create an OtlpHttpMetricExporter with user specified options.
   * @param options An object containing the user's configuration options.
   */
  OtlpHttpMetricExporter(const OtlpHttpMetricExporterOptions &options);

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::sdk::metrics::ResourceMetrics &data) noexcept override;

  /**
   * Force flush the exporter.
   */
  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

  bool Shutdown(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

private:
  // Configuration options for the exporter
  const OtlpHttpMetricExporterOptions options_;

  // Object that stores the HTTP sessions that have been created
  std::unique_ptr<OtlpHttpClient> http_client_;
  // For testing
  friend class OtlpHttpMetricExporterTestPeer;

  /**
   * Create an OtlpHttpMetricExporter using the specified http client.
   * Only tests can call this constructor directly.
   * @param http_client the http client to be used for exporting
   */
  OtlpHttpMetricExporter(std::unique_ptr<OtlpHttpClient> http_client);
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
