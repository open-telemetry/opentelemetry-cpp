// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/_metrics/meter_provider.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/_metrics/meter.h"

#  include <memory>
#  include <string>

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
#endif
