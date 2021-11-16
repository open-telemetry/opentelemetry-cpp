// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include <thread>

#  include "opentelemetry/sdk/_metrics/aggregator/min_max_sum_count_aggregator.h"

using namespace opentelemetry::sdk::metrics;
namespace metrics_api = opentelemetry::metrics;

TEST(MinMaxSumCountAggregator, Update)
{
  // This tests that the aggregator updates the maintained value correctly
  // after a call to the update() function.
  MinMaxSumCountAggregator<int> agg(metrics_api::InstrumentKind::Counter);
  auto value_set = agg.get_values();
  ASSERT_EQ(value_set[0], 0);
  ASSERT_EQ(value_set[1], 0);
  ASSERT_EQ(value_set[2], 0);
  ASSERT_EQ(value_set[3], 0);

  // 1 + 2 + 3 + ... + 10 = 55
  for (int i = 1; i <= 10; ++i)
  {
    agg.update(i);
  }

  value_set = agg.get_values();
  ASSERT_EQ(value_set[0], 1);   // min
  ASSERT_EQ(value_set[1], 10);  // max
  ASSERT_EQ(value_set[2], 55);  // sum
  ASSERT_EQ(value_set[3], 10);  // count
}

TEST(MinMaxSumCountAggregator, FirstUpdate)
{
  // This tests that the aggregator appropriately maintains the min and
  // max values after a single update call.
  MinMaxSumCountAggregator<int> agg(metrics_api::InstrumentKind::Counter);
  agg.update(1);
  auto value_set = agg.get_values();
  ASSERT_EQ(value_set[0], 1);  // min
  ASSERT_EQ(value_set[1], 1);  // max
  ASSERT_EQ(value_set[2], 1);  // sum
  ASSERT_EQ(value_set[3], 1);  // count
}

TEST(MinMaxSumCountAggregator, Checkpoint)
{
  // This test verifies that the default checkpoint is set correctly
  // and that the checkpoint values update correctly after a call
  // to the checkpoint() function.
  MinMaxSumCountAggregator<int> agg(metrics_api::InstrumentKind::Counter);

  // Verify that the default checkpoint is set correctly.
  auto checkpoint_set = agg.get_checkpoint();
  ASSERT_EQ(checkpoint_set[0], 0);  // min
  ASSERT_EQ(checkpoint_set[1], 0);  // max
  ASSERT_EQ(checkpoint_set[2], 0);  // sum
  ASSERT_EQ(checkpoint_set[3], 0);  // count

  // 1 + 2 + 3 + ... + 10 = 55
  for (int i = 1; i <= 10; ++i)
  {
    agg.update(i);
  }

  agg.checkpoint();

  // Verify that the checkpoint values were updated.
  checkpoint_set = agg.get_checkpoint();
  ASSERT_EQ(checkpoint_set[0], 1);   // min
  ASSERT_EQ(checkpoint_set[1], 10);  // max
  ASSERT_EQ(checkpoint_set[2], 55);  // sum
  ASSERT_EQ(checkpoint_set[3], 10);  // count

  // Verify that the current values were reset to the default state.
  auto value_set = agg.get_values();
  ASSERT_EQ(value_set[0], 0);  // min
  ASSERT_EQ(value_set[1], 0);  // max
  ASSERT_EQ(value_set[2], 0);  // sum
  ASSERT_EQ(value_set[3], 0);  // count
}

TEST(MinMaxSumCountAggregator, Merge)
{
  // This tests that the values_ vector is updated correctly after
  // two aggregators are merged together.
  MinMaxSumCountAggregator<int> agg1(metrics_api::InstrumentKind::Counter);
  MinMaxSumCountAggregator<int> agg2(metrics_api::InstrumentKind::Counter);

  // 1 + 2 + 3 + ... + 10 = 55
  for (int i = 1; i <= 10; ++i)
  {
    agg1.update(i);
  }

  // 1 + 2 + 3 + ... + 20 = 210
  for (int i = 1; i <= 20; ++i)
  {
    agg2.update(i);
  }

  agg1.merge(agg2);

  // Verify that the current values were changed by the merge.
  auto value_set = agg1.get_values();
  ASSERT_EQ(value_set[0], 1);    // min
  ASSERT_EQ(value_set[1], 20);   // max
  ASSERT_EQ(value_set[2], 265);  // sum
  ASSERT_EQ(value_set[3], 30);   // count
}

TEST(MinMaxSumCountAggregator, BadMerge)
{
  // This verifies that we encounter and error when we try to merge
  // two aggregators of different numeric types together.
  MinMaxSumCountAggregator<int> agg1(metrics_api::InstrumentKind::Counter);
  MinMaxSumCountAggregator<int> agg2(metrics_api::InstrumentKind::ValueRecorder);

  agg1.update(1);
  agg2.update(2);

  agg1.merge(agg2);

  // Verify that the values did NOT merge
  auto value_set = agg1.get_values();
  ASSERT_EQ(value_set[0], 1);  // min
  ASSERT_EQ(value_set[0], 1);  // max
  ASSERT_EQ(value_set[0], 1);  // sum
  ASSERT_EQ(value_set[0], 1);  // count
}

TEST(MinMaxSumCountAggregator, Types)
{
  // This test verifies that we do not encounter any errors when
  // using various numeric types.
  MinMaxSumCountAggregator<int> agg_int(metrics_api::InstrumentKind::Counter);
  MinMaxSumCountAggregator<long> agg_long(metrics_api::InstrumentKind::Counter);
  MinMaxSumCountAggregator<float> agg_float(metrics_api::InstrumentKind::Counter);
  MinMaxSumCountAggregator<double> agg_double(metrics_api::InstrumentKind::Counter);

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

  auto value_set = agg_int.get_values();
  ASSERT_EQ(value_set[0], 1);   // min
  ASSERT_EQ(value_set[1], 10);  // max
  ASSERT_EQ(value_set[2], 55);  // sum
  ASSERT_EQ(value_set[3], 10);  // count

  auto value_set2 = agg_long.get_values();
  ASSERT_EQ(value_set[0], 1);   // min
  ASSERT_EQ(value_set[1], 10);  // max
  ASSERT_EQ(value_set[2], 55);  // sum
  ASSERT_EQ(value_set[3], 10);  // count

  auto value_set3 = agg_float.get_values();
  ASSERT_EQ(value_set[0], 1.0);   // min
  ASSERT_EQ(value_set[1], 10.0);  // max
  ASSERT_EQ(value_set[2], 55.0);  // sum
  ASSERT_EQ(value_set[3], 10);    // count

  auto value_set4 = agg_double.get_values();
  ASSERT_EQ(value_set[0], 1.0);   // min
  ASSERT_EQ(value_set[1], 10.0);  // max
  ASSERT_EQ(value_set[2], 55.0);  // sum
  ASSERT_EQ(value_set[3], 10);    // count
}

static void callback(MinMaxSumCountAggregator<int> &agg)
{
  // 1 + 2 + ... + 10000 = 50005000
  for (int i = 1; i <= 10000; ++i)
  {
    agg.update(i);
  }
}

TEST(MinMaxSumCountAggregator, Concurrency)
{
  // This test checks that the aggregator updates appropriately
  // when called in a multi-threaded context.
  MinMaxSumCountAggregator<int> agg(metrics_api::InstrumentKind::Counter);

  std::thread first(&callback, std::ref(agg));
  std::thread second(&callback, std::ref(agg));

  first.join();
  second.join();

  auto value_set = agg.get_values();
  ASSERT_EQ(value_set[0], 1);
  ASSERT_EQ(value_set[1], 10000);
  ASSERT_EQ(value_set[2], 2 * 50005000);
  ASSERT_EQ(value_set[3], 2 * 10000);
}
#endif
