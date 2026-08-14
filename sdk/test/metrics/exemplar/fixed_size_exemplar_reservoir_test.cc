// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <gtest/gtest.h>
#  include <stdint.h>
#  include <atomic>
#  include <cstddef>
#  include <memory>
#  include <string>
#  include <thread>
#  include <vector>

#  include "opentelemetry/context/context.h"
#  include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#  include "opentelemetry/sdk/metrics/exemplar/aligned_histogram_bucket_exemplar_reservoir.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir_cell.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir_cell_selector.h"
#  include "opentelemetry/sdk/metrics/exemplar/simple_fixed_size_exemplar_reservoir.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class SimpleFixedSizeCellSelectorTestPeer
{
public:
  static size_t GetMeasurementsSeen(
      const SimpleFixedSizeExemplarReservoir::SimpleFixedSizeCellSelector &selector)
  {
    return selector.measurements_seen_;
  }
};

namespace
{

TEST(FixedSizeExemplarReservoirTest, CollectAndResetSupportsMultipleIntervals)
{
  std::vector<double> boundaries{1.0, 5.0, 10.0};
  auto reservoir = ExemplarReservoir::GetAlignedHistogramBucketExemplarReservoir(
      boundaries.size(),
      AlignedHistogramBucketExemplarReservoir::GetHistogramCellSelector(boundaries),
      &ReservoirCell::GetAndResetDouble);

  reservoir->OfferMeasurement(2.0, MetricAttributes{}, opentelemetry::context::Context{});
  auto first_interval = reservoir->CollectAndReset(MetricAttributes{});
  ASSERT_EQ(first_interval.size(), 1U);
  EXPECT_NE(first_interval[0], nullptr);

  // Collecting again without another offer verifies that the cells in storage,
  // rather than copies of those cells, were reset.
  EXPECT_TRUE(reservoir->CollectAndReset(MetricAttributes{}).empty());

  // A collection resets the selector state without destroying the selector, so
  // the same reservoir remains usable in the next interval.
  reservoir->OfferMeasurement(8.0, MetricAttributes{}, opentelemetry::context::Context{});
  auto second_interval = reservoir->CollectAndReset(MetricAttributes{});
  ASSERT_EQ(second_interval.size(), 1U);
  EXPECT_NE(second_interval[0], nullptr);
}

TEST(FixedSizeExemplarReservoirTest, SimpleReservoirRestartsSamplingEachInterval)
{
  auto selector =
      std::make_shared<SimpleFixedSizeExemplarReservoir::SimpleFixedSizeCellSelector>(1);
  auto reservoir = ExemplarReservoir::GetSimpleFixedSizeExemplarReservoir(
      1, selector, &ReservoirCell::GetAndResetLong);

  reservoir->OfferMeasurement(static_cast<int64_t>(1), MetricAttributes{},
                              opentelemetry::context::Context{});
  ASSERT_EQ(SimpleFixedSizeCellSelectorTestPeer::GetMeasurementsSeen(*selector), 1U);

  auto exemplars = reservoir->CollectAndReset(MetricAttributes{});
  ASSERT_EQ(exemplars.size(), 1U);
  EXPECT_NE(exemplars[0], nullptr);
  EXPECT_EQ(SimpleFixedSizeCellSelectorTestPeer::GetMeasurementsSeen(*selector), 0U);
}

class ConcurrentAccessDetector final : public ReservoirCellSelector
{
public:
  int ReservoirCellIndexFor(const std::vector<ReservoirCell> & /* cells */,
                            int64_t /* value */,
                            const MetricAttributes & /* attributes */,
                            const opentelemetry::context::Context & /* context */) override
  {
    Visit();
    return 0;
  }

  int ReservoirCellIndexFor(const std::vector<ReservoirCell> & /* cells */,
                            double /* value */,
                            const MetricAttributes & /* attributes */,
                            const opentelemetry::context::Context & /* context */) override
  {
    Visit();
    return 0;
  }

  void reset() override { Visit(); }

  bool HasConcurrentAccess() const noexcept
  {
    return concurrent_access_.load(std::memory_order_relaxed);
  }

private:
  void Visit()
  {
    if (active_calls_.fetch_add(1, std::memory_order_acq_rel) != 0)
    {
      concurrent_access_.store(true, std::memory_order_relaxed);
    }
    std::this_thread::yield();
    active_calls_.fetch_sub(1, std::memory_order_release);
  }

  std::atomic<int> active_calls_{0};
  std::atomic<bool> concurrent_access_{false};
};

TEST(FixedSizeExemplarReservoirTest, SerializesOffersAndCollection)
{
  auto selector  = std::make_shared<ConcurrentAccessDetector>();
  auto reservoir = ExemplarReservoir::GetSimpleFixedSizeExemplarReservoir(
      1, selector, &ReservoirCell::GetAndResetLong);
  std::atomic<bool> start{false};

  constexpr size_t kOfferThreadCount = 3;
  constexpr size_t kIterations       = 500;
  std::vector<std::thread> threads;
  threads.reserve(kOfferThreadCount + 1);
  for (size_t thread_index = 0; thread_index < kOfferThreadCount; ++thread_index)
  {
    threads.emplace_back([&] {
      while (!start.load(std::memory_order_acquire))
      {
        std::this_thread::yield();
      }
      for (size_t i = 0; i < kIterations; ++i)
      {
        reservoir->OfferMeasurement(static_cast<int64_t>(i), MetricAttributes{},
                                    opentelemetry::context::Context{});
      }
    });
  }
  threads.emplace_back([&] {
    while (!start.load(std::memory_order_acquire))
    {
      std::this_thread::yield();
    }
    for (size_t i = 0; i < kIterations; ++i)
    {
      reservoir->CollectAndReset(MetricAttributes{});
    }
  });

  start.store(true, std::memory_order_release);
  for (auto &thread : threads)
  {
    thread.join();
  }

  EXPECT_FALSE(selector->HasConcurrentAccess());
}

}  // namespace
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW
