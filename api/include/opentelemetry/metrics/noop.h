#pragma once
// Please refer to provider.h for documentation on how to obtain a Meter object.
//
// This file is part of the internal implementation of OpenTelemetry. Nothing in this file should be
// used directly. Please refer to meter.h for documentation on these interfaces.

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/version.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{
/**
 * No-op implementation of Instrument. This class should not be used directly.
 */
class NoopInstrument
{
public:
  NoopInstrument(const std::shared_ptr<Meter> &meter) noexcept : meter_{meter} {}


  Meter &meter() const noexcept { return *meter_; }

private:
  std::shared_ptr<Meter> meter_;
};

/**
 * No-op implementation of Meter.
 */
class NoopMeter final : public Meter, public std::enable_shared_from_this<NoopMeter>
{
public:
    NoopMeter() = default;

};

/**
 * No-op implementation of a MeterProvider.
 */
class NoopMeterProvider final : public opentelemetry::metrics::MeterProvider
{
public:
  NoopMeterProvider()
      : meter_{nostd::shared_ptr<opentelemetry::metrics::NoopMeter>(
            new opentelemetry::metrics::NoopMeter)}
  {}

  nostd::shared_ptr<opentelemetry::metrics::Meter> GetMeter(
      nostd::string_view library_name,
      nostd::string_view library_version) override
  {
    return meter_;
  }

private:
  nostd::shared_ptr<opentelemetry::metrics::Meter> meter_;
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
