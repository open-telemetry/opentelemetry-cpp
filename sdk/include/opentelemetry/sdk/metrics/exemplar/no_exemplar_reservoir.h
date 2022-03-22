// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <vector>
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/common/attribute_utils.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class NoExemplarReservoir final : public ExemplarReservoir
{

public:
  static nostd::shared_ptr<ExemplarReservoir> GetNoExemplarReservoir()
  {
    return nostd::shared_ptr<ExemplarReservoir>{new NoExemplarReservoir{}};
  }

  void OfferMeasurement(long value,
                        const MetricAttributes &attributes,
                        const opentelemetry::context::Context &context,
                        const opentelemetry::common::SystemTimestamp &timestamp) noexcept override
  {
    // Stores nothing
  }

  void OfferMeasurement(double value,
                        const MetricAttributes &attributes,
                        const opentelemetry::context::Context &context,
                        const opentelemetry::common::SystemTimestamp &timestamp) noexcept override
  {
    // Stores nothing.
  }

  std::vector<ExemplarData> CollectAndReset(
      const MetricAttributes &pointAttributes) noexcept override
  {
    return std::vector<ExemplarData>{};
  }

private:
  explicit NoExemplarReservoir() = default;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
