#pragma once

#include <map>
#include <memory>
#include <string>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/metrics/controller.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/metrics/meter_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class MeterProvider final : public opentelemetry::metrics::MeterProvider
{
public:
  /**
   * Initialize a new meter provider with a specified controller
   * @param controller The controller for this meter provider. This must
   * not be a nullptr.
   */
  explicit MeterProvider(std::shared_ptr<Controller> controller) noexcept;

  opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter> GetMeter(
      nostd::string_view library_name,
      nostd::string_view library_version = "") noexcept override;

  /**
   * Set the controller associated with this meter provider.
   * @param controller The new controller for this meter provider. This
   * must not be a nullptr.
   */
  void SetController(std::shared_ptr<Controller> controller) noexcept;

  /**
   * Obtain the controller associated with this meter provider.
   * @return The span processor for this meter provider.
   */
  std::shared_ptr<Controller> GetController() const noexcept;

private:
  opentelemetry::sdk::AtomicSharedPtr<Controller> controller_;
  std::shared_ptr<opentelemetry::metrics::Meter> meter_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
