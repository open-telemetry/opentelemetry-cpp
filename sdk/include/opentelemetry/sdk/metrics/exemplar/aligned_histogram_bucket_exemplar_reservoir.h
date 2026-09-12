// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <memory>
#  include <vector>

#  include "opentelemetry/common/key_value_iterable.h"
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/exemplar/fixed_size_exemplar_reservoir.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir_cell_selector.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
class Context;
}  // namespace context

namespace sdk
{
namespace metrics
{

class AlignedHistogramBucketExemplarReservoir : public FixedSizeExemplarReservoir
{

public:
  static std::shared_ptr<ReservoirCellSelector> GetHistogramCellSelector(
      const std::vector<double> &boundaries = std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0})
  {
    return std::shared_ptr<ReservoirCellSelector>{new HistogramCellSelector(boundaries)};
  }

  AlignedHistogramBucketExemplarReservoir(
      size_t size,
      const std::shared_ptr<ReservoirCellSelector> &reservoir_cell_selector,
      MapAndResetCellType map_and_reset_cell)
      : FixedSizeExemplarReservoir(size + 1, reservoir_cell_selector, map_and_reset_cell)
  {}

  class HistogramCellSelector : public ReservoirCellSelector
  {
  public:
    HistogramCellSelector(const std::vector<double> &boundaries) : boundaries_(boundaries) {}

    int ReservoirCellIndexFor(const std::vector<ReservoirCell> & /* cells */,
                              int64_t value,
                              const MetricAttributes & /* attributes */,
                              const opentelemetry::context::Context & /* context */) override
    {
      return FindCellIndex(static_cast<double>(value));
    }

    int ReservoirCellIndexFor(const std::vector<ReservoirCell> & /* cells */,
                              double value,
                              const MetricAttributes & /* attributes */,
                              const opentelemetry::context::Context & /* context */) override
    {
      return FindCellIndex(value);
    }

    int ReservoirCellIndexFor(const std::vector<ReservoirCell> & /* cells */,
                              int64_t value,
                              const opentelemetry::common::KeyValueIterable & /* attributes */,
                              const opentelemetry::context::Context & /* context */) override
    {
      return FindCellIndex(static_cast<double>(value));
    }

    int ReservoirCellIndexFor(const std::vector<ReservoirCell> & /* cells */,
                              double value,
                              const opentelemetry::common::KeyValueIterable & /* attributes */,
                              const opentelemetry::context::Context & /* context */) override
    {
      return FindCellIndex(value);
    }

  public:
    void reset() override
    {
      // Do nothing
    }

  private:
    int FindCellIndex(double value) const
    {
      size_t max_size = boundaries_.size();
      for (size_t i = 0; i < max_size; ++i)
      {
        if (value <= boundaries_[i])
        {
          return static_cast<int>(i);
        }
      }

      // The bucket at max_size is for values greater than the last boundary
      return static_cast<int>(max_size);
    }

    std::vector<double> boundaries_;
  };
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW
