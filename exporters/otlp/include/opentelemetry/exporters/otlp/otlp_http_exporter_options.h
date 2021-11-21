// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/otlp/otlp_environment.h"

#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Struct to hold OTLP http exporter options.
 */
struct OtlpHttpExporterOptions
{
  // The endpoint to export to. By default
  // @see
  // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/protocol/otlp.md
  // @see https://github.com/open-telemetry/opentelemetry-collector/tree/main/receiver/otlpreceiver
  std::string url;

  // By default, post json data
  HttpRequestContentType content_type;

  // If convert bytes into hex. By default, we will convert all bytes but id into base64
  // This option is ignored if content_type is not kJson
  JsonBytesMappingKind json_bytes_mapping;

  // If using the json name of protobuf field to set the key of json. By default, we will use the
  // field name just like proto files.
  bool use_json_name;

  // Whether to print the status of the exporter in the console
  bool console_debug;

  // TODO: Enable/disable to verify SSL certificate
  std::chrono::system_clock::duration timeout;

  // Additional HTTP headers
  OtlpHeaders http_headers;
};

inline OtlpHttpExporterOptions GetDefaultOtlpHttpExporterOptions()
{
  return OtlpHttpExporterOptions{GetOtlpDefaultHttpEndpoint(),
                                 HttpRequestContentType::kJson,
                                 JsonBytesMappingKind::kHexId,
                                 false,
                                 false,
                                 GetOtlpDefaultTimeout(),
                                 GetOtlpDefaultHeaders()};
}

inline OtlpHttpExporterOptions GetDefaultOtlpHttpLogExporterOptions()
{
  return OtlpHttpExporterOptions{GetOtlpDefaultHttpLogEndpoint(),
                                 HttpRequestContentType::kJson,
                                 JsonBytesMappingKind::kHexId,
                                 false,
                                 false,
                                 GetOtlpDefaultLogTimeout(),
                                 GetOtlpDefaultLogHeaders()};
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE