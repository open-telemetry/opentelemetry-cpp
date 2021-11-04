// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/_metrics/aggregator/counter_aggregator.h"

#  include <gtest/gtest.h>
#  include <numeric>
#  include <thread>

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

TEST(CounterAggregator, NoUpdates)
{
  CounterAggregator<int> alpha(metrics_api::InstrumentKind::Counter);

  EXPECT_EQ(alpha.get_checkpoint().size(), 1);
  EXPECT_EQ(alpha.get_checkpoint()[0], 0);

  alpha.checkpoint();
  EXPECT_EQ(alpha.get_checkpoint().size(), 1);
  EXPECT_EQ(alpha.get_checkpoint()[0], 0);
}

TEST(CounterAggregator, Update)
{
  CounterAggregator<int> alpha(metrics_api::InstrumentKind::Counter);
  CounterAggregator<int> beta(metrics_api::InstrumentKind::Counter);

  for (int i = 0; i < 123456; i++)
  {
    alpha.update(1);
  }

  int sum = 0;
  for (int i = 0; i < 100; i++)
  {
    int tmp = std::rand() % 128;
    beta.update(tmp);
    sum += tmp;
  }

  EXPECT_EQ(alpha.get_checkpoint()[0], 0);  // checkpoint shouldn't change even with updates
  EXPECT_EQ(beta.get_checkpoint()[0], 0);

  alpha.checkpoint();
  beta.checkpoint();

  EXPECT_EQ(alpha.get_checkpoint()[0], 123456);
  EXPECT_EQ(beta.get_checkpoint()[0], sum);

  alpha.update(15);
  alpha.checkpoint();
  EXPECT_EQ(alpha.get_checkpoint()[0], 15);  // reset to 0 after first checkpoint call
}

// callback update function used to test concurrent calls
void incrementingCallback(Aggregator<int> &agg)
{
  for (int i = 0; i < 2000000; i++)
  {
    agg.update(1);
  }
}

TEST(CounterAggregator, Concurrency)
{
  CounterAggregator<int> alpha(metrics_api::InstrumentKind::Counter);

  // spawn new threads that initiate the callback
  std::thread first(incrementingCallback, std::ref(alpha));
  std::thread second(incrementingCallback, std::ref(alpha));

  first.join();
  second.join();

  alpha.checkpoint();

  // race conditions result in values below 2*2000000
  EXPECT_EQ(alpha.get_checkpoint()[0], 2 * 2000000);
}

TEST(CounterAggregator, Merge)
{
  CounterAggregator<int> alpha(metrics_api::InstrumentKind::Counter);
  CounterAggregator<int> beta(metrics_api::InstrumentKind::Counter);

  alpha.merge(beta);

  alpha.checkpoint();
  EXPECT_EQ(alpha.get_checkpoint()[0], 0);  // merge with no updates

  for (int i = 0; i < 500; i++)
  {
    alpha.update(1);
  }

  for (int i = 0; i < 700; i++)
  {
    beta.update(1);
  }

  alpha.merge(beta);
  alpha.checkpoint();
  EXPECT_EQ(alpha.get_checkpoint()[0], 1200);

  // HistogramAggregator gamma(metrics_api::BoundInstrumentKind::BoundValueRecorder);
  // ASSERT_THROW(alpha.merge(gamma), AggregatorMismatch);
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
