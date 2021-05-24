// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/meter_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
MeterProvider::MeterProvider(std::string library_name, std::string library_version) noexcept
    : meter_(new Meter(library_name, library_version))
{}

opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter> MeterProvider::GetMeter(
    nostd::string_view library_name,
    nostd::string_view library_version) noexcept
{
  return opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter>(meter_);
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
