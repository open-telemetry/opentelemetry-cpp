// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_grpc_push_metric_builder.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

static OtlpGrpcPushMetricBuilder singleton;

void OtlpGrpcPushMetricBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  registry->SetOtlpGrpcPushMetricExporterBuilder(&singleton);
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> OtlpGrpcPushMetricBuilder::Build(
    const opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterConfiguration * /* model */)
    const
{
  // FIXME, use model
  OtlpGrpcMetricExporterOptions options;
  return OtlpGrpcMetricExporterFactory::Create(options);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
