#pragma once

#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/metrics/meter.h"

#include <memory>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class MeterProvider final : public opentelemetry::metrics::MeterProvider
{
public:
  /**
   * Initialize a new meter provider
   */
  explicit MeterProvider(std::string library_name = "", std::string library_version = "") noexcept;

  opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter> GetMeter(
      nostd::string_view library_name,
      nostd::string_view library_version = "") noexcept override;

private:
  std::shared_ptr<opentelemetry::metrics::Meter> meter_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
