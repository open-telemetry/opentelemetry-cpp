// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <gtest/gtest.h>
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"

using namespace opentelemetry::sdk::metrics;

TEST(NeverSampleFilter, SampleMeasurement)
{
  auto filter = opentelemetry::sdk::metrics::ExemplarFilter::GetNeverSampleFilter();
  ASSERT_FALSE(
      filter->ShouldSampleMeasurement(1.0, MetricAttributes{}, opentelemetry::context::Context{}));
  ASSERT_FALSE(filter->ShouldSampleMeasurement(static_cast<int64_t>(1), MetricAttributes{},
                                               opentelemetry::context::Context{}));
}

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW
