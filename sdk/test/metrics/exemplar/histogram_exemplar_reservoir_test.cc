// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/histogram_exemplar_reservoir.h"
#  include <gtest/gtest.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class HistogramExemplarReservoirTestPeer : public ::testing::Test
{
public:
};

TEST_F(HistogramExemplarReservoirTestPeer, OfferMeasurement) {}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
