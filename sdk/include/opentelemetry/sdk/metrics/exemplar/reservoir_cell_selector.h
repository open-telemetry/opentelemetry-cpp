// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/metrics/exemplar/reservoir_cell.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class ReservoirCellSelector
{

public:
  virtual ~ReservoirCellSelector() = default;

  /** Determine the index of the {@code cells} to record the measurement to. */
  virtual int ReservoirCellIndexFor(const std::vector<ReservoirCell> &cells,
                                    int64_t value,
                                    const MetricAttributes &attributes,
                                    const opentelemetry::context::Context &context) = 0;

  /** Determine the index of the {@code cells} to record the measurement to. */
  virtual int ReservoirCellIndexFor(const std::vector<ReservoirCell> &cells,
                                    double value,
                                    const MetricAttributes &attributes,
                                    const opentelemetry::context::Context &context) = 0;

  /** Called when {@link FixedSizeExemplarReservoir#CollectAndReset(Attributes)}. */
  virtual void reset() = 0;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
