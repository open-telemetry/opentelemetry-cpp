// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_metrics_utils.h"

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "opentelemetry/proto/metrics/v1/metrics.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#ifndef ENABLE_METRICS_PREVIEW

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

void OtlpMetricsUtils::PopulateRequest(
    const opentelemetry::sdk::metrics::ResourceMetrics &data,
    proto::collector::metrics::v1::ExportMetricsServiceRequest *request) noexcept
{
  if (request == nullptr || data.resource_ == nullptr)
  {
    return;
  }
  auto resource_metrics = request->add_resource_metrics();
  *resource_metrics->mutable_resource() =
      for (auto &instrum_metrics : data.instrumentation_info_metric_data_)
  {
    if (instrum_metrics.instrumentation_library_ == nullptr)
    {
      continue;
    }
    for (auto &metric_data : instrum_metrics.metric_data_)
    {
    }
  }
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
