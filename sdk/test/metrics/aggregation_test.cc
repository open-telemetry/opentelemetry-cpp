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
  LongSumAggregation aggr;
  auto data = aggr.ToPoint();
  ASSERT_TRUE(nostd::holds_alternative<SumPointData>(data));
  auto sum_data = nostd::get<SumPointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<long>(sum_data.value_));
  EXPECT_EQ(nostd::get<long>(sum_data.value_), 0l);
  EXPECT_NO_THROW(aggr.Aggregate(12l, {}));
  EXPECT_NO_THROW(aggr.Aggregate(0l, {}));
  sum_data = nostd::get<SumPointData>(aggr.ToPoint());
  EXPECT_EQ(nostd::get<long>(sum_data.value_), 12l);
}

TEST(Aggregation, DoubleSumAggregation)
{
  DoubleSumAggregation aggr;
  auto data = aggr.ToPoint();
  ASSERT_TRUE(nostd::holds_alternative<SumPointData>(data));
  auto sum_data = nostd::get<SumPointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<double>(sum_data.value_));
  EXPECT_EQ(nostd::get<double>(sum_data.value_), 0);
  EXPECT_NO_THROW(aggr.Aggregate(12.0, {}));
  EXPECT_NO_THROW(aggr.Aggregate(1.0, {}));
  sum_data = nostd::get<SumPointData>(aggr.ToPoint());
  EXPECT_EQ(nostd::get<double>(sum_data.value_), 13.0);
}

TEST(Aggregation, LongLastValueAggregation)
{
  LongLastValueAggregation aggr;
  auto data = aggr.ToPoint();
  ASSERT_TRUE(nostd::holds_alternative<LastValuePointData>(data));
  auto lastvalue_data = nostd::get<LastValuePointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<long>(lastvalue_data.value_));
  EXPECT_EQ(lastvalue_data.is_lastvalue_valid_, false);
  EXPECT_NO_THROW(aggr.Aggregate(12l, {}));
  EXPECT_NO_THROW(aggr.Aggregate(1l, {}));
  lastvalue_data = nostd::get<LastValuePointData>(aggr.ToPoint());
  EXPECT_EQ(nostd::get<long>(lastvalue_data.value_), 1.0);
}

TEST(Aggregation, DoubleLastValueAggregation)
{
  DoubleLastValueAggregation aggr;
  auto data = aggr.ToPoint();
  ASSERT_TRUE(nostd::holds_alternative<LastValuePointData>(data));
  auto lastvalue_data = nostd::get<LastValuePointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<double>(lastvalue_data.value_));
  EXPECT_EQ(lastvalue_data.is_lastvalue_valid_, false);
  EXPECT_NO_THROW(aggr.Aggregate(12.0, {}));
  EXPECT_NO_THROW(aggr.Aggregate(1.0, {}));
  lastvalue_data = nostd::get<LastValuePointData>(aggr.ToPoint());
  EXPECT_EQ(nostd::get<double>(lastvalue_data.value_), 1.0);
}

TEST(Aggregation, LongHistogramAggregation)
{
  LongHistogramAggregation aggr;
  auto data = aggr.ToPoint();
  ASSERT_TRUE(nostd::holds_alternative<HistogramPointData>(data));
  auto histogram_data = nostd::get<HistogramPointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<long>(histogram_data.sum_));
  ASSERT_TRUE(nostd::holds_alternative<std::list<long>>(histogram_data.boundaries_));
  EXPECT_EQ(nostd::get<long>(histogram_data.sum_), 0);
  EXPECT_EQ(histogram_data.count_, 0);
  EXPECT_NO_THROW(aggr.Aggregate(12l, {}));   // lies in fourth bucket
  EXPECT_NO_THROW(aggr.Aggregate(100l, {}));  // lies in eight bucket
  histogram_data = nostd::get<HistogramPointData>(aggr.ToPoint());
  EXPECT_EQ(nostd::get<long>(histogram_data.sum_), 112);
  EXPECT_EQ(histogram_data.count_, 2);
  EXPECT_EQ(histogram_data.counts_[3], 1);
  EXPECT_EQ(histogram_data.counts_[7], 1);
  EXPECT_NO_THROW(aggr.Aggregate(13l, {}));   // lies in fourth bucket
  EXPECT_NO_THROW(aggr.Aggregate(252l, {}));  // lies in ninth bucket
  histogram_data = nostd::get<HistogramPointData>(aggr.ToPoint());
  EXPECT_EQ(histogram_data.count_, 4);
  EXPECT_EQ(histogram_data.counts_[3], 2);
  EXPECT_EQ(histogram_data.counts_[8], 1);

  // Merge
  LongHistogramAggregation aggr1;
  aggr1.Aggregate(1l, {});
  aggr1.Aggregate(11l, {});
  aggr1.Aggregate(26l, {});

  LongHistogramAggregation aggr2;
  aggr2.Aggregate(2l, {});
  aggr2.Aggregate(3l, {});
  aggr2.Aggregate(13l, {});
  aggr2.Aggregate(28l, {});
  aggr2.Aggregate(105l, {});

  auto aggr3     = aggr1.Merge(aggr2);
  histogram_data = nostd::get<HistogramPointData>(aggr3->ToPoint());

  EXPECT_EQ(histogram_data.count_, 8);      // 3 each from aggr1 and aggr2
  EXPECT_EQ(histogram_data.counts_[1], 3);  // 1, 2, 3
  EXPECT_EQ(histogram_data.counts_[3], 2);  // 11, 13
  EXPECT_EQ(histogram_data.counts_[4], 2);  // 25, 28
  EXPECT_EQ(histogram_data.counts_[7], 1);  // 105

  // Diff
  auto aggr4     = aggr1.Diff(aggr2);
  histogram_data = nostd::get<HistogramPointData>(aggr4->ToPoint());
  EXPECT_EQ(histogram_data.count_, 2);      // aggr2:5 - aggr1:3
  EXPECT_EQ(histogram_data.counts_[1], 1);  // aggr2(2, 3) - aggr1(1)
  EXPECT_EQ(histogram_data.counts_[3], 0);  // aggr2(13) - aggr1(11)
  EXPECT_EQ(histogram_data.counts_[4], 0);  // aggr2(28) - aggr1(25)
  EXPECT_EQ(histogram_data.counts_[7], 1);  // aggr2(105) - aggr1(0)
}

TEST(Aggregation, DoubleHistogramAggregation)
{
  DoubleHistogramAggregation aggr;
  auto data = aggr.ToPoint();
  ASSERT_TRUE(nostd::holds_alternative<HistogramPointData>(data));
  auto histogram_data = nostd::get<HistogramPointData>(data);
  ASSERT_TRUE(nostd::holds_alternative<double>(histogram_data.sum_));
  ASSERT_TRUE(nostd::holds_alternative<std::list<double>>(histogram_data.boundaries_));
  EXPECT_EQ(nostd::get<double>(histogram_data.sum_), 0);
  EXPECT_EQ(histogram_data.count_, 0);
  EXPECT_NO_THROW(aggr.Aggregate(12.0, {}));   // lies in fourth bucket
  EXPECT_NO_THROW(aggr.Aggregate(100.0, {}));  // lies in eight bucket
  histogram_data = nostd::get<HistogramPointData>(aggr.ToPoint());
  EXPECT_EQ(nostd::get<double>(histogram_data.sum_), 112);
  EXPECT_EQ(histogram_data.count_, 2);
  EXPECT_EQ(histogram_data.counts_[3], 1);
  EXPECT_EQ(histogram_data.counts_[7], 1);
  EXPECT_NO_THROW(aggr.Aggregate(13.0, {}));   // lies in fourth bucket
  EXPECT_NO_THROW(aggr.Aggregate(252.0, {}));  // lies in ninth bucket
  histogram_data = nostd::get<HistogramPointData>(aggr.ToPoint());
  EXPECT_EQ(histogram_data.count_, 4);
  EXPECT_EQ(histogram_data.counts_[3], 2);
  EXPECT_EQ(histogram_data.counts_[8], 1);
  EXPECT_EQ(nostd::get<double>(histogram_data.sum_), 377);

  // Merge
  DoubleHistogramAggregation aggr1;
  aggr1.Aggregate(1.0, {});
  aggr1.Aggregate(11.0, {});
  aggr1.Aggregate(25.1, {});

  DoubleHistogramAggregation aggr2;
  aggr2.Aggregate(2.0, {});
  aggr2.Aggregate(3.0, {});
  aggr2.Aggregate(13.0, {});
  aggr2.Aggregate(28.1, {});
  aggr2.Aggregate(105.0, {});

  auto aggr3     = aggr1.Merge(aggr2);
  histogram_data = nostd::get<HistogramPointData>(aggr3->ToPoint());

  EXPECT_EQ(histogram_data.count_, 8);      // 3 each from aggr1 and aggr2
  EXPECT_EQ(histogram_data.counts_[1], 3);  // 1.0, 2.0, 3.0
  EXPECT_EQ(histogram_data.counts_[3], 2);  // 11.0, 13.0
  EXPECT_EQ(histogram_data.counts_[4], 2);  // 25.1, 28.1
  EXPECT_EQ(histogram_data.counts_[7], 1);  // 105.0

  // Diff
  auto aggr4     = aggr1.Diff(aggr2);
  histogram_data = nostd::get<HistogramPointData>(aggr4->ToPoint());
  EXPECT_EQ(histogram_data.count_, 2);      // aggr2:5 - aggr1:3
  EXPECT_EQ(histogram_data.counts_[1], 1);  // aggr2(2.0, 3.0) - aggr1(1.0)
  EXPECT_EQ(histogram_data.counts_[3], 0);  // aggr2(13.0) - aggr1(11.0)
  EXPECT_EQ(histogram_data.counts_[4], 0);  // aggr2(28.1) - aggr1(25.1)
  EXPECT_EQ(histogram_data.counts_[7], 1);  // aggr2(105.0) - aggr1(0)
}
#endif
