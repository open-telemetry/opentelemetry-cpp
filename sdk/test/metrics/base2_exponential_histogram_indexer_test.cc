// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/aggregation/base2_exponential_histogram_indexer.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::metrics;

TEST(Base2ExponentialHistogramIndexerTest, ScaleOne)
{
  const Base2ExponentialHistogramIndexer indexer{1};
  auto compute_index = [indexer](double value) { return indexer.ComputeIndex(value); };

  EXPECT_EQ(compute_index(std::numeric_limits<double>::max()), 2047);
  EXPECT_EQ(compute_index(0x1p1023), 2045);
  EXPECT_EQ(compute_index(0x1.1p1023), 2046);
  EXPECT_EQ(compute_index(0x1p1022), 2043);
  EXPECT_EQ(compute_index(0x1.1p1022), 2044);
  EXPECT_EQ(compute_index(0x1p-1022), -2045);
  EXPECT_EQ(compute_index(0x1.1p-1022), -2044);
  EXPECT_EQ(compute_index(0x1p-1021), -2043);
  EXPECT_EQ(compute_index(0x1.1p-1021), -2042);
  EXPECT_EQ(compute_index(std::numeric_limits<double>::min()), -2045);
  EXPECT_EQ(compute_index(std::numeric_limits<double>::denorm_min()), -2149);
  EXPECT_EQ(compute_index(15.0), 7);
  EXPECT_EQ(compute_index(9.0), 6);
  EXPECT_EQ(compute_index(7.0), 5);
  EXPECT_EQ(compute_index(5.0), 4);
  EXPECT_EQ(compute_index(3.0), 3);
  EXPECT_EQ(compute_index(2.5), 2);
  EXPECT_EQ(compute_index(1.5), 1);
  EXPECT_EQ(compute_index(1.2), 0);
  EXPECT_EQ(compute_index(1.0), -1);
  EXPECT_EQ(compute_index(0.75), -1);
  EXPECT_EQ(compute_index(0.55), -2);
  EXPECT_EQ(compute_index(0.45), -3);
}

TEST(Base2ExponentialHistogramIndexerTest, ScaleZero)
{
  const Base2ExponentialHistogramIndexer indexer{0};
  auto compute_index = [indexer](double value) { return indexer.ComputeIndex(value); };

  // Near +Inf.
  // Use constant for exp, as max_exponent constant includes bias.
  EXPECT_EQ(compute_index(std::numeric_limits<double>::max()), 1023);
  EXPECT_EQ(compute_index(0x1p1023), 1022);
  EXPECT_EQ(compute_index(0x1.1p1023), 1023);
  EXPECT_EQ(compute_index(0x1p1022), 1021);
  EXPECT_EQ(compute_index(0x1.1p1022), 1022);
  // Near 0.
  EXPECT_EQ(compute_index(0x1p-1022), -1023);
  EXPECT_EQ(compute_index(0x1.1p-1022), -1022);
  EXPECT_EQ(compute_index(0x1p-1021), -1022);
  EXPECT_EQ(compute_index(0x1.1p-1021), -1021);
  EXPECT_EQ(compute_index(std::numeric_limits<double>::min()), -1023);
  EXPECT_EQ(compute_index(std::numeric_limits<double>::denorm_min()), -1075);
  // Near 1.
  EXPECT_EQ(compute_index(4.0), 1);
  EXPECT_EQ(compute_index(3.0), 1);
  EXPECT_EQ(compute_index(2.0), 0);
  EXPECT_EQ(compute_index(1.5), 0);
  EXPECT_EQ(compute_index(1.0), -1);
  EXPECT_EQ(compute_index(0.75), -1);
  EXPECT_EQ(compute_index(0.51), -1);
  EXPECT_EQ(compute_index(0.5), -2);
  EXPECT_EQ(compute_index(0.26), -2);
  EXPECT_EQ(compute_index(0.25), -3);
  EXPECT_EQ(compute_index(0.126), -3);
  EXPECT_EQ(compute_index(0.125), -4);
}

TEST(Base2ExponentialHistogramIndexerTest, ScaleNegativeOne)
{
  const Base2ExponentialHistogramIndexer indexer{-1};
  auto compute_index = [indexer](double value) { return indexer.ComputeIndex(value); };

  EXPECT_EQ(compute_index(17.0), 2);
  EXPECT_EQ(compute_index(16.0), 1);
  EXPECT_EQ(compute_index(15.0), 1);
  EXPECT_EQ(compute_index(9.0), 1);
  EXPECT_EQ(compute_index(8.0), 1);
  EXPECT_EQ(compute_index(5.0), 1);
  EXPECT_EQ(compute_index(4.0), 0);
  EXPECT_EQ(compute_index(3.0), 0);
  EXPECT_EQ(compute_index(2.0), 0);
  EXPECT_EQ(compute_index(1.5), 0);
  EXPECT_EQ(compute_index(1.0), -1);
  EXPECT_EQ(compute_index(0.75), -1);
  EXPECT_EQ(compute_index(0.5), -1);
  EXPECT_EQ(compute_index(0.25), -2);
  EXPECT_EQ(compute_index(0.20), -2);
  EXPECT_EQ(compute_index(0.13), -2);
  EXPECT_EQ(compute_index(0.125), -2);
  EXPECT_EQ(compute_index(0.10), -2);
  EXPECT_EQ(compute_index(0.0625), -3);
  EXPECT_EQ(compute_index(0.06), -3);
}

TEST(Base2ExponentialHistogramIndexerTest, ScaleNegativeFour)
{
  const Base2ExponentialHistogramIndexer indexer{-4};
  auto compute_index = [indexer](double value) { return indexer.ComputeIndex(value); };

  EXPECT_EQ(compute_index(0x1p0), -1);
  EXPECT_EQ(compute_index(0x10p0), 0);
  EXPECT_EQ(compute_index(0x100p0), 0);
  EXPECT_EQ(compute_index(0x1000p0), 0);
  EXPECT_EQ(compute_index(0x10000p0), 0);  // Base == 2**16
  EXPECT_EQ(compute_index(0x100000p0), 1);
  EXPECT_EQ(compute_index(0x1000000p0), 1);
  EXPECT_EQ(compute_index(0x10000000p0), 1);
  EXPECT_EQ(compute_index(0x100000000p0), 1);  // == 2**32
  EXPECT_EQ(compute_index(0x1000000000p0), 2);
  EXPECT_EQ(compute_index(0x10000000000p0), 2);
  EXPECT_EQ(compute_index(0x100000000000p0), 2);
  EXPECT_EQ(compute_index(0x1000000000000p0), 2);  // 2**48
  EXPECT_EQ(compute_index(0x10000000000000p0), 3);
  EXPECT_EQ(compute_index(0x100000000000000p0), 3);
  EXPECT_EQ(compute_index(0x1000000000000000p0), 3);
  EXPECT_EQ(compute_index(0x10000000000000000p0), 3);  // 2**64
  EXPECT_EQ(compute_index(0x100000000000000000p0), 4);
  EXPECT_EQ(compute_index(0x1000000000000000000p0), 4);
  EXPECT_EQ(compute_index(0x10000000000000000000p0), 4);
  EXPECT_EQ(compute_index(0x100000000000000000000p0), 4);
  EXPECT_EQ(compute_index(0x1000000000000000000000p0), 5);
  EXPECT_EQ(compute_index(1 / 0x1p0), -1);
  EXPECT_EQ(compute_index(1 / 0x10p0), -1);
  EXPECT_EQ(compute_index(1 / 0x100p0), -1);
  EXPECT_EQ(compute_index(1 / 0x1000p0), -1);
  EXPECT_EQ(compute_index(1 / 0x10000p0), -2);  // 2**-16
  EXPECT_EQ(compute_index(1 / 0x100000p0), -2);
  EXPECT_EQ(compute_index(1 / 0x1000000p0), -2);
  EXPECT_EQ(compute_index(1 / 0x10000000p0), -2);
  EXPECT_EQ(compute_index(1 / 0x100000000p0), -3);  // 2**-32
  EXPECT_EQ(compute_index(1 / 0x1000000000p0), -3);
  EXPECT_EQ(compute_index(1 / 0x10000000000p0), -3);
  EXPECT_EQ(compute_index(1 / 0x100000000000p0), -3);
  EXPECT_EQ(compute_index(1 / 0x1000000000000p0), -4);  // 2**-48
  EXPECT_EQ(compute_index(1 / 0x10000000000000p0), -4);
  EXPECT_EQ(compute_index(1 / 0x100000000000000p0), -4);
  EXPECT_EQ(compute_index(1 / 0x1000000000000000p0), -4);
  EXPECT_EQ(compute_index(1 / 0x10000000000000000p0), -5);  // 2**-64
  EXPECT_EQ(compute_index(1 / 0x100000000000000000p0), -5);
  // Max values.
  EXPECT_EQ(compute_index(0x1.FFFFFFFFFFFFFp1023), 63);
  EXPECT_EQ(compute_index(0x1p1023), 63);
  EXPECT_EQ(compute_index(0x1p1019), 63);
  EXPECT_EQ(compute_index(0x1p1009), 63);
  EXPECT_EQ(compute_index(0x1p1008), 62);
  EXPECT_EQ(compute_index(0x1p1007), 62);
  EXPECT_EQ(compute_index(0x1p1000), 62);
  EXPECT_EQ(compute_index(0x1p0993), 62);
  EXPECT_EQ(compute_index(0x1p0992), 61);
  EXPECT_EQ(compute_index(0x1p0991), 61);
  // Min and subnormal values.
  EXPECT_EQ(compute_index(0x1p-1074), -68);
  EXPECT_EQ(compute_index(0x1p-1073), -68);
  EXPECT_EQ(compute_index(0x1p-1072), -68);
  EXPECT_EQ(compute_index(0x1p-1057), -67);
  EXPECT_EQ(compute_index(0x1p-1056), -67);
  EXPECT_EQ(compute_index(0x1p-1041), -66);
  EXPECT_EQ(compute_index(0x1p-1040), -66);
  EXPECT_EQ(compute_index(0x1p-1025), -65);
  EXPECT_EQ(compute_index(0x1p-1024), -65);
  EXPECT_EQ(compute_index(0x1p-1023), -64);
  EXPECT_EQ(compute_index(0x1p-1022), -64);
  EXPECT_EQ(compute_index(0x1p-1009), -64);
  EXPECT_EQ(compute_index(0x1p-1008), -64);
  EXPECT_EQ(compute_index(0x1p-1007), -63);
  EXPECT_EQ(compute_index(0x1p-0993), -63);
  EXPECT_EQ(compute_index(0x1p-0992), -63);
  EXPECT_EQ(compute_index(0x1p-0991), -62);
  EXPECT_EQ(compute_index(0x1p-0977), -62);
  EXPECT_EQ(compute_index(0x1p-0976), -62);
  EXPECT_EQ(compute_index(0x1p-0975), -61);
}
