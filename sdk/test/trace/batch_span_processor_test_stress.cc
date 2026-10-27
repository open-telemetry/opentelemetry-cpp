// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace
{

class CountingSpanExporter final : public sdk::trace::SpanExporter
{
public:
  explicit CountingSpanExporter(std::shared_ptr<std::atomic<std::size_t>> exported_count) noexcept
      : exported_count_(std::move(exported_count))
  {}

  std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<sdk::trace::Recordable>(new sdk::trace::SpanData);
  }

  sdk::common::ExportResult Export(
      const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &recordables) noexcept override
  {
    exported_count_->fetch_add(recordables.size(), std::memory_order_relaxed);
    return sdk::common::ExportResult::kSuccess;
  }

  bool ForceFlush(std::chrono::microseconds /*timeout*/) noexcept override { return true; }

  bool Shutdown(std::chrono::microseconds /*timeout*/) noexcept override { return true; }

private:
  std::shared_ptr<std::atomic<std::size_t>> exported_count_;
};

// A lost wakeup results in the worker being parked for the entire schedule delay,
// so the watchdog only has to separate "instant" from "parked for the entire delay"
// while being generous enough to avoid false positives on slow CI runners.
constexpr std::chrono::minutes kParkScheduleDelay{10};
constexpr std::chrono::minutes kWakeupWatchdog{1};
constexpr int kRounds = 2000;
constexpr const char *kShutdownStallHint =
    "possible lost shutdown wakeup stall during worker join()";

// Runs `operation` on another thread and aborts the binary if it does not return in time.
template <typename Operation>
auto CallWithWatchdog(const char *operation_name,
                      const char *stall_hint,
                      int round,
                      const Operation &operation)
{
  auto result = std::async(std::launch::async, operation);
  if (result.wait_for(kWakeupWatchdog) == std::future_status::timeout)
  {
    std::cerr << operation_name << " did not return within " << kWakeupWatchdog.count()
              << "m at round " << round << ". " << stall_hint << '\n';
    std::abort();
  }
  return result.get();
}

// Busy-spins a scheduling-independent amount to offset the next call within a race window.
void BusySpin(int iterations)
{
  volatile int spin_sink = 0;
  for (int s = 0; s < iterations; ++s)
  {
    int next  = spin_sink;
    spin_sink = next + 1;
  }
}

void ShutdownWithWatchdog(sdk::trace::BatchSpanProcessor &processor, int round)
{
  EXPECT_TRUE(CallWithWatchdog("teardown Shutdown()", kShutdownStallHint, round,
                               [&processor] { return processor.Shutdown(); }));
}

template <typename Operation>
void RunWorkerParkRace(const char *operation_name, const char *stall_hint, Operation operation)
{
  constexpr int kSpinSweep = 50;

  for (int round = 0; round < kRounds; ++round)
  {
    auto exported_count = std::make_shared<std::atomic<std::size_t>>(0);

    sdk::trace::BatchSpanProcessorOptions options;
    options.schedule_delay_millis = kParkScheduleDelay;
    options.max_queue_size        = 4096;
    options.max_export_batch_size = 512;

    auto processor = std::make_shared<sdk::trace::BatchSpanProcessor>(
        std::make_unique<CountingSpanExporter>(exported_count), options);

    // Vary the offset across a sweep so that over the whole set we have a better chance of hitting
    // the race window.
    BusySpin(round * kSpinSweep);
    processor->OnEnd(processor->MakeRecordable());

    EXPECT_TRUE(CallWithWatchdog(operation_name, stall_hint, round,
                                 [operation, processor] { return operation(*processor); }));
    EXPECT_EQ(exported_count->load(std::memory_order_relaxed), 1u);

    // Shutdown() already joined the worker; ForceFlush() left it running. Join it either way
    // before the next round.
    ShutdownWithWatchdog(*processor, round);
  }
}

// Catch a lost cv wakeup during Shutdown(). A lost wakeup parks the worker for the whole schedule
// delay, so the untimed join() inside Shutdown() blocks for that long.
TEST(BatchSpanProcessorStress, ShutdownRacesWorkerPark)
{
  RunWorkerParkRace("ShutdownRacesWorkerPark: Shutdown()", kShutdownStallHint,
                    [](sdk::trace::BatchSpanProcessor &processor) { return processor.Shutdown(); });
}

// Catch a lost cv wakeup during ForceFlush(). A lost wakeup parks the worker for the whole
// schedule delay before it services the flush, so ForceFlush() blocks for that long.
TEST(BatchSpanProcessorStress, ForceFlushRacesWorkerPark)
{
  RunWorkerParkRace(
      "ForceFlushRacesWorkerPark: ForceFlush()", "possible lost force-flush wakeup",
      [](sdk::trace::BatchSpanProcessor &processor) { return processor.ForceFlush(); });
}

// Catch a lost cv wakeup in OnEnd() from the span that fills the queue.
TEST(BatchSpanProcessorStress, OnEndRacesWorkerPark)
{
  constexpr std::size_t kQueueSize = 2;
  constexpr int kSweepPeriod       = 200;
  constexpr int kSpinSweep         = 10;

  for (int round = 0; round < kRounds; ++round)
  {
    auto exported_count = std::make_shared<std::atomic<std::size_t>>(0);

    sdk::trace::BatchSpanProcessorOptions options;
    options.schedule_delay_millis = kParkScheduleDelay;
    options.max_queue_size        = kQueueSize;
    options.max_export_batch_size = kQueueSize;

    auto processor = std::make_shared<sdk::trace::BatchSpanProcessor>(
        std::make_unique<CountingSpanExporter>(exported_count), options);

    // The half-full notify wakes the worker, which parks again on the partial batch.
    processor->OnEnd(processor->MakeRecordable());

    // The race window sits shortly after that wakeup, so sweep short offsets repeatedly.
    BusySpin((round % kSweepPeriod) * kSpinSweep);
    processor->OnEnd(processor->MakeRecordable());

    CallWithWatchdog("OnEndRacesWorkerPark: export", "possible lost OnEnd wakeup", round,
                     [exported_count] {
                       while (exported_count->load(std::memory_order_relaxed) < kQueueSize)
                       {
                         std::this_thread::yield();
                       }
                     });

    ShutdownWithWatchdog(*processor, round);
  }
}

}  // namespace

OPENTELEMETRY_END_NAMESPACE
