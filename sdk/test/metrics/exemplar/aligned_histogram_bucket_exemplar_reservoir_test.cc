// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <gtest/gtest.h>
#  include <stdint.h>
#  include <chrono>
#  include <cstdint>
#  include <memory>
#  include <string>
#  include <vector>

#  include "opentelemetry/context/context.h"
#  include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#  include "opentelemetry/sdk/metrics/exemplar/aligned_histogram_bucket_exemplar_reservoir.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir_cell.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
namespace
{

class AlignedHistogramBucketExemplarReservoirTestPeer : public ::testing::Test
{
public:
};

TEST_F(AlignedHistogramBucketExemplarReservoirTestPeer, OfferMeasurement)
{
  std::vector<double> boundaries{1, 5.0, 10, 15, 20};
  auto histogram_exemplar_reservoir = ExemplarReservoir::GetAlignedHistogramBucketExemplarReservoir(
      boundaries.size(),
      AlignedHistogramBucketExemplarReservoir::GetHistogramCellSelector(boundaries), nullptr);
  histogram_exemplar_reservoir->OfferMeasurement(1.0, MetricAttributes{},
                                                 opentelemetry::context::Context{});
  histogram_exemplar_reservoir->OfferMeasurement(static_cast<int64_t>(1), MetricAttributes{},
                                                 opentelemetry::context::Context{});
  auto exemplar_data = histogram_exemplar_reservoir->CollectAndReset(MetricAttributes{});
  ASSERT_TRUE(exemplar_data.empty());
}

TEST_F(AlignedHistogramBucketExemplarReservoirTestPeer, OfferMeasurementWithNonEmptyCollection)
{
  std::vector<double> boundaries{1, 5.0, 10, 15, 20};
  auto histogram_exemplar_reservoir = ExemplarReservoir::GetAlignedHistogramBucketExemplarReservoir(
      boundaries.size(),
      AlignedHistogramBucketExemplarReservoir::GetHistogramCellSelector(boundaries),
      &ReservoirCell::GetAndResetDouble);
  histogram_exemplar_reservoir->OfferMeasurement(1.0, MetricAttributes{},
                                                 opentelemetry::context::Context{});
  histogram_exemplar_reservoir->OfferMeasurement(static_cast<int64_t>(1), MetricAttributes{},
                                                 opentelemetry::context::Context{});
  auto exemplar_data = histogram_exemplar_reservoir->CollectAndReset(MetricAttributes{});
  ASSERT_TRUE(!exemplar_data.empty());
}

}  // namespace

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW
