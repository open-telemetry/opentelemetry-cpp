// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <string>

#include "opentelemetry/exporters/otlp/otlp_environment.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_client_options.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_preferred_temporality.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

OtlpGrpcMetricExporterOptions::OtlpGrpcMetricExporterOptions()
{
  endpoint                    = GetOtlpDefaultGrpcMetricsEndpoint();
  use_ssl_credentials         = !GetOtlpDefaultGrpcMetricsIsInsecure(); /* negation intended. */
  ssl_credentials_cacert_path = GetOtlpDefaultMetricsSslCertificatePath();
  ssl_credentials_cacert_as_string = GetOtlpDefaultMetricsSslCertificateString();

#ifdef ENABLE_OTLP_GRPC_SSL_MTLS_PREVIEW
  ssl_client_key_path    = GetOtlpDefaultMetricsSslClientKeyPath();
  ssl_client_key_string  = GetOtlpDefaultMetricsSslClientKeyString();
  ssl_client_cert_path   = GetOtlpDefaultMetricsSslClientCertificatePath();
  ssl_client_cert_string = GetOtlpDefaultMetricsSslClientCertificateString();
#endif

  timeout                         = GetOtlpDefaultMetricsTimeout();
  metadata                        = GetOtlpDefaultMetricsHeaders();
  user_agent                      = GetOtlpDefaultUserAgent();
  compression                     = GetOtlpDefaultMetricsCompression();
  retry_policy_max_attempts       = GetOtlpDefaultMetricsRetryMaxAttempts();
  retry_policy_initial_backoff    = GetOtlpDefaultMetricsRetryInitialBackoff();
  retry_policy_max_backoff        = GetOtlpDefaultMetricsRetryMaxBackoff();
  retry_policy_backoff_multiplier = GetOtlpDefaultMetricsRetryBackoffMultiplier();
}

OtlpGrpcMetricExporterOptions::OtlpGrpcMetricExporterOptions(void *)
    : OtlpGrpcClientOptions(nullptr)
{}

OtlpGrpcMetricExporterOptions::OtlpGrpcMetricExporterOptions(
    const OtlpGrpcClientOptions &client_options)
    : OtlpGrpcClientOptions(client_options)
{
  std::chrono::system_clock::duration signal_timeout;
  if (GetOtlpDefaultMetricsTimeoutOverride(signal_timeout))
  {
    timeout = signal_timeout;
  }
  metadata = GetOtlpDefaultMetricsHeaders();
}

OtlpGrpcMetricExporterOptions::~OtlpGrpcMetricExporterOptions() {}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
