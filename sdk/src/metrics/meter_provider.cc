#include "opentelemetry/sdk/metrics/meter_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
MeterProvider::MeterProvider() noexcept
    :  meter_(new Meter)
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
