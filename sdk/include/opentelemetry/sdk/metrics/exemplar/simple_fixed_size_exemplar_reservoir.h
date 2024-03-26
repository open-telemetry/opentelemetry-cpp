// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <memory>
#  include <vector>

#  include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/exemplar/fixed_size_exemplar_reservoir.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir_cell_selector.h"
#  include "opentelemetry/version.h"

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
  static const size_t kDefaultSimpleReservoirSize = 1;

  static std::shared_ptr<ReservoirCellSelector> GetSimpleFixedSizeCellSelector(
      size_t size = kDefaultSimpleReservoirSize)
  {
    return std::shared_ptr<ReservoirCellSelector>{new SimpleFixedSizeCellSelector{size}};
  }

  SimpleFixedSizeExemplarReservoir(size_t size,
                                   std::shared_ptr<ReservoirCellSelector> reservoir_cell_selector,
                                   MapAndResetCellType map_and_reset_cell)
      : FixedSizeExemplarReservoir(size, reservoir_cell_selector, map_and_reset_cell)
  {}

  class SimpleFixedSizeCellSelector : public ReservoirCellSelector
  {
  public:
    SimpleFixedSizeCellSelector(size_t size) : size_(size) {}

    int ReservoirCellIndexFor(const std::vector<ReservoirCell> &cells,
                              int64_t value,
                              const MetricAttributes &attributes,
                              const opentelemetry::context::Context &context) override
    {
      return ReservoirCellIndexFor(cells, static_cast<double>(value), attributes, context);
    }

    int ReservoirCellIndexFor(const std::vector<ReservoirCell> & /* cells */,
                              double /* value */,
                              const MetricAttributes & /* attributes */,
                              const opentelemetry::context::Context & /* context */) override
    {
      size_t measurement_num = measurements_seen_++;
      size_t index           = static_cast<size_t>(-1);

      if (measurement_num < size_)
      {
        index = measurement_num;
      }
      else
      {
        // return random index between 0 and measurement_num
        index = rand() % (measurement_num + 1);
      }

      return static_cast<int>(index);
    }

    void reset() override {}

  private:
    size_t measurements_seen_ = 0;
    size_t size_;
  };  // class SimpleFixedSizeCellSelector

};  // class SimpleFixedSizeExemplarReservoir

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW