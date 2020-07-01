#pragma once

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/metrics/meter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace meter
{
/**
 * Creates new Meter instances.
 */
class MeterProvider
{
public:
  virtual ~MeterProvider() = default;
  MeterProvider() = default;
  /**
   * Gets or creates a named Meter instance.
   *
   * Optionally a version can be passed to create a named and versioned Meter
   * instance.
   */
   virtual nostd::shared_ptr<Meter> GetMeter(nostd::string_view library_name,
                                              nostd::string_view library_version = "") = 0;
};
}  // namespace meter
OPENTELEMETRY_END_NAMESPACE
