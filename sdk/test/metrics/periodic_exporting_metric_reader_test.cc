// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdlib>
#include <initializer_list>
#include <mutex>
#include <ratio>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "opentelemetry/metrics/async_instruments.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/observer_result.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_options.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

#if defined(_MSC_VER)
#  include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#endif

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::metrics;

namespace
{

class MockPushMetricExporter : public PushMetricExporter
{
public:
  MockPushMetricExporter(std::chrono::milliseconds wait) : wait_(wait) {}

  opentelemetry::sdk::common::ExportResult Export(const ResourceMetrics &record) noexcept override
  {
    if (wait_ > std::chrono::milliseconds::zero())
    {
      std::this_thread::sleep_for(wait_);
    }
    std::lock_guard<std::mutex> lk(m_);
    records_.push_back(record);
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  bool ForceFlush(std::chrono::microseconds /* timeout */) noexcept override { return false; }

  sdk::metrics::AggregationTemporality GetAggregationTemporality(
      sdk::metrics::InstrumentType /* instrument_type */) const noexcept override
  {
    return sdk::metrics::AggregationTemporality::kCumulative;
  }

  bool Shutdown(std::chrono::microseconds timeout) noexcept override
  {
    std::lock_guard<std::mutex> lk(m_);
    last_shutdown_timeout_ = timeout;
    return true;
  }

  // Guarded by a mutex (rather than left as a plain read) because tests may poll this
  // from the main thread concurrently with the worker thread's Export() calls, with no
  // other synchronization between the two accesses.
  size_t GetDataCount()
  {
    std::lock_guard<std::mutex> lk(m_);
    return records_.size();
  }

  // The timeout this exporter was handed by the reader's Shutdown(), so tests can check that
  // the reader passes on what remains of the caller's budget instead of a fresh full one.
  std::chrono::microseconds GetLastShutdownTimeout()
  {
    std::lock_guard<std::mutex> lk(m_);
    return last_shutdown_timeout_;
  }

private:
  std::mutex m_;
  std::vector<ResourceMetrics> records_;
  std::chrono::milliseconds wait_;
  std::chrono::microseconds last_shutdown_timeout_{std::chrono::microseconds::zero()};
};

class MockMetricProducer : public MetricProducer
{
public:
  MockMetricProducer(std::chrono::microseconds sleep_ms = std::chrono::microseconds::zero())
      : sleep_ms_{sleep_ms}
  {}

  MetricProducer::Result Produce() noexcept override
  {
    std::this_thread::sleep_for(sleep_ms_);
    data_sent_size_.fetch_add(1, std::memory_order_acq_rel);
    ResourceMetrics data;
    return {data, MetricProducer::Status::kSuccess};
  }

  // Atomic (rather than a plain size_t) because tests may poll this from the main thread
  // concurrently with the worker thread's Produce() calls, with no other synchronization
  // between the two accesses.
  size_t GetDataCount() { return data_sent_size_.load(std::memory_order_acquire); }

private:
  std::chrono::microseconds sleep_ms_;
  std::atomic<size_t> data_sent_size_{0};
};

// Lets the test thread wait until the worker thread has started a slow call.
class StartedSignal
{
public:
  void Notify()
  {
    {
      std::lock_guard<std::mutex> lk(m_);
      started_ = true;
    }
    cv_.notify_all();
  }

  void Wait()
  {
    std::unique_lock<std::mutex> lk(m_);
    cv_.wait(lk, [this]() { return started_; });
  }

private:
  std::mutex m_;
  std::condition_variable cv_;
  bool started_ = false;
};

// Blocks in its first Produce() call, then reports a timeout together with one metric, the way a
// producer that gives up part way through returns what it had already collected.
class TimingOutMetricProducer : public MetricProducer
{
public:
  explicit TimingOutMetricProducer(std::chrono::milliseconds block) : block_{block} {}

  MetricProducer::Result Produce() noexcept override
  {
    started_.Notify();
    std::this_thread::sleep_for(block_);
    ScopeMetrics scope_metrics;
    scope_metrics.metric_data_.emplace_back();
    ResourceMetrics data;
    data.scope_metric_data_.emplace_back(std::move(scope_metrics));
    return {data, MetricProducer::Status::kTimeout};
  }

  void WaitUntilProducing() { started_.Wait(); }

private:
  std::chrono::milliseconds block_;
  StartedSignal started_;
};

struct SlowCallbackState
{
  std::chrono::milliseconds duration{0};
  std::atomic<int> calls{0};
  StartedSignal started;
};

// NOLINTNEXTLINE(performance-unnecessary-value-param)
void SlowObservableCallback(opentelemetry::metrics::ObserverResult /* result */, void *state)
{
  auto *slow_state = static_cast<SlowCallbackState *>(state);
  slow_state->calls.fetch_add(1, std::memory_order_acq_rel);
  slow_state->started.Notify();
  std::this_thread::sleep_for(slow_state->duration);
}

TEST(PeriodicExportingMetricReader, BasicTests)
{
  std::unique_ptr<PushMetricExporter> exporter(
      new MockPushMetricExporter(std::chrono::milliseconds{0}));
  PeriodicExportingMetricReaderOptions options;
  options.export_timeout_millis  = std::chrono::milliseconds(200);
  options.export_interval_millis = std::chrono::milliseconds(500);
  auto exporter_ptr              = exporter.get();
  std::shared_ptr<PeriodicExportingMetricReader> reader =
      std::make_shared<PeriodicExportingMetricReader>(std::move(exporter), options);
  MockMetricProducer producer;
  reader->SetMetricProducer(&producer);
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  reader->ForceFlush();
  reader->Shutdown();
  EXPECT_EQ(static_cast<MockPushMetricExporter *>(exporter_ptr)->GetDataCount(),
            static_cast<MockMetricProducer *>(&producer)->GetDataCount());
}

TEST(PeriodicExportingMetricReader, ShutdownPerformsFinalCollectAndExport)
{
  // Shutdown() must perform one last collect-and-export cycle so that metrics recorded since
  // the last periodic tick are not silently dropped. Use a long export interval so that no
  // periodic tick (other than the initial one at thread start) fires on its own before we call
  // Shutdown().
  std::unique_ptr<PushMetricExporter> exporter(
      new MockPushMetricExporter(std::chrono::milliseconds{0}));
  PeriodicExportingMetricReaderOptions options;
  options.export_timeout_millis  = std::chrono::milliseconds(200);
  options.export_interval_millis = std::chrono::milliseconds(10000);
  auto exporter_ptr              = exporter.get();
  std::shared_ptr<PeriodicExportingMetricReader> reader =
      std::make_shared<PeriodicExportingMetricReader>(std::move(exporter), options);
  MockMetricProducer producer;
  reader->SetMetricProducer(&producer);

  // Let the initial (t=0) collect-and-export cycle complete and the worker settle into its
  // long wait, well before the next periodic tick would ever fire on its own.
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  auto count_before_shutdown = producer.GetDataCount();

  reader->Shutdown();

  // At least one: the shutdown-time drain reuses OnForceFlush()'s wake-the-worker-and-wait
  // machinery, which can (rarely, depending on scheduling) wake the worker for an extra cycle
  // beyond the one this test is checking for -- harmless, since the spec only requires that
  // recently-recorded metrics aren't dropped, not that shutdown perform exactly one cycle.
  EXPECT_GE(producer.GetDataCount(), count_before_shutdown + 1);
  EXPECT_EQ(static_cast<MockPushMetricExporter *>(exporter_ptr)->GetDataCount(),
            producer.GetDataCount());
}

TEST(PeriodicExportingMetricReader, ShutdownReportsFailedFinalFlush)
{
  // MockPushMetricExporter fails ForceFlush() but succeeds Shutdown(). A failed final flush must
  // not be masked by the successful exporter shutdown that follows it -- Shutdown() reports the
  // failure -- while cleanup still completes: the drain reaches the exporter, the worker thread
  // is joined, and the exporter is shut down.
  std::unique_ptr<PushMetricExporter> exporter(
      new MockPushMetricExporter(std::chrono::milliseconds{0}));
  PeriodicExportingMetricReaderOptions options;
  options.export_timeout_millis  = std::chrono::milliseconds(200);
  options.export_interval_millis = std::chrono::milliseconds(10000);
  auto exporter_ptr              = exporter.get();
  std::shared_ptr<PeriodicExportingMetricReader> reader =
      std::make_shared<PeriodicExportingMetricReader>(std::move(exporter), options);
  MockMetricProducer producer;
  reader->SetMetricProducer(&producer);

  // Let the initial (t=0) collect-and-export cycle complete and the worker settle into its
  // long wait.
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  auto count_before_shutdown = producer.GetDataCount();

  EXPECT_FALSE(reader->Shutdown());

  EXPECT_TRUE(reader->IsShutdown());
  EXPECT_GE(producer.GetDataCount(), count_before_shutdown + 1);
  EXPECT_EQ(static_cast<MockPushMetricExporter *>(exporter_ptr)->GetDataCount(),
            producer.GetDataCount());
}

TEST(PeriodicExportingMetricReader, ShutdownPassesRemainingTimeoutToExporter)
{
  // The exporter must be given what is left of the caller's shutdown budget after the final
  // flush and join, not a second full one. Here the exporter's 300ms Export() outlasts the
  // 100ms budget, so the budget is exhausted by the time the exporter is shut down -- and an
  // exhausted budget must arrive as a small positive value, never as zero, which some exporters
  // read as "wait indefinitely".
  std::unique_ptr<PushMetricExporter> exporter(
      new MockPushMetricExporter(std::chrono::milliseconds{300}));
  PeriodicExportingMetricReaderOptions options;
  options.export_timeout_millis  = std::chrono::milliseconds(200);
  options.export_interval_millis = std::chrono::milliseconds(10000);
  auto exporter_ptr              = static_cast<MockPushMetricExporter *>(exporter.get());
  std::shared_ptr<PeriodicExportingMetricReader> reader =
      std::make_shared<PeriodicExportingMetricReader>(std::move(exporter), options);
  MockMetricProducer producer;
  reader->SetMetricProducer(&producer);

  // Let the initial (t=0) collect-and-export cycle finish and the worker settle into its long
  // wait, so the only export competing with the budget below is the shutdown drain itself.
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  const auto requested_timeout = std::chrono::milliseconds(100);
  reader->Shutdown(requested_timeout);

  const auto exporter_timeout = exporter_ptr->GetLastShutdownTimeout();
  EXPECT_GT(exporter_timeout, std::chrono::microseconds::zero());
  EXPECT_LT(exporter_timeout, std::chrono::microseconds(requested_timeout));
}

TEST(PeriodicExportingMetricReader, ShutdownWithoutMetricProducer)
{
  // No producer is ever registered, so OnInitialized() never runs and no worker thread exists.
  // Shutdown() must still record that the reader is stopping, otherwise a later ForceFlush()
  // waits to be serviced by a worker that will never run -- which, with the default timeout,
  // means waiting indefinitely.
  std::unique_ptr<PushMetricExporter> exporter(
      new MockPushMetricExporter(std::chrono::milliseconds{0}));
  PeriodicExportingMetricReaderOptions options;
  options.export_timeout_millis  = std::chrono::milliseconds(200);
  options.export_interval_millis = std::chrono::milliseconds(500);
  std::shared_ptr<PeriodicExportingMetricReader> reader =
      std::make_shared<PeriodicExportingMetricReader>(std::move(exporter), options);

  EXPECT_TRUE(reader->Shutdown());

  const auto start = std::chrono::steady_clock::now();
  reader->ForceFlush();
  const auto elapsed = std::chrono::steady_clock::now() - start;

  EXPECT_LT(elapsed, std::chrono::seconds(1));
}

TEST(PeriodicExportingMetricReader, Timeout)
{
  std::unique_ptr<PushMetricExporter> exporter(
      new MockPushMetricExporter(std::chrono::milliseconds{2000}));
  PeriodicExportingMetricReaderOptions options;
  options.export_timeout_millis  = std::chrono::milliseconds(200);
  options.export_interval_millis = std::chrono::milliseconds(500);
  std::shared_ptr<PeriodicExportingMetricReader> reader =
      std::make_shared<PeriodicExportingMetricReader>(std::move(exporter), options);
  MockMetricProducer producer;
  reader->SetMetricProducer(&producer);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  reader->Shutdown();
}

TEST(PeriodicExportingMetricReader, DestroyWithoutShutdown)
{
  // Verify that destroying a reader without calling Shutdown() does not cause
  // use-after-destroy races on the condition variable / mutex used by the
  // background worker thread.  Before the destructor fix this test would fail
  // under ThreadSanitizer with:
  //   WARNING: ThreadSanitizer: unlock of an unlocked mutex (or by a wrong thread)
  auto exporter = std::make_unique<MockPushMetricExporter>(std::chrono::milliseconds{0});
  PeriodicExportingMetricReaderOptions options;
  options.export_timeout_millis  = std::chrono::milliseconds(200);
  options.export_interval_millis = std::chrono::milliseconds(500);
  // producer must be declared before reader so it outlives it — the reader's
  // destructor joins the background thread which may still call Produce().
  MockMetricProducer producer;
  {
    auto reader = std::make_shared<PeriodicExportingMetricReader>(std::move(exporter), options);
    reader->SetMetricProducer(&producer);
    // Let the background thread start and enter its wait loop.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // reader goes out of scope here — no Shutdown() call.
  }
}

TEST(PeriodicExportingMetricReader, ShutdownStopsCollectionBetweenObservableCallbacks)
{
  // Four slow observable callbacks, two in each of two meters, and the reader's default options
  // (60 s interval, 30 s export timeout). Shutdown(timeout) is called while the first callback is
  // running. Collection must stop once that callback returns, rather than run the other three,
  // and nothing collected after the shutdown budget is spent may be exported.
  constexpr int kCallbacks = 4;
  SlowCallbackState state;
  state.duration = std::chrono::milliseconds(500);

  std::unique_ptr<PushMetricExporter> exporter(
      new MockPushMetricExporter(std::chrono::milliseconds{0}));
  auto exporter_ptr = static_cast<MockPushMetricExporter *>(exporter.get());
  auto reader       = std::make_shared<PeriodicExportingMetricReader>(
      std::move(exporter), PeriodicExportingMetricReaderOptions{});

  std::shared_ptr<MeterProvider> provider(new MeterProvider());
  std::vector<nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument>> gauges;
  for (const char *meter_name : {"meter_a", "meter_b"})
  {
    auto meter = provider->GetMeter(meter_name);
    for (const char *gauge_name : {"gauge_1", "gauge_2"})
    {
      auto gauge = meter->CreateInt64ObservableGauge(gauge_name);
      gauge->AddCallback(SlowObservableCallback, &state);
      gauges.push_back(gauge);
    }
  }

  // Registering the reader starts the worker thread, which collects right away.
  provider->AddMetricReader(reader);
  state.started.Wait();

  const auto start = std::chrono::steady_clock::now();
  provider->Shutdown(std::chrono::milliseconds(100));
  const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::steady_clock::now() - start)
                              .count();

  EXPECT_EQ(state.calls.load(std::memory_order_acquire), 1);
  EXPECT_LT(elapsed_ms, (state.duration * (kCallbacks - 1)).count());
  EXPECT_EQ(exporter_ptr->GetDataCount(), 0U);
}

TEST(PeriodicExportingMetricReader, ShutdownSkipsExportOfTimedOutCollection)
{
  // The producer returns kTimeout with one metric after the shutdown budget is spent. With the
  // default 30 s export timeout the collection itself is not over time, so only the shutdown
  // decides that the result must not be exported, and the slow exporter must not be called.
  TimingOutMetricProducer producer(std::chrono::milliseconds(300));
  const auto export_duration = std::chrono::milliseconds(2000);
  std::unique_ptr<PushMetricExporter> exporter(new MockPushMetricExporter(export_duration));
  auto exporter_ptr = static_cast<MockPushMetricExporter *>(exporter.get());
  auto reader       = std::make_shared<PeriodicExportingMetricReader>(
      std::move(exporter), PeriodicExportingMetricReaderOptions{});
  reader->SetMetricProducer(&producer);
  producer.WaitUntilProducing();

  const auto start = std::chrono::steady_clock::now();
  reader->Shutdown(std::chrono::milliseconds(100));
  const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::steady_clock::now() - start)
                              .count();

  EXPECT_EQ(exporter_ptr->GetDataCount(), 0U);
  EXPECT_LT(elapsed_ms, export_duration.count());
}

TEST(PeriodicExportingMetricReaderOptions, UsesEnvVars)
{
  const char *env_interval = "OTEL_METRIC_EXPORT_INTERVAL";
  const char *env_timeout  = "OTEL_METRIC_EXPORT_TIMEOUT";

  setenv(env_interval, "1500ms", 1);
  setenv(env_timeout, "1000ms", 1);

  PeriodicExportingMetricReaderOptions options;
  EXPECT_EQ(options.export_interval_millis, std::chrono::milliseconds(1500));
  EXPECT_EQ(options.export_timeout_millis, std::chrono::milliseconds(1000));

  unsetenv(env_interval);
  unsetenv(env_timeout);
}

TEST(PeriodicExportingMetricReaderOptions, UsesDefault)
{
  const char *env_interval = "OTEL_METRIC_EXPORT_INTERVAL";
  const char *env_timeout  = "OTEL_METRIC_EXPORT_TIMEOUT";

  unsetenv(env_interval);
  unsetenv(env_timeout);

  PeriodicExportingMetricReaderOptions options;
  EXPECT_EQ(options.export_interval_millis, std::chrono::milliseconds(60000));
  EXPECT_EQ(options.export_timeout_millis, std::chrono::milliseconds(30000));
}

}  // namespace
