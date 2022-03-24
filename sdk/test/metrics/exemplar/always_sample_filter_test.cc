// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/always_sample_filter.h"
#  include <gtest/gtest.h>

using namespace opentelemetry::sdk::metrics;

TEST(AlwaysSampleFilter, SampleMeasurement)
{
  auto filter = opentelemetry::sdk::metrics::AlwaysSampleFilter::GetAlwaysSampleFilter();
  ASSERT_TRUE(
      filter->ShouldSampleMeasurement(1.0, MetricAttributes{}, opentelemetry::context::Context{}));
  ASSERT_TRUE(
      filter->ShouldSampleMeasurement(1l, MetricAttributes{}, opentelemetry::context::Context{}));
}

#endif
