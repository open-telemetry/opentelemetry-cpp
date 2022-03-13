// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/context/context.h"
#ifndef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include "opentelemetry/sdk/metrics/examplar/never_sample_filter.h"

using namespace opentelemetry::sdk::metrics;

TEST(NeverSampleFilter, SampleMeasurement)
{
  auto filter = opentelemetry::sdk::metrics::NeverSampleFilter::GetNeverSampleFilter();
  ASSERT_FALSE(
      filter->shouldSampleMeasurement(1.0, MetricAttributes{}, opentelemetry::context::Context{}));
  ASSERT_FALSE(
      filter->shouldSampleMeasurement(1l, MetricAttributes{}, opentelemetry::context::Context{}));
}

#endif
