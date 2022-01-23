// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/lastvalue_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/sum_aggregation.h"

#  include "opentelemetry/nostd/variant.h"

using namespace opentelemetry::sdk::metrics;
namespace nostd = opentelemetry::nostd;
TEST(Aggregation, LongSumAggregation)
{
  LongSumAggregation aggr(true);
  auto data = aggr.Collect();
  ASSERT_TRUE(nostd::holds_alternative<SumPointData>(data));
  auto sum_data = nostd::get<SumPointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<long>(sum_data.value_));
  EXPECT_EQ(nostd::get<long>(sum_data.value_), 0l);
  EXPECT_EQ(sum_data.is_monotonic_, true);
  EXPECT_NO_THROW(aggr.Aggregate(12l, {}));
  EXPECT_NO_THROW(aggr.Aggregate(0l, {}));
  sum_data = nostd::get<SumPointData>(aggr.Collect());
  EXPECT_EQ(nostd::get<long>(sum_data.value_), 12l);
}

TEST(Aggregation, DoubleSumAggregation)
{
  DoubleSumAggregation aggr(true);
  auto data = aggr.Collect();
  ASSERT_TRUE(nostd::holds_alternative<SumPointData>(data));
  auto sum_data = nostd::get<SumPointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<double>(sum_data.value_));
  EXPECT_EQ(nostd::get<double>(sum_data.value_), 0);
  EXPECT_EQ(sum_data.is_monotonic_, true);
  EXPECT_NO_THROW(aggr.Aggregate(12.0, {}));
  EXPECT_NO_THROW(aggr.Aggregate(1.0, {}));
  sum_data = nostd::get<SumPointData>(aggr.Collect());
  EXPECT_EQ(nostd::get<double>(sum_data.value_), 13.0);
}

TEST(Aggregation, LongLastValueAggregation)
{
  LongLastValueAggregation aggr;
  auto data = aggr.Collect();
  ASSERT_TRUE(nostd::holds_alternative<LastValuePointData>(data));
  auto lastvalue_data = nostd::get<LastValuePointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<long>(lastvalue_data.value_));
  EXPECT_EQ(lastvalue_data.is_lastvalue_valid_, false);
  EXPECT_NO_THROW(aggr.Aggregate(12l, {}));
  EXPECT_NO_THROW(aggr.Aggregate(1l, {}));
  lastvalue_data = nostd::get<LastValuePointData>(aggr.Collect());
  EXPECT_EQ(nostd::get<long>(lastvalue_data.value_), 1.0);
}

TEST(Aggregation, DoubleLastValueAggregation)
{
  DoubleLastValueAggregation aggr;
  auto data = aggr.Collect();
  ASSERT_TRUE(nostd::holds_alternative<LastValuePointData>(data));
  auto lastvalue_data = nostd::get<LastValuePointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<double>(lastvalue_data.value_));
  EXPECT_EQ(lastvalue_data.is_lastvalue_valid_, false);
  EXPECT_NO_THROW(aggr.Aggregate(12.0, {}));
  EXPECT_NO_THROW(aggr.Aggregate(1.0, {}));
  lastvalue_data = nostd::get<LastValuePointData>(aggr.Collect());
  EXPECT_EQ(nostd::get<double>(lastvalue_data.value_), 1.0);
}

TEST(Aggregation, LongHistogramAggregation)
{
  LongHistogramAggregation aggr;
  auto data = aggr.Collect();
  ASSERT_TRUE(nostd::holds_alternative<HistogramPointData>(data));
  auto histogram_data = nostd::get<HistogramPointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<long>(histogram_data.sum_));
  ASSERT_TRUE(nostd::holds_alternative<std::vector<long>>(histogram_data.boundaries_));
  EXPECT_EQ(nostd::get<long>(histogram_data.sum_), 0);
  EXPECT_EQ(histogram_data.count_, 0);
  EXPECT_NO_THROW(aggr.Aggregate(12l, {}));   // lies in fourth bucket
  EXPECT_NO_THROW(aggr.Aggregate(100l, {}));  // lies in eight bucket
  histogram_data = nostd::get<HistogramPointData>(aggr.Collect());
  EXPECT_EQ(nostd::get<long>(histogram_data.sum_), 112);
  EXPECT_EQ(histogram_data.count_, 2);
  EXPECT_EQ(histogram_data.counts_[3], 1);
  EXPECT_EQ(histogram_data.counts_[7], 1);
  EXPECT_NO_THROW(aggr.Aggregate(13l, {}));   // lies in fourth bucket
  EXPECT_NO_THROW(aggr.Aggregate(252l, {}));  // lies in ninth bucket
  histogram_data = nostd::get<HistogramPointData>(aggr.Collect());
  EXPECT_EQ(histogram_data.count_, 4);
  EXPECT_EQ(histogram_data.counts_[3], 2);
  EXPECT_EQ(histogram_data.counts_[8], 1);
}

TEST(Aggregation, DoubleHistogramAggregation)
{
  DoubleHistogramAggregation aggr;
  auto data = aggr.Collect();
  ASSERT_TRUE(nostd::holds_alternative<HistogramPointData>(data));
  auto histogram_data = nostd::get<HistogramPointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<double>(histogram_data.sum_));
  ASSERT_TRUE(nostd::holds_alternative<std::vector<double>>(histogram_data.boundaries_));
  EXPECT_EQ(nostd::get<double>(histogram_data.sum_), 0);
  EXPECT_EQ(histogram_data.count_, 0);
  EXPECT_NO_THROW(aggr.Aggregate(12.0, {}));   // lies in fourth bucket
  EXPECT_NO_THROW(aggr.Aggregate(100.0, {}));  // lies in eight bucket
  histogram_data = nostd::get<HistogramPointData>(aggr.Collect());
  EXPECT_EQ(nostd::get<double>(histogram_data.sum_), 112);
  EXPECT_EQ(histogram_data.count_, 2);
  EXPECT_EQ(histogram_data.counts_[3], 1);
  EXPECT_EQ(histogram_data.counts_[7], 1);
  EXPECT_NO_THROW(aggr.Aggregate(13.0, {}));   // lies in fourth bucket
  EXPECT_NO_THROW(aggr.Aggregate(252.0, {}));  // lies in ninth bucket
  histogram_data = nostd::get<HistogramPointData>(aggr.Collect());
  EXPECT_EQ(histogram_data.count_, 4);
  EXPECT_EQ(histogram_data.counts_[3], 2);
  EXPECT_EQ(histogram_data.counts_[8], 1);
  EXPECT_EQ(nostd::get<double>(histogram_data.sum_), 377);
}
#endif