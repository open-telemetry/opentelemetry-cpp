// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/exemplar/histogram_exemplar_reservoir.h"
#include <gtest/gtest.h>
#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class HistogramExemplarReservoirTestPeer : public ::testing::Test
{
public:
};

TEST_F(HistogramExemplarReservoirTestPeer, OfferMeasurement)
{
  std::vector<double> boundaries{1, 5.0, 10, 15, 20};
  auto histogram_exemplar_reservoir = ExemplarReservoir::GetHistogramExemplarReservoir(
      boundaries.size(), HistogramExemplarReservoir::GetHistogramCellSelector(boundaries), nullptr);
  histogram_exemplar_reservoir->OfferMeasurement(
      1.0, MetricAttributes{}, opentelemetry::context::Context{}, std::chrono::system_clock::now());
  histogram_exemplar_reservoir->OfferMeasurement((int64_t)1, MetricAttributes{},
                                                 opentelemetry::context::Context{},
                                                 std::chrono::system_clock::now());
  auto exemplar_data = histogram_exemplar_reservoir->CollectAndReset(MetricAttributes{});
  ASSERT_TRUE(exemplar_data.empty());
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
