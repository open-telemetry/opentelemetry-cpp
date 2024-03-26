// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>

#include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#include "opentelemetry/sdk/metrics/exemplar/fixed_size_exemplar_reservoir.h"
#include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#include "opentelemetry/sdk/metrics/exemplar/reservoir_cell_selector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
class OrderedAttributeMap;
}  // namespace common

namespace context
{
class Context;
}  // namespace context

namespace sdk
{
namespace metrics
{

class SimpleFixedSizeExemplarReservoir : public FixedSizeExemplarReservoir
{

public:
  SimpleFixedSizeExemplarReservoir(size_t size,
                                   std::shared_ptr<ReservoirCellSelector> reservoir_cell_selector,
                                   MapAndResetCellType map_and_reset_cell)
      : FixedSizeExemplarReservoir(size, reservoir_cell_selector, map_and_reset_cell)
  {}
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
