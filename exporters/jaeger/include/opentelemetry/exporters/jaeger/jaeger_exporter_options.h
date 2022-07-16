// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

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
struct JaegerExporterOptions
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
