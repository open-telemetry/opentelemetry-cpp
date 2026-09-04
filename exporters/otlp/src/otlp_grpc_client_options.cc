// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_grpc_client_options.h"
#include "opentelemetry/exporters/otlp/otlp_environment.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

OtlpGrpcClientOptions::OtlpGrpcClientOptions()
    : endpoint(GetOtlpDefaultGrpcClientEndpoint()),
      use_ssl_credentials(!GetOtlpDefaultGrpcClientIsInsecure()),
      ssl_credentials_cacert_path(GetOtlpDefaultGrpcClientSslCertificatePath()),
      ssl_credentials_cacert_as_string(GetOtlpDefaultGrpcClientSslCertificateString()),
      timeout(GetOtlpDefaultGrpcClientTimeout()),
      metadata(GetOtlpDefaultGrpcClientHeaders()),
      user_agent(GetOtlpDefaultUserAgent()),
      compression(GetOtlpDefaultGrpcClientCompression()),
      retry_policy_max_attempts(GetOtlpDefaultGrpcClientRetryMaxAttempts()),
      retry_policy_initial_backoff(GetOtlpDefaultGrpcClientRetryInitialBackoff()),
      retry_policy_max_backoff(GetOtlpDefaultGrpcClientRetryMaxBackoff()),
      retry_policy_backoff_multiplier(GetOtlpDefaultGrpcClientRetryBackoffMultiplier())
{
#ifdef ENABLE_OTLP_GRPC_SSL_MTLS_PREVIEW
  ssl_client_key_path    = GetOtlpDefaultGrpcClientSslClientKeyPath();
  ssl_client_key_string  = GetOtlpDefaultGrpcClientSslClientKeyString();
  ssl_client_cert_path   = GetOtlpDefaultGrpcClientSslClientCertificatePath();
  ssl_client_cert_string = GetOtlpDefaultGrpcClientSslClientCertificateString();
#endif
}

OtlpGrpcClientOptions::OtlpGrpcClientOptions(void *) {}

OtlpGrpcClientOptions::~OtlpGrpcClientOptions() {}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
