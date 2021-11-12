// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/otlp/otlp_environment.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Struct to hold OTLP exporter options.
 */
struct OtlpGrpcExporterOptions
{
  // The endpoint to export to. By default the OpenTelemetry Collector's default endpoint.
  std::string endpoint = GetOtlpDefaultGrpcEndpoint();
  // By default when false, uses grpc::InsecureChannelCredentials(); If true,
  // uses ssl_credentials_cacert_path if non-empty, else uses ssl_credentials_cacert_as_string
  bool use_ssl_credentials = GetOtlpDefaultIsSslEnable();
  // ssl_credentials_cacert_path specifies path to .pem file to be used for SSL encryption.
  std::string ssl_credentials_cacert_path = GetOtlpDefaultSslCertificatePath();
  // ssl_credentials_cacert_as_string in-memory string representation of .pem file to be used for
  // SSL encryption.
  std::string ssl_credentials_cacert_as_string = GetOtlpDefaultSslCertificateString();
  // Timeout for grpc deadline
  std::chrono::system_clock::duration timeout = GetOtlpDefaultTimeout();
  // Additional HTTP headers
  OtlpHeaders metadata = GetOtlpDefaultHeaders();
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
