// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stddef.h>
#include <stdint.h>
#include <algorithm>
#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "common.h"

#include <functional>
#include "opentelemetry/context/context.h"
#include "opentelemetry/metrics/async_instruments.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/resource/resource.h"

#include <opentelemetry/sdk/metrics/view/view_registry_factory.h>

#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/observer_result.h"
#include "opentelemetry/metrics/sync_instruments.h"  // IWYU pragma: keep
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::metrics;

namespace
{
nostd::shared_ptr<metrics::Meter> InitMeter(MetricReader **metricReaderPtr,
                                            const std::string &meter_name = "meter_name")
{
  static std::shared_ptr<metrics::MeterProvider> provider(new MeterProvider());
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());
  *metricReaderPtr = metric_reader.get();
  auto p           = std::static_pointer_cast<MeterProvider>(provider);
  p->AddMetricReader(std::move(metric_reader));
  auto meter = provider->GetMeter(meter_name);
  return meter;
}

void asyc_generate_measurements_long(opentelemetry::metrics::ObserverResult observer,
                                     void * /* state */)
{
  auto observer_long =
      nostd::get<nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<int64_t>>>(observer);
  observer_long->Observe(10);
}

void asyc_generate_measurements_double(opentelemetry::metrics::ObserverResult observer,
                                       void * /* state */)
{
  auto observer_double =
      nostd::get<nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<double>>>(observer);
  observer_double->Observe(10.2f);
}

std::shared_ptr<metrics::MeterProvider> GetMeterProviderWithScopeConfigurator(
    const ScopeConfigurator<MeterConfig> &meter_configurator,
    MetricReader **metric_reader_ptr)
{
  auto views    = ViewRegistryFactory::Create();
  auto resource = sdk::resource::Resource::Create({});
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());
  *metric_reader_ptr = metric_reader.get();
  std::shared_ptr<metrics::MeterProvider> provider(
      new MeterProvider(std::move(views), resource,
                        std::make_unique<ScopeConfigurator<MeterConfig>>(meter_configurator)));
  auto p = std::static_pointer_cast<MeterProvider>(provider);
  p->AddMetricReader(std::move(metric_reader));
  return p;
}
}  // namespace

TEST(MeterTest, BasicAsyncTests)
{
  MetricReader *metric_reader_ptr = nullptr;
  auto meter                      = InitMeter(&metric_reader_ptr);
  auto observable_counter         = meter->CreateInt64ObservableCounter("observable_counter");
  observable_counter->AddCallback(asyc_generate_measurements_long, nullptr);

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
  observable_counter->RemoveCallback(asyc_generate_measurements_long, nullptr);
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
            observable_instrument->AddCallback(asyc_generate_measurements_long, nullptr);
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

TEST(MeterTest, MeterWithDisabledConfig)
{
  ScopeConfigurator<MeterConfig> disable_all_scopes =
      ScopeConfigurator<MeterConfig>::Builder(MeterConfig::Disabled()).Build();
  MetricReader *metric_reader_ptr = nullptr;
  std::shared_ptr<metrics::MeterProvider> meter_provider =
      GetMeterProviderWithScopeConfigurator(disable_all_scopes, &metric_reader_ptr);

  auto meter = meter_provider->GetMeter("foo", "0.1.0", "https://opentelemetry.io/schemas/1.24.0");

  // Test all supported instruments from this meter - create instruments
  auto double_counter         = meter->CreateDoubleCounter("double_counter");
  auto double_histogram       = meter->CreateDoubleHistogram("double_histogram");
  auto double_up_down_counter = meter->CreateDoubleUpDownCounter("double_up_down_counter");
  auto double_obs_counter     = meter->CreateDoubleObservableCounter("double_obs_counter");
  auto double_obs_gauge       = meter->CreateDoubleObservableGauge("double_obs_gauge");
  auto double_obs_up_down_counter =
      meter->CreateDoubleObservableUpDownCounter("double_obs_up_down_counter");

  auto uint64_counter        = meter->CreateUInt64Counter("uint64_counter");
  auto uint64_histogram      = meter->CreateUInt64Histogram("uint64_histogram");
  auto int64_up_down_counter = meter->CreateInt64UpDownCounter("int64_up_down_counter");
  auto int64_obs_counter     = meter->CreateInt64ObservableCounter("int64_obs_counter");
  auto int64_obs_gauge       = meter->CreateInt64ObservableGauge("int64_obs_gauge");
  auto int64_obs_up_down_counter =
      meter->CreateInt64ObservableUpDownCounter("int64_obs_up_down_counter");

  // Invoke the created instruments
  double_counter->Add(1.0f);
  double_histogram->Record(23.2f, {});
  double_up_down_counter->Add(-2.4f);
  double_obs_counter->AddCallback(asyc_generate_measurements_double, nullptr);
  double_obs_gauge->AddCallback(asyc_generate_measurements_double, nullptr);
  double_obs_up_down_counter->AddCallback(asyc_generate_measurements_double, nullptr);

  uint64_counter->Add(1);
  uint64_histogram->Record(23, {});
  int64_up_down_counter->Add(-2);
  int64_obs_counter->AddCallback(asyc_generate_measurements_long, nullptr);
  int64_obs_gauge->AddCallback(asyc_generate_measurements_long, nullptr);
  int64_obs_up_down_counter->AddCallback(asyc_generate_measurements_long, nullptr);

  // No active instruments are expected - since all scopes are disabled.
  metric_reader_ptr->Collect([&](ResourceMetrics &metric_data) {
    EXPECT_EQ(metric_data.scope_metric_data_.size(), 0);
    return true;
  });
}

TEST(MeterTest, MeterWithEnabledConfig)
{
  ScopeConfigurator<MeterConfig> enable_all_scopes =
      ScopeConfigurator<MeterConfig>::Builder(MeterConfig::Enabled()).Build();
  MetricReader *metric_reader_ptr = nullptr;
  std::shared_ptr<metrics::MeterProvider> meter_provider =
      GetMeterProviderWithScopeConfigurator(enable_all_scopes, &metric_reader_ptr);

  auto meter = meter_provider->GetMeter("foo", "0.1.0", "https://opentelemetry.io/schemas/1.24.0");

  // Test all supported instruments from this meter - create instruments
  auto double_counter         = meter->CreateDoubleCounter("double_counter");
  auto double_histogram       = meter->CreateDoubleHistogram("double_histogram");
  auto double_up_down_counter = meter->CreateDoubleUpDownCounter("double_up_down_counter");
  auto double_obs_counter     = meter->CreateDoubleObservableCounter("double_obs_counter");
  auto double_obs_gauge       = meter->CreateDoubleObservableGauge("double_obs_gauge");
  auto double_obs_up_down_counter =
      meter->CreateDoubleObservableUpDownCounter("double_obs_up_down_counter");

  auto uint64_counter        = meter->CreateUInt64Counter("uint64_counter");
  auto uint64_histogram      = meter->CreateUInt64Histogram("uint64_histogram");
  auto int64_up_down_counter = meter->CreateInt64UpDownCounter("int64_up_down_counter");
  auto int64_obs_counter     = meter->CreateInt64ObservableCounter("int64_obs_counter");
  auto int64_obs_gauge       = meter->CreateInt64ObservableGauge("int64_obs_gauge");
  auto int64_obs_up_down_counter =
      meter->CreateInt64ObservableUpDownCounter("int64_obs_up_down_counter");

  // Invoke the created instruments
  double_counter->Add(1.0f);
  double_histogram->Record(23.2f, {});
  double_up_down_counter->Add(-2.4f);
  double_obs_counter->AddCallback(asyc_generate_measurements_double, nullptr);
  double_obs_gauge->AddCallback(asyc_generate_measurements_double, nullptr);
  double_obs_up_down_counter->AddCallback(asyc_generate_measurements_double, nullptr);

  uint64_counter->Add(1);
  uint64_histogram->Record(23, {});
  int64_up_down_counter->Add(-2);
  int64_obs_counter->AddCallback(asyc_generate_measurements_long, nullptr);
  int64_obs_gauge->AddCallback(asyc_generate_measurements_long, nullptr);
  int64_obs_up_down_counter->AddCallback(asyc_generate_measurements_long, nullptr);

  // Expected active instruments
  std::vector<std::pair<std::string, std::string>> active_scope_instrument_pairs;
  active_scope_instrument_pairs.emplace_back("foo", "double_counter");
  active_scope_instrument_pairs.emplace_back("foo", "double_histogram");
  active_scope_instrument_pairs.emplace_back("foo", "double_up_down_counter");
  active_scope_instrument_pairs.emplace_back("foo", "double_obs_up_down_counter");
  active_scope_instrument_pairs.emplace_back("foo", "double_obs_counter");
  active_scope_instrument_pairs.emplace_back("foo", "double_obs_gauge");
  active_scope_instrument_pairs.emplace_back("foo", "uint64_counter");
  active_scope_instrument_pairs.emplace_back("foo", "uint64_histogram");
  active_scope_instrument_pairs.emplace_back("foo", "int64_up_down_counter");
  active_scope_instrument_pairs.emplace_back("foo", "int64_obs_up_down_counter");
  active_scope_instrument_pairs.emplace_back("foo", "int64_obs_counter");
  active_scope_instrument_pairs.emplace_back("foo", "int64_obs_gauge");

  metric_reader_ptr->Collect([&](const ResourceMetrics &metric_data) {
    bool unexpected_instrument_found = false;
    std::string curr_scope_name      = metric_data.scope_metric_data_.at(0).scope_->GetName();
    EXPECT_EQ(metric_data.scope_metric_data_.size(), 1);
    EXPECT_EQ(metric_data.scope_metric_data_.at(0).scope_->GetName(), "foo");
    EXPECT_EQ(metric_data.scope_metric_data_.at(0).metric_data_.size(), 12);
    for (const MetricData &md : metric_data.scope_metric_data_.at(0).metric_data_)
    {
      auto found_instrument = std::make_pair(curr_scope_name, md.instrument_descriptor.name_);
      // confirm if the found instrument is expected
      auto it = std::find(active_scope_instrument_pairs.begin(),
                          active_scope_instrument_pairs.end(), found_instrument);
      if (it == active_scope_instrument_pairs.end())
      {
        // found instrument is not expected
        unexpected_instrument_found = true;
        break;
      }
    }
    EXPECT_FALSE(unexpected_instrument_found);
    return true;
  });
}

TEST(MeterTest, MeterWithCustomConfig)
{
  // within the same call
  auto check_if_version_present = [](const InstrumentationScope &scope_info) {
    return !scope_info.GetVersion().empty();
  };

  // custom scope configurator that only disables meters with name "foo_library" or do not have
  // version information
  ScopeConfigurator<MeterConfig> custom_scope_configurator =
      ScopeConfigurator<MeterConfig>::Builder(MeterConfig::Disabled())
          .AddConditionNameEquals("foo_library", MeterConfig::Disabled())
          .AddCondition(check_if_version_present, MeterConfig::Enabled())
          .Build();
  MetricReader *metric_reader_ptr = nullptr;
  std::shared_ptr<metrics::MeterProvider> meter_provider =
      GetMeterProviderWithScopeConfigurator(custom_scope_configurator, &metric_reader_ptr);

  // The meter has version information and name is not "foo_library".
  // All instruments from this meter should be active and recording metrics.
  auto meter_enabled_expected_bar =
      meter_provider->GetMeter("bar_library", "0.1.0", "https://opentelemetry.io/schemas/1.24.0");

  // The meter has version information and name is "foo_library".
  // All instruments from this meter should be noop.
  auto meter_disabled_expected_foo =
      meter_provider->GetMeter("foo_library", "0.1.0", "https://opentelemetry.io/schemas/1.24.0");

  // This meter has no version information.
  // All instruments from this meter should be noop.
  auto meter_disabled_expected_baz =
      meter_provider->GetMeter("baz_library", "", "https://opentelemetry.io/schemas/1.24.0");

  // Create instruments from all meters
  auto double_counter_bar = meter_enabled_expected_bar->CreateDoubleCounter("double_counter");
  auto double_counter_foo = meter_disabled_expected_foo->CreateDoubleCounter("double_counter");
  auto double_counter_baz = meter_disabled_expected_baz->CreateDoubleCounter("double_counter");

  // Invoke created instruments at least once
  double_counter_bar->Add(1.0f);
  double_counter_foo->Add(1.0f);
  double_counter_baz->Add(1.0f);

  std::vector<std::pair<std::string, std::string>> active_scope_instrument_pairs;
  active_scope_instrument_pairs.emplace_back("bar_library", "double_counter");

  metric_reader_ptr->Collect([&](const ResourceMetrics &metric_data) {
    int found_instruments            = 0;
    bool unexpected_instrument_found = false;
    for (const ScopeMetrics &sm : metric_data.scope_metric_data_)
    {
      std::string curr_scope = sm.scope_->GetName();
      for (const MetricData &md : sm.metric_data_)
      {
        found_instruments++;
        auto found_instrument = std::make_pair(curr_scope, md.instrument_descriptor.name_);
        // confirm if the found instrument is expected
        auto it = std::find(active_scope_instrument_pairs.begin(),
                            active_scope_instrument_pairs.end(), found_instrument);
        if (it == active_scope_instrument_pairs.end())
        {
          // found instrument is not expected
          unexpected_instrument_found = true;
          break;
        }
      }
    }
    EXPECT_EQ(found_instruments, active_scope_instrument_pairs.size());
    EXPECT_FALSE(unexpected_instrument_found);
    return true;
  });
}
