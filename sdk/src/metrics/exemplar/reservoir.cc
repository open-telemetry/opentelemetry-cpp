// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/exemplar/reservoir.h"

#include <memory>
#include <vector>

#include "opentelemetry/sdk/metrics/exemplar/filtered_exemplar_reservoir.h"
#include "opentelemetry/sdk/metrics/exemplar/fixed_size_exemplar_reservoir.h"
#include "opentelemetry/sdk/metrics/exemplar/histogram_exemplar_reservoir.h"
#include "opentelemetry/sdk/metrics/exemplar/no_exemplar_reservoir.h"
#include "opentelemetry/sdk/metrics/exemplar/reservoir_cell.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

std::shared_ptr<ExemplarReservoir> ExemplarReservoir::GetFilteredExemplarReservoir(
    std::shared_ptr<ExemplarFilter> filter,
    std::shared_ptr<ExemplarReservoir> reservoir)
{
  return std::shared_ptr<ExemplarReservoir>{new FilteredExemplarReservoir{filter, reservoir}};
}

std::shared_ptr<ExemplarReservoir> ExemplarReservoir::GetHistogramExemplarReservoir(
    size_t size,
    std::shared_ptr<ReservoirCellSelector> reservoir_cell_selector,
    std::shared_ptr<ExemplarData> (ReservoirCell::*map_and_reset_cell)(
        const common::OrderedAttributeMap &attributes))
{
  return std::shared_ptr<ExemplarReservoir>{
      new HistogramExemplarReservoir{size, reservoir_cell_selector, map_and_reset_cell}};
}

std::shared_ptr<ExemplarReservoir> ExemplarReservoir::GetNoExemplarReservoir()
{
  return std::shared_ptr<ExemplarReservoir>{new NoExemplarReservoir{}};
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
