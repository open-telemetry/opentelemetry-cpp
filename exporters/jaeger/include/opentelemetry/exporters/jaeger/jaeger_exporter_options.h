// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <opentelemetry/common/macros.h>

#ifdef OPENTELEMETRY_NO_DEPRECATED_CODE
#  error "header <opentelemetry/exporters/jaeger/jaeger_exporter_options.h> is deprecated."
#endif

#include <opentelemetry/ext/http/client/http_client.h>

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

/**
 * Struct to hold Jaeger exporter options.
 */
struct OPENTELEMETRY_DEPRECATED JaegerExporterOptions
{
  TransportFormat transport_format = TransportFormat::kThriftUdpCompact;
  std::string endpoint             = "localhost";
  uint16_t server_port             = 6831;
  // Only applicable when using kThriftHttp transport.
  ext::http::client::Headers headers;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
