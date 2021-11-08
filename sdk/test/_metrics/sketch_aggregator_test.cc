// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/_metrics/aggregator/sketch_aggregator.h"

#  include <gtest/gtest.h>
#  include <iostream>
#  include <numeric>
#  include <thread>

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

// Test updating with a uniform set of updates
TEST(Sketch, UniformValues)
{
  SketchAggregator<int> alpha(metrics_api::InstrumentKind::ValueRecorder, .000005);

  EXPECT_EQ(alpha.get_aggregator_kind(), AggregatorKind::Sketch);

  alpha.checkpoint();
  EXPECT_EQ(alpha.get_checkpoint().size(), 2);
  EXPECT_EQ(alpha.get_boundaries().size(), 0);
  EXPECT_EQ(alpha.get_counts().size(), 0);

  for (int i = 0; i < 60; i++)
  {
    alpha.update(i);
  }

  alpha.checkpoint();

  EXPECT_EQ(alpha.get_boundaries().size(), 60);
  EXPECT_EQ(alpha.get_counts().size(), 60);

  EXPECT_EQ(alpha.get_checkpoint()[0], 1770);
  EXPECT_EQ(alpha.get_checkpoint()[1], 60);
}

// Test updating with a normal distribution
TEST(Sketch, NormalValues)
{
  SketchAggregator<int> alpha(metrics_api::InstrumentKind::ValueRecorder, .0005);

  std::vector<int> vals{1, 3, 3, 5, 5, 5, 7, 7, 7, 7, 9, 9, 9, 11, 11, 13};
  for (int i : vals)
  {
    alpha.update(i);
  }
  alpha.checkpoint();

  EXPECT_EQ(alpha.get_checkpoint()[0], std::accumulate(vals.begin(), vals.end(), 0));
  EXPECT_EQ(alpha.get_checkpoint()[1], vals.size());

  std::vector<int> correct = {1, 2, 3, 4, 3, 2, 1};
  EXPECT_EQ(alpha.get_counts(), correct);

  std::vector<double> captured_bounds = alpha.get_boundaries();
  for (size_t i = 0; i < captured_bounds.size(); i++)
  {
    captured_bounds[i] = round(captured_bounds[i]);
  }

  // It is not guaranteed that bounds are correct once the bucket sizes pass 1000
  std::vector<double> correct_bounds = {1, 3, 5, 7, 9, 11, 13};
  EXPECT_EQ(captured_bounds, correct_bounds);
}

int randVal()
{
  return rand() % 100000;
}

/** Note that in this case, "Large" refers to a number of distinct values which exceed the maximum
 * number of allowed buckets.
 */
TEST(Sketch, QuantileSmall)
{
  SketchAggregator<int> alpha(metrics_api::InstrumentKind::ValueRecorder, .00005);

  std::vector<int> vals1(2048);
  std::generate(vals1.begin(), vals1.end(), randVal);

  std::vector<int> vals2(2048);
  std::generate(vals1.begin(), vals1.end(), randVal);

  for (int i : vals1)
  {
    alpha.update(i);
  }
  alpha.checkpoint();
  std::sort(vals1.begin(), vals1.end());

  EXPECT_TRUE(abs(alpha.get_quantiles(.25) - vals1[2048 * .25 - 1]) <= 10);
  EXPECT_TRUE(abs(alpha.get_quantiles(.50) - vals1[2048 * .50 - 1]) <= 10);
  EXPECT_TRUE(abs(alpha.get_quantiles(.75) - vals1[2048 * .75 - 1]) <= 10);
}

TEST(Sketch, UpdateQuantileLarge)
{
  SketchAggregator<int> alpha(metrics_api::InstrumentKind::ValueRecorder, .0005, 7);
  std::vector<int> vals{1, 3, 3, 5, 5, 5, 7, 7, 7, 7, 9, 9, 9, 11, 11, 13};
  for (int i : vals)
  {
    alpha.update(i);
  }

  // This addition should trigger the "1" and "3" buckets to merge
  alpha.update(15);
  alpha.checkpoint();

  std::vector<int> correct = {3, 3, 4, 3, 2, 1, 1};
  EXPECT_EQ(alpha.get_counts(), correct);

  for (int i : vals)
  {
    alpha.update(i);
  }
  alpha.update(15);
  alpha.update(17);
  alpha.checkpoint();

  correct = {6, 4, 3, 2, 1, 1, 1};
  EXPECT_EQ(alpha.get_counts(), correct);
}

TEST(Sketch, MergeSmall)
{
  SketchAggregator<int> alpha(metrics_api::InstrumentKind::ValueRecorder, .0005);
  SketchAggregator<int> beta(metrics_api::InstrumentKind::ValueRecorder, .0005);

  std::vector<int> vals{1, 3, 3, 5, 5, 5, 7, 7, 7, 7, 9, 9, 9, 11, 11, 13};
  for (int i : vals)
  {
    alpha.update(i);
  }

  std::vector<int> otherVals{1, 1, 1, 1, 11, 11, 13, 13, 13, 15};
  for (int i : otherVals)
  {
    beta.update(i);
  }

  alpha.merge(beta);
  alpha.checkpoint();

  EXPECT_EQ(alpha.get_checkpoint()[0], std::accumulate(vals.begin(), vals.end(), 0) +
                                           std::accumulate(otherVals.begin(), otherVals.end(), 0));
  EXPECT_EQ(alpha.get_checkpoint()[1], vals.size() + otherVals.size());

  std::vector<int> correct = {5, 2, 3, 4, 3, 4, 4, 1};
  EXPECT_EQ(alpha.get_counts(), correct);
}

TEST(Sketch, MergeLarge)
{
  SketchAggregator<int> alpha(metrics_api::InstrumentKind::ValueRecorder, .0005, 7);
  SketchAggregator<int> beta(metrics_api::InstrumentKind::ValueRecorder, .0005, 7);

  std::vector<int> vals{1, 3, 3, 5, 5, 5, 7, 7, 7, 7, 9, 9, 9, 11, 11, 13};
  for (int i : vals)
  {
    alpha.update(i);
  }

  std::vector<int> otherVals{1, 1, 1, 1, 11, 11, 13, 13, 13, 15};
  for (int i : otherVals)
  {
    beta.update(i);
  }

  alpha.merge(beta);
  alpha.checkpoint();

  EXPECT_EQ(alpha.get_checkpoint()[0], std::accumulate(vals.begin(), vals.end(), 0) +
                                           std::accumulate(otherVals.begin(), otherVals.end(), 0));
  EXPECT_EQ(alpha.get_checkpoint()[1], vals.size() + otherVals.size());

  std::vector<int> correct = {7, 3, 4, 3, 4, 4, 1};
  EXPECT_EQ(alpha.get_counts(), correct);
}

// Update callback used to validate multi-threaded performance
void sketchUpdateCallback(Aggregator<int> &agg, std::vector<int> vals)
{
  for (int i : vals)
  {
    agg.update(i);
  }
}

TEST(Sketch, Concurrency)
{
  SketchAggregator<int> alpha(metrics_api::InstrumentKind::ValueRecorder, .0005, 20);

  std::vector<int> vals1(1000);
  std::generate(vals1.begin(), vals1.end(), randVal);

  std::vector<int> vals2(1000);
  std::generate(vals2.begin(), vals2.end(), randVal);

  std::thread first(sketchUpdateCallback, std::ref(alpha), vals1);
  std::thread second(sketchUpdateCallback, std::ref(alpha), vals2);

  first.join();
  second.join();

  SketchAggregator<int> beta(metrics_api::InstrumentKind::ValueRecorder, .0005, 20);

  for (int i : vals1)
  {
    beta.update(i);
  }
  for (int i : vals2)
  {
    beta.update(i);
  }

  alpha.checkpoint();
  beta.checkpoint();

  EXPECT_EQ(alpha.get_checkpoint(), beta.get_checkpoint());
  EXPECT_EQ(alpha.get_counts(), beta.get_counts());
  EXPECT_EQ(alpha.get_boundaries(), beta.get_boundaries());
}

#  if __EXCEPTIONS

TEST(Sketch, Errors)
{

  SketchAggregator<int> tol1(metrics_api::InstrumentKind::ValueRecorder, .000005);
  SketchAggregator<int> tol2(metrics_api::InstrumentKind::ValueRecorder, .005);
  SketchAggregator<int> sz1(metrics_api::InstrumentKind::ValueRecorder, .000005, 2938);
  SketchAggregator<int> sz2(metrics_api::InstrumentKind::ValueRecorder, .000005);

  EXPECT_ANY_THROW(tol1.merge(tol2));
  EXPECT_ANY_THROW(sz1.merge(sz2));
  EXPECT_ANY_THROW(tol1.get_quantiles(-.000001));
  EXPECT_ANY_THROW(tol1.get_quantiles(1.000001));
}

#  endif

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
