// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include <thread>

#  include "opentelemetry/sdk/_metrics/aggregator/gauge_aggregator.h"

using namespace opentelemetry::sdk::metrics;
namespace metrics_api = opentelemetry::metrics;

TEST(GaugeAggregator, Update)
{
  // This tests that the aggregator updates the maintained value correctly
  // after a call to the update() function.
  auto agg = new GaugeAggregator<int>(metrics_api::InstrumentKind::Counter);

  // Verify default value
  ASSERT_EQ(agg->get_values()[0], 0);

  // Verify that the value updates correctly
  agg->update(1);
  ASSERT_EQ(agg->get_values()[0], 1);

  // Verify that the value continually updates correctly
  for (int i = 0; i < 10; ++i)
  {
    agg->update(i);
  }
  ASSERT_EQ(agg->get_values()[0], 9);
  delete agg;
}

TEST(GaugeAggregator, Checkpoint)
{
  // This tests that the aggregator correctly updates the
  // checkpoint_ value after a call to update() followed
  // by a call to checkpoint().
  GaugeAggregator<int> agg(metrics_api::InstrumentKind::Counter);

  // Verify default checkpoint, before updates
  ASSERT_EQ(agg.get_checkpoint()[0], 0);

  agg.update(10);
  agg.checkpoint();

  // Verify that the new checkpoint contains the update value
  ASSERT_EQ(agg.get_checkpoint()[0], 10);
}

TEST(GaugeAggregator, Merge)
{
  // This tests that the values_ vector is updated correctly after
  // two aggregators are merged together.
  GaugeAggregator<int> agg1(metrics_api::InstrumentKind::Counter);
  GaugeAggregator<int> agg2(metrics_api::InstrumentKind::Counter);

  agg1.update(1);
  agg2.update(2);

  agg1.merge(agg2);

  // Verify that the aggregators merged and the value was updated correctly
  ASSERT_EQ(agg1.get_values()[0], 2);
}

TEST(GaugeAggregator, BadMerge)
{
  // This verifies that we encounter and error when we try to merge
  // two aggregators of different numeric types together.
  GaugeAggregator<int> agg1(metrics_api::InstrumentKind::Counter);
  GaugeAggregator<int> agg2(metrics_api::InstrumentKind::ValueRecorder);

  agg1.update(1);
  agg2.update(2);
  agg1.merge(agg2);

  // Verify that the aggregators did NOT merge
  std::vector<int> correct{1};
  ASSERT_EQ(agg1.get_values(), correct);
}

TEST(GaugeAggregator, Types)
{
  // This test verifies that we do not encounter any errors when
  // using various numeric types.
  GaugeAggregator<int> agg_int(metrics_api::InstrumentKind::Counter);
  GaugeAggregator<long> agg_long(metrics_api::InstrumentKind::Counter);
  GaugeAggregator<float> agg_float(metrics_api::InstrumentKind::Counter);
  GaugeAggregator<double> agg_double(metrics_api::InstrumentKind::Counter);

  for (int i = 1; i <= 10; ++i)
  {
    agg_int.update(i);
    agg_long.update(i);
  }

  for (float i = 1.0; i <= 10.0; i += 1)
  {
    agg_float.update(i);
    agg_double.update(i);
  }

  ASSERT_EQ(agg_int.get_values()[0], 10);
  ASSERT_EQ(agg_long.get_values()[0], 10);
  ASSERT_EQ(agg_float.get_values()[0], 10.0);
  ASSERT_EQ(agg_double.get_values()[0], 10.0);
}

static void callback(GaugeAggregator<int> &agg)
{
  for (int i = 1; i <= 10000; ++i)
  {
    agg.update(i);
  }
}

TEST(GaugeAggregator, Concurrency)
{
  // This test checks that the aggregator updates appropriately
  // when called in a multi-threaded context.
  GaugeAggregator<int> agg(metrics_api::InstrumentKind::Counter);

  std::thread first(&callback, std::ref(agg));
  std::thread second(&callback, std::ref(agg));

  first.join();
  second.join();

  ASSERT_EQ(agg.get_values()[0], 10000);
}
#endif
