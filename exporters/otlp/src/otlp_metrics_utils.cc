// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_metrics_utils.h"
#include "opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h"

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

  // populate resource
  auto resource_metrics = request->add_resource_metrics();
  proto::resource::v1::Resource proto;
  OtlpPopulateAttributeUtils::PopulateAttribute(&proto, *(data.resource_));
  *resource_metrics->mutable_resource() = proto;

  for (auto &instrumentation_metrics : data.instrumentation_info_metric_data_)
  {
    if (instrumentation_metrics.instrumentation_library_ == nullptr)
    {
      continue;
    }
    auto instrumentation_lib_metrics = resource_metrics->add_instrumentation_library_metrics();
    proto::common::v1::InstrumentationLibrary instrumentation_library;
    instrumentation_library.set_name(instrumentation_metrics.instrumentation_library_->GetName());
    instrumentation_library.set_version(
        instrumentation_metrics.instrumentation_library_->GetVersion());
    *instrumentation_lib_metrics->mutable_instrumentation_library() = instrumentation_library;

    for (auto &metric_data : instrumentation_metrics.metric_data_)
    {
      proto::metrics::v1::Metric metric;
      metric.set_name(metric_data.instrument_descriptor.name_);
      metric.set_description(metric_data.instrument_descriptor.description_);
      metric.set_unit(metric_data.instrument_descriptor.unit_);
    }
  }
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
