// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_options.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

OtlpHttpMetricExporterOptions::OtlpHttpMetricExporterOptions()
{
  url                     = GetOtlpDefaultMetricsEndpoint();
  content_type            = HttpRequestContentType::kJson;
  json_bytes_mapping      = JsonBytesMappingKind::kHexId;
  use_json_name           = false;
  console_debug           = false;
  timeout                 = GetOtlpDefaultMetricsTimeout();
  http_headers            = GetOtlpDefaultMetricsHeaders();
  aggregation_temporality = PreferredAggregationTemporality::kCumulative;

#ifdef ENABLE_ASYNC_EXPORT
  max_concurrent_requests     = 64;
  max_requests_per_connection = 8;
#endif

#ifdef ENABLE_OTLP_HTTP_SSL_PREVIEW
  ssl_insecure_skip_verify = false;
  ssl_ca_cert_path         = GetOtlpDefaultMetricsSslCertificatePath();
  ssl_ca_cert_string       = GetOtlpDefaultMetricsSslCertificateString();
  ssl_client_key_path      = GetOtlpDefaultMetricsSslClientKeyPath();
  ssl_client_key_string    = GetOtlpDefaultMetricsSslClientKeyString();
  ssl_client_cert_path     = GetOtlpDefaultMetricsSslClientCertificatePath();
  ssl_client_cert_string   = GetOtlpDefaultMetricsSslClientCertificateString();
#endif /* ENABLE_OTLP_HTTP_SSL_PREVIEW */

#ifdef ENABLE_OTLP_HTTP_SSL_TLS_PREVIEW
  ssl_min_tls      = GetOtlpDefaultMetricsSslTlsMinVersion();
  ssl_max_tls      = GetOtlpDefaultMetricsSslTlsMaxVersion();
  ssl_cipher       = GetOtlpDefaultMetricsSslTlsCipher();
  ssl_cipher_suite = GetOtlpDefaultMetricsSslTlsCipherSuite();
#endif /* ENABLE_OTLP_HTTP_SSL_TLS_PREVIEW */
}

OtlpHttpMetricExporterOptions::~OtlpHttpMetricExporterOptions() {}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
