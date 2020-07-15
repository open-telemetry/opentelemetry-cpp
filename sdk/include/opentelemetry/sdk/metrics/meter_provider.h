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
   * Initialize a new tracer provider with a specified sampler
   * @param processor The span processor for this tracer provider. This must
   * not be a nullptr.
   * @param sampler The sampler for this tracer provider. This must
   * not be a nullptr.
   */
  explicit MeterProvider() noexcept;

  opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter> GetMeter(
      nostd::string_view library_name,
      nostd::string_view library_version = "") noexcept override;

  /**
   * Set the span processor associated with this tracer provider.
   * @param processor The new span processor for this tracer provider. This
   * must not be a nullptr.
   */
  void SetController(std::shared_ptr<Controller> controller) noexcept;

  /**
   * Obtain the span processor associated with this tracer provider.
   * @return The span processor for this tracer provider.
   */
  std::shared_ptr<Controller> GetController() const noexcept;

private:
  opentelemetry::sdk::AtomicSharedPtr<Controller> controller_;
  std::shared_ptr<opentelemetry::metrics::Meter> meter_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
