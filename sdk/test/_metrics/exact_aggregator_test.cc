// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#ifdef ENABLE_METRICS_PREVIEW
#  include <thread>

#  include "opentelemetry/sdk/_metrics/aggregator/exact_aggregator.h"

using namespace opentelemetry::sdk::metrics;
namespace metrics_api = opentelemetry::metrics;

TEST(ExactAggregatorOrdered, Update)
{
  ExactAggregator<int> agg(metrics_api::InstrumentKind::Counter);

  std::vector<int> correct;

  ASSERT_EQ(agg.get_values(), correct);

  agg.update(1);
  correct.push_back(1);

  ASSERT_EQ(agg.get_values(), std::vector<int>{1});

  for (int i = 2; i <= 5; ++i)
  {
    correct.push_back(i);
    agg.update(i);
  }
  ASSERT_EQ(agg.get_values(), correct);
}

TEST(ExactAggregatorOrdered, Checkpoint)
{
  ExactAggregator<int> agg(metrics_api::InstrumentKind::Counter);

  std::vector<int> correct;

  ASSERT_EQ(agg.get_checkpoint(), correct);

  agg.update(1);
  correct.push_back(1);
  agg.checkpoint();

  ASSERT_EQ(agg.get_checkpoint(), correct);
}

TEST(ExactAggregatorOrdered, Merge)
{
  ExactAggregator<int> agg1(metrics_api::InstrumentKind::Counter);
  ExactAggregator<int> agg2(metrics_api::InstrumentKind::Counter);

  agg1.update(1);
  agg2.update(2);
  agg1.merge(agg2);

  std::vector<int> correct{1, 2};

  ASSERT_EQ(agg1.get_values(), correct);
}

TEST(ExactAggregatorOrdered, BadMerge)
{
  // This verifies that we encounter and error when we try to merge
  // two aggregators of different numeric types together.
  ExactAggregator<int> agg1(metrics_api::InstrumentKind::Counter);
  ExactAggregator<int> agg2(metrics_api::InstrumentKind::ValueRecorder);

  agg1.update(1);
  agg2.update(2);

  agg1.merge(agg2);

  // Verify that the aggregators did NOT merge
  std::vector<int> correct{1};
  ASSERT_EQ(agg1.get_values(), correct);
}

TEST(ExactAggregatorOrdered, Types)
{
  // This test verifies that we do not encounter any errors when
  // using various numeric types.
  ExactAggregator<int> agg_int(metrics_api::InstrumentKind::Counter);
  ExactAggregator<long> agg_long(metrics_api::InstrumentKind::Counter);
  ExactAggregator<float> agg_float(metrics_api::InstrumentKind::Counter);
  ExactAggregator<double> agg_double(metrics_api::InstrumentKind::Counter);

  for (int i = 1; i <= 5; ++i)
  {
    agg_int.update(i);
    agg_long.update(i);
  }

  for (float i = 1.0; i <= 5.0; i += 1)
  {
    agg_float.update(i);
    agg_double.update(i);
  }

  std::vector<int> correct_int{1, 2, 3, 4, 5};
  std::vector<long> correct_long{1, 2, 3, 4, 5};
  std::vector<float> correct_float{1.0, 2.0, 3.0, 4.0, 5.0};
  std::vector<double> correct_double{1.0, 2.0, 3.0, 4.0, 5.0};

  ASSERT_EQ(agg_int.get_values(), correct_int);
  ASSERT_EQ(agg_long.get_values(), correct_long);
  ASSERT_EQ(agg_float.get_values(), correct_float);
  ASSERT_EQ(agg_double.get_values(), correct_double);
}

TEST(ExactAggregatorQuant, Update)
{
  ExactAggregator<int> agg(metrics_api::InstrumentKind::Counter, true);

  std::vector<int> correct;

  ASSERT_EQ(agg.get_values(), correct);

  agg.update(1);
  correct.push_back(1);

  ASSERT_EQ(agg.get_values(), std::vector<int>{1});

  for (int i = 2; i <= 5; ++i)
  {
    correct.push_back(i);
    agg.update(i);
  }
  ASSERT_EQ(agg.get_values(), correct);
}

TEST(ExactAggregatorQuant, Checkpoint)
{
  // This test verifies that the aggregator updates correctly when
  // quantile estimation is turned on.

  ExactAggregator<int> agg(metrics_api::InstrumentKind::Counter, true);

  std::vector<int> correct;

  ASSERT_EQ(agg.get_checkpoint(), correct);

  agg.update(1);
  agg.update(0);
  agg.update(-1);

  // The vector MUST be sorted when checkpointed
  correct.push_back(-1);
  correct.push_back(0);
  correct.push_back(1);
  agg.checkpoint();

  ASSERT_EQ(agg.get_checkpoint(), correct);
}

TEST(ExactAggregatorQuant, Quantile)
{
  // This test verifies that the quantile estimation function returns
  // the correct values.

  ExactAggregator<int> agg(metrics_api::InstrumentKind::Counter, true);

  std::vector<int> tmp{3, 9, 42, 57, 163, 210, 272, 300};
  for (int i : tmp)
  {
    agg.update(i);
  }
  agg.checkpoint();
  ASSERT_EQ(agg.get_quantiles(.25), 42);
  ASSERT_EQ(agg.get_quantiles(0.5), 163);
  ASSERT_EQ(agg.get_quantiles(0.75), 272);
}

TEST(ExactAggregatorInOrder, Quantile)
{
  // This test verifies that if the user has an exact aggregator in "in-order" mode
  // an exception will be thrown if they call the quantile() function.
  ExactAggregator<int> agg(metrics_api::InstrumentKind::Counter);

  std::vector<int> tmp{3, 9, 42, 57, 163, 210, 272, 300};
  for (int i : tmp)
  {
    agg.update(i);
  }
  agg.checkpoint();
#  if __EXCEPTIONS
  ASSERT_THROW(agg.get_quantiles(0.5), std::domain_error);
#  else
#  endif
}

void callback(ExactAggregator<int> &agg)
{
  for (int i = 1; i <= 10000; ++i)
  {
    agg.update(i);
  }
}

TEST(ExactAggregatorQuant, Concurrency)
{
  // This test checks that the aggregator updates appropriately
  // when called in a multi-threaded context.
  ExactAggregator<int> agg(metrics_api::InstrumentKind::Counter, true);

  std::thread first(&callback, std::ref(agg));
  std::thread second(&callback, std::ref(agg));

  first.join();
  second.join();

  std::vector<int> correct;
  for (int i = 1; i <= 10000; ++i)
  {
    correct.push_back(i);
    correct.push_back(i);
  }
  agg.checkpoint();

  ASSERT_EQ(agg.get_checkpoint(), correct);
}
#else
TEST(ExactAggregatorQuant, DummyTest)
{
  // empty
}
#endif
