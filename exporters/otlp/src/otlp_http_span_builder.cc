// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <memory>
#include <string>

#include "opentelemetry/exporters/otlp/otlp_http.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_http_span_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_configuration.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

static OtlpHttpSpanBuilder singleton;

void OtlpHttpSpanBuilder::Register(opentelemetry::sdk::init::Registry *registry)
{
  registry->SetOtlpHttpSpanBuilder(&singleton);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpHttpSpanBuilder::Build(
    const opentelemetry::sdk::configuration::OtlpHttpSpanExporterConfiguration *model) const
{
  OtlpHttpExporterOptions options(nullptr);
  options.url          = model->endpoint;
  options.content_type = GetOtlpHttpProtocolFromString(model->protocol);
  options.timeout      = std::chrono::duration_cast<std::chrono::system_clock::duration>(
      std::chrono::seconds{model->timeout});
  // options.http_headers = model->xxx;
  options.ssl_ca_cert_path     = model->certificate;
  options.ssl_client_key_path  = model->client_key;
  options.ssl_client_cert_path = model->client_certificate;
  return OtlpHttpExporterFactory::Create(options);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
