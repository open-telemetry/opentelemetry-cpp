// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/_metrics/meter_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
namespace nostd       = opentelemetry::nostd;
namespace metrics_api = opentelemetry::metrics;

MeterProvider::MeterProvider(std::string library_name, std::string library_version) noexcept
    : meter_(new Meter(library_name, library_version))
{}

nostd::shared_ptr<metrics_api::Meter> MeterProvider::GetMeter(
    nostd::string_view library_name,
    nostd::string_view library_version) noexcept
{
  return nostd::shared_ptr<metrics_api::Meter>(meter_);
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
