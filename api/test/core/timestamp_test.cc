#include "opentelemetry/core/timestamp.h"

#include <gtest/gtest.h>

using opentelemetry::core::Timestamp;

template <class Timestamp>
static bool AreNearlyEqual(const Timestamp &t1, const Timestamp &t2) noexcept
{
  return std::abs(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) < 2;
}

TEST(TimestampTest, Construction)
{
  auto now_system = std::chrono::system_clock::now();
  auto now_steady = std::chrono::steady_clock::now();

  Timestamp t1;
  EXPECT_FALSE(t1.HasSystemTimestamp());
  EXPECT_FALSE(t1.HasSteadyTimestamp());

  Timestamp t2{now_system};
  EXPECT_TRUE(t2.HasSystemTimestamp());
  EXPECT_FALSE(t2.HasSteadyTimestamp());
  EXPECT_TRUE(AreNearlyEqual(now_system, t2.system_timestamp()));

  Timestamp t3{now_steady};
  EXPECT_FALSE(t3.HasSystemTimestamp());
  EXPECT_TRUE(t3.HasSteadyTimestamp());
  EXPECT_TRUE(AreNearlyEqual(now_steady, t3.steady_timestamp()));

  Timestamp t4{now_system, now_steady};
  EXPECT_TRUE(t4.HasSystemTimestamp());
  EXPECT_TRUE(t4.HasSteadyTimestamp());
  EXPECT_TRUE(AreNearlyEqual(now_system, t4.system_timestamp()));
  EXPECT_TRUE(AreNearlyEqual(now_steady, t4.steady_timestamp()));
}
