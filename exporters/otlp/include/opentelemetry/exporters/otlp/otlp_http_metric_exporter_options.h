// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/otlp/otlp_environment.h"
#include "opentelemetry/exporters/otlp/otlp_http.h"
#include "opentelemetry/sdk/metrics/instruments.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <string>

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
  std::string url = GetOtlpDefaultMetricsEndpoint();

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
  std::chrono::system_clock::duration timeout = GetOtlpDefaultMetricsTimeout();

  // Additional HTTP headers
  OtlpHeaders http_headers = GetOtlpDefaultMetricsHeaders();

  // Preferred Aggregation Temporality
  sdk::metrics::AggregationTemporality aggregation_temporality =
      sdk::metrics::AggregationTemporality::kCumulative;

#ifdef ENABLE_ASYNC_EXPORT
  // Concurrent requests
  // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/protocol/otlp.md#otlpgrpc-concurrent-requests
  std::size_t max_concurrent_requests = 64;

  // Requests per connections
  std::size_t max_requests_per_connection = 8;
#endif
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
