#include "opentelemetry/sdk/metrics/meter_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
MeterProvider::MeterProvider(std::shared_ptr<Controller> controller) noexcept
    : controller_{controller}, meter_(new Meter)
{}

opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter> MeterProvider::GetMeter(
    nostd::string_view library_name,
    nostd::string_view library_version) noexcept
{
  return opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter>(meter_);
}

void MeterProvider::SetController(std::shared_ptr<Controller> controller) noexcept
{
  controller_.store(controller);

  // TODO: Set controller in Meter once the stub has been replaced
}

std::shared_ptr<Controller> MeterProvider::GetController() const noexcept
{
  return controller_.load();
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
