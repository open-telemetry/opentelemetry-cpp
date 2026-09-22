// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "common.h"

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/metrics/cardinality_limits.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/sdk/metrics/state/metric_collector.h"

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::metrics;

TEST(MetricReaderTest, BasicTests)
{
  std::unique_ptr<MetricReader> metric_reader1(new MockMetricReader());
  EXPECT_EQ(metric_reader1->GetAggregationTemporality(InstrumentType::kCounter),
            AggregationTemporality::kCumulative);

  std::shared_ptr<MeterContext> meter_context1(new MeterContext());
  meter_context1->AddMetricReader(std::move(metric_reader1));

  std::unique_ptr<MetricReader> metric_reader2(new MockMetricReader());
  std::shared_ptr<MeterContext> meter_context2(new MeterContext());
  std::shared_ptr<MetricProducer> metric_producer{
      new MetricCollector(meter_context2.get(), std::move(metric_reader2))};
  metric_producer->Produce();
}

TEST(MetricReaderTest, CardinalityLimitsDefaults)
{
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());

  // Default values are kAggregationCardinalityLimit (2000) for all instrument types
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kCounter),
            kAggregationCardinalityLimit);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kHistogram),
            kAggregationCardinalityLimit);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kUpDownCounter),
            kAggregationCardinalityLimit);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableCounter),
            kAggregationCardinalityLimit);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableGauge),
            kAggregationCardinalityLimit);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableUpDownCounter),
            kAggregationCardinalityLimit);
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kGauge),
            kAggregationCardinalityLimit);
#endif
}

TEST(MetricReaderTest, CardinalityLimitsPerInstrumentType)
{
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());

  // Override every field
  CardinalityLimits limits;
  limits.default_limit              = 1000;
  limits.counter                    = 100;
  limits.histogram                  = 200;
  limits.up_down_counter            = 300;
  limits.observable_counter         = 400;
  limits.observable_gauge           = 500;
  limits.observable_up_down_counter = 600;
  limits.gauge                      = 700;

  metric_reader->SetCardinalityLimits(limits);

  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kCounter), 100);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kHistogram), 200);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kUpDownCounter), 300);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableCounter), 400);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableGauge), 500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableUpDownCounter), 600);
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kGauge), 700);
#endif
}

TEST(MetricReaderTest, CardinalityLimitsOverrideDefaultLimit)
{
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());

  // Explicitly set all fields — callers are responsible for resolving
  // default_limit into per-instrument fields before calling SetCardinalityLimits.
  // sdk_builder.cc handles this resolution for YAML config (where 0 = unset).
  CardinalityLimits limits;
  limits.default_limit              = 1500;
  limits.counter                    = 100;
  limits.histogram                  = 200;
  limits.up_down_counter            = 1500;  // explicitly set to default_limit
  limits.observable_counter         = 1500;
  limits.observable_gauge           = 1500;
  limits.observable_up_down_counter = 1500;
  limits.gauge                      = 1500;

  metric_reader->SetCardinalityLimits(limits);

  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kCounter), 100);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kHistogram), 200);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kUpDownCounter), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableCounter), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableGauge), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableUpDownCounter), 1500);
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kGauge), 1500);
#endif
}

// Verify that a user-configured default_limit of 1500 with an explicit
// per-instrument override of 2000 (the SDK default) is honoured as written.
// The value 2000 must be treated as an explicit user choice, not collapsed
// back to the default_limit.  This matters when the YAML config uses
// CardinalityLimitsConfiguration where 0 means "unset", but any non-zero
// value (including 2000) is an intentional override.
TEST(MetricReaderTest, CardinalityLimitsExplicitSdkDefaultIsHonoured)
{
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());

  // Simulate: user sets default=1500, counter=2000 (explicit, not "unset")
  CardinalityLimits limits;
  limits.default_limit = 1500;
  limits.counter       = 2000;  // explicitly set to the SDK default value
  // all other fields explicitly set to default_limit (1500)
  limits.histogram                  = 1500;
  limits.up_down_counter            = 1500;
  limits.observable_counter         = 1500;
  limits.observable_gauge           = 1500;
  limits.observable_up_down_counter = 1500;
  limits.gauge                      = 1500;

  metric_reader->SetCardinalityLimits(limits);

  // counter was explicitly set to 2000 — must be returned as 2000
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kCounter), 2000);
  // histogram was not explicitly set — falls back to default_limit 1500
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kHistogram), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kUpDownCounter), 1500);
}

namespace
{

class CountingMetricReader : public MetricReader
{
public:
  // hook_result is what both OnForceFlush() and OnShutDown() report back.
  explicit CountingMetricReader(bool hook_result = true) : hook_result_(hook_result) {}

  AggregationTemporality GetAggregationTemporality(InstrumentType) const noexcept override
  {
    return AggregationTemporality::kCumulative;
  }

  std::atomic<int> shutdown_count{0};
  std::atomic<int> force_flush_count{0};

private:
  bool OnForceFlush(std::chrono::microseconds) noexcept override
  {
    ++force_flush_count;
    return hook_result_;
  }

  bool OnShutDown(std::chrono::microseconds) noexcept override
  {
    ++shutdown_count;
    return hook_result_;
  }

  const bool hook_result_;
};

}  // namespace

TEST(MetricReaderTest, ShutdownIsInvokedOnce)
{
  CountingMetricReader reader;

  EXPECT_TRUE(reader.Shutdown());
  EXPECT_TRUE(reader.IsShutdown());
  EXPECT_EQ(reader.shutdown_count.load(), 1);

  EXPECT_TRUE(reader.Shutdown());
  EXPECT_TRUE(reader.Shutdown());
  EXPECT_EQ(reader.shutdown_count.load(), 1);
}

TEST(MetricReaderTest, ConcurrentShutdownIsInvokedOnce)
{
  namespace internal_log = opentelemetry::sdk::common::internal_log;
  CountingMetricReader reader;

  // default logger is not thread-safe
  auto previous_handler = internal_log::GlobalLogHandler::GetLogHandler();
  internal_log::GlobalLogHandler::SetLogHandler(
      nostd::shared_ptr<internal_log::LogHandler>(new internal_log::NoopLogHandler()));

  std::vector<std::thread> threads;
  threads.reserve(8);
  for (int i = 0; i < 8; i++)
  {
    threads.emplace_back([&reader]() { reader.Shutdown(); });
  }
  for (auto &thread : threads)
  {
    thread.join();
  }

  internal_log::GlobalLogHandler::SetLogHandler(previous_handler);

  EXPECT_EQ(reader.shutdown_count.load(), 1);
}

TEST(MetricReaderTest, ForceFlushAfterShutdownIsNoOp)
{
  CountingMetricReader reader;

  EXPECT_TRUE(reader.ForceFlush());
  EXPECT_EQ(reader.force_flush_count.load(), 1);

  EXPECT_TRUE(reader.Shutdown());

  EXPECT_FALSE(reader.ForceFlush());
  EXPECT_EQ(reader.force_flush_count.load(), 1);
}

TEST(MetricReaderTest, FailedShutdownIsReportedAndNotRetried)
{
  CountingMetricReader reader{/* hook_result= */ false};

  EXPECT_FALSE(reader.ForceFlush());
  EXPECT_EQ(reader.force_flush_count.load(), 1);

  // The first call reports the hook's failure.
  EXPECT_FALSE(reader.Shutdown());
  EXPECT_TRUE(reader.IsShutdown());
  EXPECT_EQ(reader.shutdown_count.load(), 1);

  // A later call is a no-op that succeeds, without re-entering the failed hook.
  EXPECT_TRUE(reader.Shutdown());
  EXPECT_EQ(reader.shutdown_count.load(), 1);

  // Flush stays rejected even after a failed shutdown, without re-entering the hook.
  EXPECT_FALSE(reader.ForceFlush());
  EXPECT_EQ(reader.force_flush_count.load(), 1);
}

namespace
{

// Parks inside OnShutDown() until released, to observe what a concurrent caller sees.
class BlockingMetricReader : public MetricReader
{
public:
  AggregationTemporality GetAggregationTemporality(InstrumentType) const noexcept override
  {
    return AggregationTemporality::kCumulative;
  }

  std::promise<void> entered_shutdown;
  std::promise<void> release_shutdown;
  std::atomic<bool> shutdown_finished{false};

private:
  bool OnForceFlush(std::chrono::microseconds) noexcept override { return true; }

  bool OnShutDown(std::chrono::microseconds) noexcept override
  {
    entered_shutdown.set_value();
    release_shutdown.get_future().wait();
    shutdown_finished.store(true, std::memory_order_release);
    return true;
  }
};

}  // namespace

TEST(MetricReaderTest, ConcurrentShutdownWaitsForCleanupToComplete)
{
  namespace internal_log = opentelemetry::sdk::common::internal_log;
  BlockingMetricReader reader;

  // default logger is not thread-safe
  auto previous_handler = internal_log::GlobalLogHandler::GetLogHandler();
  internal_log::GlobalLogHandler::SetLogHandler(
      nostd::shared_ptr<internal_log::LogHandler>(new internal_log::NoopLogHandler()));

  auto entered = reader.entered_shutdown.get_future();
  std::thread first([&reader]() { EXPECT_TRUE(reader.Shutdown()); });

  // The first caller owns the shutdown and is now parked inside OnShutDown().
  entered.wait();
  EXPECT_TRUE(reader.IsShutdown());
  EXPECT_FALSE(reader.shutdown_finished.load(std::memory_order_acquire));

  std::atomic<bool> second_returned{false};
  std::promise<void> second_started;
  auto started = second_started.get_future();
  std::thread second([&]() {
    second_started.set_value();
    // Block until first caller releases the shutdown, then return true.
    EXPECT_TRUE(reader.Shutdown());
    EXPECT_TRUE(reader.shutdown_finished.load(std::memory_order_acquire));
    second_returned.store(true, std::memory_order_release);
  });

  started.wait();
  // Arbitrary sleep to ensure second is still blocked and not just we were too fast to check.
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_FALSE(second_returned.load(std::memory_order_acquire));

  reader.release_shutdown.set_value();
  second.join();
  first.join();

  internal_log::GlobalLogHandler::SetLogHandler(previous_handler);

  EXPECT_TRUE(second_returned.load(std::memory_order_acquire));
}
