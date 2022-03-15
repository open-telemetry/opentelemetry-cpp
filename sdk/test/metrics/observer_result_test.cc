// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/observer_result.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"

#  include <gtest/gtest.h>

using namespace opentelemetry::sdk::metrics;
TEST(ObserverResult, BasicTests)
{
  const AttributesProcessor *attributes_processor = new DefaultAttributesProcessor();

  ObserverResult<long> observer_result(attributes_processor);

  observer_result.Observe(10l);
  observer_result.Observe(20l);
  EXPECT_EQ(observer_result.GetMeasurements().size(), 1);

  std::map<std::string, int64_t> m1 = {{"k2", 12}};
  observer_result.Observe(
      30l, opentelemetry::common::KeyValueIterableView<std::map<std::string, int64_t>>(m1));
  EXPECT_EQ(observer_result.GetMeasurements().size(), 2);

  observer_result.Observe(
      40l, opentelemetry::common::KeyValueIterableView<std::map<std::string, int64_t>>(m1));
  EXPECT_EQ(observer_result.GetMeasurements().size(), 2);

  std::map<std::string, int64_t> m2 = {{"k2", 12}, {"k4", 12}};
  observer_result.Observe(
      40l, opentelemetry::common::KeyValueIterableView<std::map<std::string, int64_t>>(m2));
  EXPECT_EQ(observer_result.GetMeasurements().size(), 3);

  delete attributes_processor;
}

#endif