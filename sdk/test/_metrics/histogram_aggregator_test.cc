// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/_metrics/aggregator/histogram_aggregator.h"

#  include <gtest/gtest.h>
#  include <iostream>
#  include <numeric>
#  include <thread>

// #include <chrono>

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

// Test updating with a uniform set of updates
TEST(Histogram, Uniform)
{
  std::vector<double> boundaries{10, 20, 30, 40, 50};
  HistogramAggregator<int> alpha(metrics_api::InstrumentKind::Counter, boundaries);

  EXPECT_EQ(alpha.get_aggregator_kind(), AggregatorKind::Histogram);

  alpha.checkpoint();
  EXPECT_EQ(alpha.get_checkpoint().size(), 2);
  EXPECT_EQ(alpha.get_counts().size(), 6);

  for (int i = 0; i < 60; i++)
  {
    alpha.update(i);
  }

  alpha.checkpoint();

  EXPECT_EQ(alpha.get_checkpoint()[0], 1770);
  EXPECT_EQ(alpha.get_checkpoint()[1], 60);

  std::vector<int> correct = {10, 10, 10, 10, 10, 10};
  EXPECT_EQ(alpha.get_counts(), correct);
}

// Test updating with a normal distribution
TEST(Histogram, Normal)
{
  std::vector<double> boundaries{2, 4, 6, 8, 10, 12};
  HistogramAggregator<int> alpha(metrics_api::InstrumentKind::Counter, boundaries);

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
}

TEST(Histogram, Merge)
{
  std::vector<double> boundaries{2, 4, 6, 8, 10, 12};
  HistogramAggregator<int> alpha(metrics_api::InstrumentKind::Counter, boundaries);
  HistogramAggregator<int> beta(metrics_api::InstrumentKind::Counter, boundaries);

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

  std::vector<int> correct = {5, 2, 3, 4, 3, 4, 5};
  EXPECT_EQ(alpha.get_counts(), correct);
}

// Update callback used to validate multi-threaded performance
void histogramUpdateCallback(Aggregator<int> &agg, std::vector<int> vals)
{
  for (int i : vals)
  {
    agg.update(i);
  }
}

int randVal()
{
  return rand() % 15;
}

TEST(Histogram, Concurrency)
{
  std::vector<double> boundaries{2, 4, 6, 8, 10, 12};
  HistogramAggregator<int> alpha(metrics_api::InstrumentKind::Counter, boundaries);

  std::vector<int> vals1(1000);
  std::generate(vals1.begin(), vals1.end(), randVal);

  std::vector<int> vals2(1000);
  std::generate(vals2.begin(), vals2.end(), randVal);

  std::thread first(histogramUpdateCallback, std::ref(alpha), vals1);
  std::thread second(histogramUpdateCallback, std::ref(alpha), vals2);

  first.join();
  second.join();

  HistogramAggregator<int> beta(metrics_api::InstrumentKind::Counter, boundaries);

  // Timing harness to compare linear and binary insertion
  // auto start = std::chrono::system_clock::now();
  for (int i : vals1)
  {
    beta.update(i);
  }
  for (int i : vals2)
  {
    beta.update(i);
  }
  // auto end = std::chrono::system_clock::now();
  // auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  // std::cout <<"Update time: " <<elapsed.count() <<std::endl;

  alpha.checkpoint();
  beta.checkpoint();

  EXPECT_EQ(alpha.get_checkpoint(), beta.get_checkpoint());
  EXPECT_EQ(alpha.get_counts(), beta.get_counts());
}

#  if __EXCEPTIONS

TEST(Histogram, Errors)
{
  std::vector<double> boundaries{2, 4, 6, 8, 10, 12};
  std::vector<double> boundaries2{1, 4, 6, 8, 10, 12};
  std::vector<double> unsortedBoundaries{10, 12, 4, 6, 8};
  EXPECT_ANY_THROW(
      HistogramAggregator<int> alpha(metrics_api::InstrumentKind::Counter, unsortedBoundaries));

  HistogramAggregator<int> beta(metrics_api::InstrumentKind::Counter, boundaries);
  HistogramAggregator<int> gamma(metrics_api::InstrumentKind::Counter, boundaries2);

  EXPECT_ANY_THROW(beta.merge(gamma));
}

#  endif

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
