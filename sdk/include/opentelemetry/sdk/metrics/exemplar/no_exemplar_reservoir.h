// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>

#include "opentelemetry/sdk/metrics/exemplar/filter.h"
#include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
class SystemTimestamp;
}  // namespace common

namespace context
{
class Context;
}  // namespace context

namespace sdk
{
namespace metrics
{

class OPENTELEMETRY_SDK_METRICS_EXPORT ExemplarData;

class OPENTELEMETRY_SDK_METRICS_EXPORT NoExemplarReservoir final : public ExemplarReservoir
{

public:
  void OfferMeasurement(
      int64_t /* value */,
      const MetricAttributes & /* attributes */,
      const opentelemetry::context::Context & /* context */,
      const opentelemetry::common::SystemTimestamp & /* timestamp */) noexcept override
  {
    // Stores nothing
  }

  void OfferMeasurement(
      double /* value */,
      const MetricAttributes & /* attributes */,
      const opentelemetry::context::Context & /* context */,
      const opentelemetry::common::SystemTimestamp & /* timestamp */) noexcept override
  {
    // Stores nothing.
  }

  std::vector<std::shared_ptr<ExemplarData>> CollectAndReset(
      const MetricAttributes & /* pointAttributes */) noexcept override
  {
    return std::vector<std::shared_ptr<ExemplarData>>{};
  }

  explicit NoExemplarReservoir() = default;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
