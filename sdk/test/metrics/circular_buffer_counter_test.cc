// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stddef.h>
#include <cstdint>
#include <limits>
#include <string>

#include "opentelemetry/sdk/metrics/data/circular_buffer.h"

using namespace opentelemetry::sdk::metrics;

namespace
{

// The allocate-and-copy fold that AdaptingCircularBufferCounter::Downscale()
// replaced. Kept here as an oracle: it is obviously correct because it never
// writes into the buffer it reads from.
AdaptingCircularBufferCounter ReferenceDownscale(const AdaptingCircularBufferCounter &counter,
                                                 uint32_t by)
{
  AdaptingCircularBufferCounter folded{counter.MaxSize()};
  if (counter.Empty())
  {
    return folded;
  }

  for (int32_t index = counter.StartIndex(); index <= counter.EndIndex(); ++index)
  {
    const uint64_t count = counter.Get(index);
    if (count > 0)
    {
      EXPECT_TRUE(folded.Increment(index >> by, count));
    }
  }
  return folded;
}

void ExpectSameBuckets(const AdaptingCircularBufferCounter &expected,
                       const AdaptingCircularBufferCounter &actual)
{
  ASSERT_EQ(expected.Empty(), actual.Empty());
  EXPECT_EQ(expected.MaxSize(), actual.MaxSize());
  if (expected.Empty())
  {
    return;
  }

  ASSERT_EQ(expected.StartIndex(), actual.StartIndex());
  ASSERT_EQ(expected.EndIndex(), actual.EndIndex());
  for (int32_t index = expected.StartIndex(); index <= expected.EndIndex(); ++index)
  {
    EXPECT_EQ(expected.Get(index), actual.Get(index)) << "bucket " << index;
  }
}

uint64_t TotalCount(const AdaptingCircularBufferCounter &counter)
{
  if (counter.Empty())
  {
    return 0;
  }

  uint64_t total = 0;
  for (int32_t index = counter.StartIndex(); index <= counter.EndIndex(); ++index)
  {
    total += counter.Get(index);
  }
  return total;
}

class AdaptingIntegerArrayTest : public testing::TestWithParam<uint64_t>
{};

INSTANTIATE_TEST_SUITE_P(InterestingValues,
                         AdaptingIntegerArrayTest,
                         testing::Values<uint64_t>(1,
                                                   std::numeric_limits<uint8_t>::max() + 1ull,
                                                   std::numeric_limits<uint16_t>::max() + 1ull,
                                                   std::numeric_limits<uint32_t>::max() + 1ull));

TEST_P(AdaptingIntegerArrayTest, PreservesSizeOnEnlargement)
{
  AdaptingIntegerArray counter(10);
  EXPECT_EQ(counter.Size(), 10);
  counter.Increment(0, GetParam());
  EXPECT_EQ(counter.Size(), 10);
}

TEST_P(AdaptingIntegerArrayTest, IncrementAndGet)
{
  AdaptingIntegerArray counter(10);
  for (int idx = 0; idx < 10; idx += 1)
  {
    EXPECT_EQ(counter.Get(idx), 0);
    counter.Increment(idx, 1);
    EXPECT_EQ(counter.Get(idx), 1);
    counter.Increment(idx, GetParam());
    EXPECT_EQ(counter.Get(idx), GetParam() + 1);
  }
}

TEST_P(AdaptingIntegerArrayTest, Copy)
{
  AdaptingIntegerArray counter(10);
  counter.Increment(0, GetParam());
  EXPECT_EQ(counter.Get(0), GetParam());

  AdaptingIntegerArray copy = counter;
  EXPECT_EQ(copy.Get(0), GetParam());

  counter.Increment(0, 1);
  EXPECT_EQ(counter.Get(0), GetParam() + 1);
  EXPECT_EQ(copy.Get(0), GetParam());
}

TEST_P(AdaptingIntegerArrayTest, Clear)
{
  AdaptingIntegerArray counter(10);
  counter.Increment(0, GetParam());
  EXPECT_EQ(counter.Get(0), GetParam());

  counter.Clear();
  counter.Increment(0, 1);
  EXPECT_EQ(counter.Get(0), 1);
}

TEST_P(AdaptingIntegerArrayTest, Fold)
{
  AdaptingIntegerArray counter(10);
  counter.Increment(3, GetParam());
  counter.Increment(4, GetParam());
  counter.Increment(5, 1);

  counter.Fold(3, 4);
  EXPECT_EQ(counter.Get(3), 0);
  EXPECT_EQ(counter.Get(4), 2 * GetParam());
  // Unrelated cells and the array size are left alone.
  EXPECT_EQ(counter.Get(5), 1);
  EXPECT_EQ(counter.Size(), 10);

  // Folding an empty cell, and folding a cell into itself, are both no-ops.
  counter.Fold(3, 5);
  counter.Fold(4, 4);
  EXPECT_EQ(counter.Get(4), 2 * GetParam());
  EXPECT_EQ(counter.Get(5), 1);

  // The widened cells stay usable.
  counter.Increment(3, GetParam());
  EXPECT_EQ(counter.Get(3), GetParam());
}

TEST(AdaptingIntegerArrayWidthTest, FoldWidensCellsWhenTheMergedValueDoesNotFit)
{
  // One case per cell width boundary: the merged value overflows the current
  // width, so Fold has to enlarge the array and retry.
  const uint64_t sources[] = {200, 40000, 3000000000ull};
  for (uint64_t source : sources)
  {
    AdaptingIntegerArray counter(4);
    counter.Increment(1, source);
    counter.Increment(2, source);

    counter.Fold(1, 2);

    EXPECT_EQ(counter.Get(1), 0);
    EXPECT_EQ(counter.Get(2), 2 * source);
    EXPECT_EQ(counter.Size(), 4);
  }
}

TEST(AdaptingCircularBufferCounterTest, ReturnsZeroOutsidePopulatedRange)
{
  AdaptingCircularBufferCounter counter{10};
  EXPECT_EQ(counter.Get(0), 0);
  EXPECT_EQ(counter.Get(100), 0);
  counter.Increment(2, 1);
  counter.Increment(99, 1);
  EXPECT_EQ(counter.Get(0), 0);
  EXPECT_EQ(counter.Get(100), 0);
}

TEST(AdaptingCircularBufferCounterTest, ExpandLower)
{
  AdaptingCircularBufferCounter counter{160};
  EXPECT_TRUE(counter.Increment(10, 1));
  // Add BEFORE the initial see (array index 0) and make sure we wrap around the datastructure.
  EXPECT_TRUE(counter.Increment(0, 1));
  EXPECT_EQ(counter.Get(10), 1);
  EXPECT_EQ(counter.Get(0), 1);
  EXPECT_EQ(counter.StartIndex(), 0);
  EXPECT_EQ(counter.EndIndex(), 10);
  // Add AFTER initial entry and just push back end.
  EXPECT_TRUE(counter.Increment(20, 1));
  EXPECT_EQ(counter.Get(20), 1);
  EXPECT_EQ(counter.Get(10), 1);
  EXPECT_EQ(counter.Get(0), 1);
  EXPECT_EQ(counter.StartIndex(), 0);
  EXPECT_EQ(counter.EndIndex(), 20);
}

TEST(AdaptingCircularBufferCounterTest, ShouldFailAtLimit)
{
  AdaptingCircularBufferCounter counter{10};
  EXPECT_TRUE(counter.Increment(10, 1));
  EXPECT_TRUE(counter.Increment(15, 2));
  EXPECT_TRUE(counter.Increment(6, 3));
  // Check state
  EXPECT_EQ(counter.StartIndex(), 6);
  EXPECT_EQ(counter.EndIndex(), 15);
  EXPECT_EQ(counter.Get(6), 3);
  EXPECT_EQ(counter.Get(10), 1);
  EXPECT_EQ(counter.Get(15), 2);
  // Adding over the maximum # of buckets
  EXPECT_FALSE(counter.Increment(5, 1));
  EXPECT_FALSE(counter.Increment(16, 1));
}

TEST(AdaptingCircularBufferCounterTest, ShouldCopyCounters)
{
  AdaptingCircularBufferCounter counter{2};
  EXPECT_TRUE(counter.Increment(2, 1));
  EXPECT_TRUE(counter.Increment(1, 1));
  EXPECT_FALSE(counter.Increment(3, 1));

  AdaptingCircularBufferCounter copy{counter};
  EXPECT_EQ(counter.Get(2), 1);
  EXPECT_EQ(copy.Get(2), 1);
  EXPECT_EQ(copy.MaxSize(), counter.MaxSize());
  EXPECT_EQ(copy.StartIndex(), counter.StartIndex());
  EXPECT_EQ(copy.EndIndex(), counter.EndIndex());
  // Mutate copy and make sure original is unchanged.
  EXPECT_TRUE(copy.Increment(2, 1));
  EXPECT_EQ(copy.Get(2), 2);
  EXPECT_EQ(counter.Get(2), 1);
}

TEST(AdaptingCircularBufferCounterTest, Clear)
{
  AdaptingCircularBufferCounter counter{10};
  EXPECT_TRUE(counter.Empty());
  EXPECT_TRUE(counter.Increment(2, 1));
  EXPECT_FALSE(counter.Empty());
  EXPECT_TRUE(counter.Increment(8, 1));
  // Check state.
  EXPECT_EQ(counter.StartIndex(), 2);
  EXPECT_EQ(counter.EndIndex(), 8);
  EXPECT_EQ(counter.Get(2), 1);
  EXPECT_EQ(counter.Get(8), 1);
  // Clear and verify.
  EXPECT_FALSE(counter.Empty());
  counter.Clear();
  EXPECT_TRUE(counter.Empty());
}

TEST(AdaptingCircularBufferCounterTest, DownscaleIsANoOpWhenEmptyOrByZero)
{
  AdaptingCircularBufferCounter empty{10};
  empty.Downscale(3);
  EXPECT_TRUE(empty.Empty());

  AdaptingCircularBufferCounter counter{10};
  EXPECT_TRUE(counter.Increment(3, 5));
  EXPECT_TRUE(counter.Increment(9, 7));
  counter.Downscale(0);
  EXPECT_EQ(counter.StartIndex(), 3);
  EXPECT_EQ(counter.EndIndex(), 9);
  EXPECT_EQ(counter.Get(3), 5);
  EXPECT_EQ(counter.Get(9), 7);
}

TEST(AdaptingCircularBufferCounterTest, DownscaleWithoutWraparound)
{
  AdaptingCircularBufferCounter counter{10};
  EXPECT_TRUE(counter.Increment(4, 1));
  EXPECT_TRUE(counter.Increment(5, 2));
  EXPECT_TRUE(counter.Increment(6, 3));
  EXPECT_TRUE(counter.Increment(7, 4));

  counter.Downscale(1);

  EXPECT_EQ(counter.StartIndex(), 2);
  EXPECT_EQ(counter.EndIndex(), 3);
  EXPECT_EQ(counter.Get(2), 3);
  EXPECT_EQ(counter.Get(3), 7);
  EXPECT_EQ(counter.MaxSize(), 10);
  EXPECT_EQ(TotalCount(counter), 10);
}

TEST(AdaptingCircularBufferCounterTest, DownscaleWithWraparound)
{
  AdaptingCircularBufferCounter counter{8};
  // The first recording fixes the base index, so 2..4 land at the tail of the
  // backing array while 5..6 stay at its front.
  EXPECT_TRUE(counter.Increment(5, 1));
  EXPECT_TRUE(counter.Increment(6, 2));
  EXPECT_TRUE(counter.Increment(2, 4));
  EXPECT_TRUE(counter.Increment(3, 8));
  EXPECT_TRUE(counter.Increment(4, 16));

  counter.Downscale(1);

  EXPECT_EQ(counter.StartIndex(), 1);
  EXPECT_EQ(counter.EndIndex(), 3);
  EXPECT_EQ(counter.Get(1), 12);  // 2 and 3
  EXPECT_EQ(counter.Get(2), 17);  // 4 and 5
  EXPECT_EQ(counter.Get(3), 2);   // 6
  EXPECT_EQ(TotalCount(counter), 31);
}

TEST(AdaptingCircularBufferCounterTest, DownscaleAcrossZero)
{
  AdaptingCircularBufferCounter counter{10};
  EXPECT_TRUE(counter.Increment(1, 1));
  EXPECT_TRUE(counter.Increment(-1, 2));
  EXPECT_TRUE(counter.Increment(-2, 4));
  EXPECT_TRUE(counter.Increment(0, 8));
  EXPECT_TRUE(counter.Increment(2, 16));

  counter.Downscale(1);

  // Right shifting a negative index rounds towards minus infinity, so -2 and -1
  // both fold into -1.
  EXPECT_EQ(counter.StartIndex(), -1);
  EXPECT_EQ(counter.EndIndex(), 1);
  EXPECT_EQ(counter.Get(-1), 6);
  EXPECT_EQ(counter.Get(0), 9);
  EXPECT_EQ(counter.Get(1), 16);
  EXPECT_EQ(TotalCount(counter), 31);
}

TEST(AdaptingCircularBufferCounterTest, DownscaleAtMinimumIndex)
{
  constexpr int32_t kMinIndex = (std::numeric_limits<int32_t>::min)();
  AdaptingCircularBufferCounter counter{2};
  EXPECT_TRUE(counter.Increment(kMinIndex + 1, 1));
  EXPECT_TRUE(counter.Increment(kMinIndex, 1));

  counter.Downscale(1);

  EXPECT_EQ(counter.StartIndex(), kMinIndex >> 1);
  EXPECT_EQ(counter.EndIndex(), kMinIndex >> 1);
  EXPECT_EQ(counter.Get(kMinIndex >> 1), 2);
}

TEST(AdaptingCircularBufferCounterTest, DownscaleAtMaximumIndex)
{
  constexpr int32_t kMaxIndex = (std::numeric_limits<int32_t>::max)();
  AdaptingCircularBufferCounter counter{2};
  EXPECT_TRUE(counter.Increment(kMaxIndex - 1, 1));
  EXPECT_TRUE(counter.Increment(kMaxIndex, 1));

  counter.Downscale(1);

  EXPECT_EQ(counter.StartIndex(), kMaxIndex >> 1);
  EXPECT_EQ(counter.EndIndex(), kMaxIndex >> 1);
  EXPECT_EQ(counter.Get(kMaxIndex >> 1), 2);

  AdaptingCircularBufferCounter reverse_order{2};
  EXPECT_TRUE(reverse_order.Increment(kMaxIndex, 1));
  EXPECT_TRUE(reverse_order.Increment(kMaxIndex - 1, 1));

  reverse_order.Downscale(1);

  EXPECT_EQ(reverse_order.StartIndex(), kMaxIndex >> 1);
  EXPECT_EQ(reverse_order.EndIndex(), kMaxIndex >> 1);
  EXPECT_EQ(reverse_order.Get(kMaxIndex >> 1), 2);
}

TEST(AdaptingCircularBufferCounterTest, DownscaleFullBuffer)
{
  constexpr int32_t kSize = 16;
  AdaptingCircularBufferCounter counter{kSize};
  // Seed in the middle so the populated range spans the whole backing array and
  // wraps around it.
  EXPECT_TRUE(counter.Increment(8, 9));
  for (int32_t index = 0; index < kSize; ++index)
  {
    if (index != 8)
    {
      EXPECT_TRUE(counter.Increment(index, static_cast<uint64_t>(index) + 1));
    }
  }
  ASSERT_EQ(counter.StartIndex(), 0);
  ASSERT_EQ(counter.EndIndex(), kSize - 1);

  const AdaptingCircularBufferCounter expected = ReferenceDownscale(counter, 2);
  counter.Downscale(2);

  ExpectSameBuckets(expected, counter);
  EXPECT_EQ(counter.StartIndex(), 0);
  EXPECT_EQ(counter.EndIndex(), 3);
  EXPECT_EQ(counter.Get(0), 10);
  EXPECT_EQ(counter.Get(1), 26);
  EXPECT_EQ(counter.Get(2), 42);
  EXPECT_EQ(counter.Get(3), 58);
  EXPECT_EQ(TotalCount(counter), 136);
}

TEST(AdaptingCircularBufferCounterTest, DownscaleWidensCellsWhenBucketsMerge)
{
  const uint64_t deltas[] = {200, std::numeric_limits<uint8_t>::max() + 1ull,
                             std::numeric_limits<uint16_t>::max() + 1ull,
                             std::numeric_limits<uint32_t>::max() + 1ull};
  for (uint64_t delta : deltas)
  {
    AdaptingCircularBufferCounter counter{4};
    EXPECT_TRUE(counter.Increment(0, delta));
    EXPECT_TRUE(counter.Increment(1, delta));

    counter.Downscale(1);

    EXPECT_EQ(counter.StartIndex(), 0);
    EXPECT_EQ(counter.EndIndex(), 0);
    EXPECT_EQ(counter.Get(0), 2 * delta);
    EXPECT_EQ(counter.MaxSize(), 4);
  }
}

TEST(AdaptingCircularBufferCounterTest, DownscaleCollapsesToASingleBucket)
{
  AdaptingCircularBufferCounter counter{10};
  EXPECT_TRUE(counter.Increment(4, 1));
  EXPECT_TRUE(counter.Increment(-4, 2));
  EXPECT_TRUE(counter.Increment(0, 3));

  counter.Downscale(31);

  EXPECT_EQ(counter.StartIndex(), -1);
  EXPECT_EQ(counter.EndIndex(), 0);
  EXPECT_EQ(counter.Get(-1), 2);
  EXPECT_EQ(counter.Get(0), 4);

  // The counter stays usable afterwards.
  EXPECT_TRUE(counter.Increment(0, 5));
  EXPECT_EQ(counter.Get(0), 9);
  EXPECT_EQ(TotalCount(counter), 11);
}

TEST(AdaptingCircularBufferCounterTest, DownscaleSaturatesShiftAtIndexWidth)
{
  AdaptingCircularBufferCounter counter{2};
  EXPECT_TRUE(counter.Increment(0, 1));
  EXPECT_TRUE(counter.Increment(1, 1));

  counter.Downscale(32);

  EXPECT_EQ(counter.StartIndex(), 0);
  EXPECT_EQ(counter.EndIndex(), 0);
  EXPECT_EQ(counter.Get(0), 2);
}

TEST(AdaptingCircularBufferCounterTest, DownscaleSaturatesArbitrarilyLargeShift)
{
  AdaptingCircularBufferCounter counter{4};
  EXPECT_TRUE(counter.Increment(1, 1));
  EXPECT_TRUE(counter.Increment(-1, 2));
  EXPECT_TRUE(counter.Increment(0, 4));

  counter.Downscale((std::numeric_limits<uint32_t>::max)());

  EXPECT_EQ(counter.StartIndex(), -1);
  EXPECT_EQ(counter.EndIndex(), 0);
  EXPECT_EQ(counter.Get(-1), 2);
  EXPECT_EQ(counter.Get(0), 5);
}

TEST(AdaptingCircularBufferCounterTest, DownscaleMatchesReferenceImplementation)
{
  // A plain linear congruential generator keeps the case set reproducible
  // without pulling in <random>.
  uint32_t state = 0x9e3779b9u;
  auto next      = [&state](uint32_t bound) {
    state = (state * 1664525u) + 1013904223u;
    return (state >> 8) % bound;
  };

  const uint64_t magnitudes[] = {1, 250, 65000, 4000000000ull};

  for (uint32_t trial = 0; trial < 2000; ++trial)
  {
    const size_t max_size = 2 + next(30);
    const int32_t base    = static_cast<int32_t>(next(41)) - 20;

    AdaptingCircularBufferCounter counter{max_size};
    // The first recording fixes the base index and therefore how the populated
    // range wraps around the backing array.
    EXPECT_TRUE(counter.Increment(base, magnitudes[next(4)] + next(97)));
    for (size_t i = 0; i < 4 * max_size; ++i)
    {
      const int32_t index = base + static_cast<int32_t>(next(2 * static_cast<uint32_t>(max_size))) -
                            static_cast<int32_t>(max_size);
      // Out-of-range recordings are rejected, which is part of what is exercised.
      counter.Increment(index, magnitudes[next(4)] + next(97));
    }

    const uint32_t by                            = 1 + next(4);
    const AdaptingCircularBufferCounter expected = ReferenceDownscale(counter, by);
    const uint64_t total                         = TotalCount(counter);

    counter.Downscale(by);

    ExpectSameBuckets(expected, counter);
    EXPECT_EQ(TotalCount(counter), total)
        << "trial " << trial << " max_size " << max_size << " by " << by;
  }
}

}  // namespace
