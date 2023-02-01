// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "common.h"

#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"

#include <gtest/gtest.h>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::metrics;

namespace
{
nostd::shared_ptr<metrics::Meter> InitMeter(MetricReader **metricReaderPtr,
                                            std::string meter_name = "meter_name")
{
  static std::shared_ptr<metrics::MeterProvider> provider(new MeterProvider());
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());
  *metricReaderPtr = metric_reader.get();
  auto p           = std::static_pointer_cast<MeterProvider>(provider);
  p->AddMetricReader(std::move(metric_reader));
  auto meter = provider->GetMeter(meter_name);
  return meter;
}
}  // namespace

void asyc_generate_measurements(opentelemetry::metrics::ObserverResult observer, void * /* state */)
{
  auto observer_long =
      nostd::get<nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<int64_t>>>(observer);
  observer_long->Observe(10);
}

TEST(MeterTest, BasicAsyncTests)
{
  MetricReader *metric_reader_ptr = nullptr;
  auto meter                      = InitMeter(&metric_reader_ptr);
  auto observable_counter         = meter->CreateInt64ObservableCounter("observable_counter");
  observable_counter->AddCallback(asyc_generate_measurements, nullptr);

  size_t count = 0;
  metric_reader_ptr->Collect([&count](ResourceMetrics &metric_data) {
    EXPECT_EQ(metric_data.scope_metric_data_.size(), 1);
    if (metric_data.scope_metric_data_.size())
    {
      EXPECT_EQ(metric_data.scope_metric_data_[0].metric_data_.size(), 1);
      if (metric_data.scope_metric_data_.size())
      {
        count += metric_data.scope_metric_data_[0].metric_data_.size();
        EXPECT_EQ(count, 1);
      }
    }
    return true;
  });
  observable_counter->RemoveCallback(asyc_generate_measurements, nullptr);
}

constexpr static unsigned MAX_THREADS       = 25;
constexpr static unsigned MAX_ITERATIONS_MT = 1000;

TEST(MeterTest, StressMultiThread)
{
  MetricReader *metric_reader_ptr = nullptr;
  auto meter                      = InitMeter(&metric_reader_ptr, "stress_test_meter");
  std::atomic<unsigned> threadCount(0);
  std::atomic<size_t> numIterations(MAX_ITERATIONS_MT);
  std::atomic<bool> do_collect{false}, do_sync_create{true}, do_async_create{false};
  std::vector<nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument>>
      observable_instruments;
  std::vector<std::thread> meter_operation_threads;
  std::atomic<size_t> instrument_id(0);
  while (numIterations--)
  {
    for (size_t i = 0; i < MAX_THREADS; i++)
    {
      if (threadCount++ < MAX_THREADS)
      {
        auto t = std::thread([&]() {
          std::this_thread::yield();
          if (do_sync_create.exchange(false))
          {
            std::string instrument_name = "test_couter_" + std::to_string(instrument_id);
            meter->CreateUInt64Counter(instrument_name, "", "");
            do_async_create.store(true);
            instrument_id++;
          }
          if (do_async_create.exchange(false))
          {
            std::cout << "\n creating async thread " << std::to_string(numIterations);
            auto observable_instrument = meter->CreateInt64ObservableUpDownCounter(
                "test_gauge_" + std::to_string(instrument_id));
            observable_instrument->AddCallback(asyc_generate_measurements, nullptr);
            observable_instruments.push_back(std::move(observable_instrument));
            do_collect.store(true);
            instrument_id++;
          }
          if (do_collect.exchange(false))
          {
            metric_reader_ptr->Collect([](ResourceMetrics & /* metric_data */) { return true; });
            do_sync_create.store(true);
          }
        });
        meter_operation_threads.push_back(std::move(t));
      }
    }
  }
  for (auto &t : meter_operation_threads)
  {
    if (t.joinable())
    {
      t.join();
    }
  }
}
