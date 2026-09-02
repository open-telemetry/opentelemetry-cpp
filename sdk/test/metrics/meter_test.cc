// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>  // IWYU pragma: keep
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "common.h"

#include "opentelemetry/context/context.h"  // IWYU pragma: keep
#include "opentelemetry/metrics/async_instruments.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/observer_result.h"
#include "opentelemetry/metrics/sync_instruments.h"  // IWYU pragma: keep
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/data/exemplar_data.h"  // IWYU pragma: keep
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#include "opentelemetry/sdk/metrics/view/meter_selector.h"
#include "opentelemetry/sdk/metrics/view/view.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/metrics/view/view_registry_factory.h"
#include "opentelemetry/sdk/resource/resource.h"

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::common::internal_log;

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

class TestLogHandler : public LogHandler
{
public:
  void Handle(LogLevel level,
              const char * /*file*/,
              int /*line*/,
              const char *msg,
              const sdk::common::AttributeMap & /*attributes*/) noexcept override

  {
    switch (level)
    {
      case LogLevel::Warning: {
        warnings.emplace_back(msg);
        break;
      }
      case LogLevel::Error: {
        errors.emplace_back(msg);
        break;
      }
      case LogLevel::None:
      case LogLevel::Info:
      case LogLevel::Debug:
      default:
        break;
    }

    std::cout << "[" << LevelToString(level) << "] " << msg << "\n";
  }

  bool HasNameCaseConflictWarning() const
  {
    return std::any_of(warnings.begin(), warnings.end(), [](const std::string &warning) {
      return warning.find("WarnOnNameCaseConflict") != std::string::npos;
    });
  }

  bool HasDuplicateInstrumentWarning() const
  {
    return std::any_of(warnings.begin(), warnings.end(), [](const std::string &warning) {
      return warning.find("WarnOnDuplicateInstrument") != std::string::npos;
    });
  }

  bool HasSemanticErrorWarning() const
  {
    return std::any_of(warnings.begin(), warnings.end(), [](const std::string &warning) {
      return warning.find("WarnOnViewSemanticError") != std::string::npos;
    });
  }

  bool HasErrors() const { return !errors.empty(); }
  bool HasWarnings() const { return !warnings.empty(); }

private:
  std::vector<std::string> warnings;
  std::vector<std::string> errors;
};

class MeterCreateInstrumentTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    ASSERT_TRUE(log_handler_ != nullptr);
    ASSERT_TRUE(metric_reader_ptr_ != nullptr);
    ASSERT_TRUE(provider_ != nullptr);
    GlobalLogHandler::SetLogHandler(std::static_pointer_cast<LogHandler>(log_handler_));
    GlobalLogHandler::SetLogLevel(LogLevel::Debug);

    provider_->AddMetricReader(metric_reader_ptr_);
    meter_ = provider_->GetMeter("test_meter");
    ASSERT_TRUE(meter_ != nullptr);
  }

  void TearDown() override {}

  void AddNameCorrectionView(const std::string &name,
                             const std::string &unit,
                             InstrumentType type,
                             const std::string &new_name)
  {
    std::unique_ptr<View> corrective_view{new View(new_name)};
    std::unique_ptr<InstrumentSelector> instrument_selector{
        new InstrumentSelector(type, name, unit)};

    std::unique_ptr<MeterSelector> meter_selector{new MeterSelector("test_meter", "", "")};

    provider_->AddView(std::move(instrument_selector), std::move(meter_selector),
                       std::move(corrective_view));
  }

  void AddDescriptionCorrectionView(const std::string &name,
                                    const std::string &unit,
                                    InstrumentType type,
                                    const std::string &new_description)
  {
    std::unique_ptr<View> corrective_view{new View(name, new_description)};
    std::unique_ptr<InstrumentSelector> instrument_selector{
        new InstrumentSelector(type, name, unit)};

    std::unique_ptr<MeterSelector> meter_selector{new MeterSelector("test_meter", "", "")};

    provider_->AddView(std::move(instrument_selector), std::move(meter_selector),
                       std::move(corrective_view));
  }

  void AddView(const std::string &name,
               const std::string &unit,
               InstrumentType type,
               const std::string &new_name,
               AggregationType new_aggregation_type,
               const std::shared_ptr<AggregationConfig> &new_aggregation_config = nullptr)
  {
    std::unique_ptr<View> view{
        new View(new_name, "", new_aggregation_type, new_aggregation_config)};
    std::unique_ptr<InstrumentSelector> instrument_selector{
        new InstrumentSelector(type, name, unit)};

    std::unique_ptr<MeterSelector> meter_selector{new MeterSelector("test_meter", "", "")};

    provider_->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(view));
  }

  static size_t CountPointData(const std::vector<MetricData> &data,
                               const InstrumentDescriptor &descriptor,
                               bool expected_match = true)
  {
    size_t match_count = 0;
    size_t point_count = 0;
    for (const auto &md : data)
    {
      if (md.instrument_descriptor.name_ == descriptor.name_ &&
          md.instrument_descriptor.unit_ == descriptor.unit_ &&
          md.instrument_descriptor.description_ == descriptor.description_ &&
          md.instrument_descriptor.type_ == descriptor.type_ &&
          md.instrument_descriptor.value_type_ == descriptor.value_type_)
      {
        match_count++;
        point_count = md.point_data_attr_.size();
      }
    }
    EXPECT_EQ(match_count, expected_match ? 1 : 0);
    return point_count;
  }

  static double SumPointData(const std::vector<MetricData> &data,
                             const InstrumentDescriptor &descriptor)
  {
    auto value_to_double = [](const ValueType &value) {
      return nostd::holds_alternative<double>(value)
                 ? nostd::get<double>(value)
                 : static_cast<double>(nostd::get<int64_t>(value));
    };

    double total = 0;
    for (const auto &md : data)
    {
      if (md.instrument_descriptor.name_ != descriptor.name_ ||
          md.instrument_descriptor.unit_ != descriptor.unit_ ||
          md.instrument_descriptor.description_ != descriptor.description_ ||
          md.instrument_descriptor.type_ != descriptor.type_ ||
          md.instrument_descriptor.value_type_ != descriptor.value_type_)
      {
        continue;
      }
      for (const auto &point_data_attr : md.point_data_attr_)
      {
        if (auto *sum_point_data =
                nostd::get_if<sdk::metrics::SumPointData>(&point_data_attr.point_data))
        {
          total += value_to_double(sum_point_data->value_);
        }
        else if (auto *histogram_point_data =
                     nostd::get_if<sdk::metrics::HistogramPointData>(&point_data_attr.point_data))
        {
          total += value_to_double(histogram_point_data->sum_);
        }
        else if (auto *last_value_point_data =
                     nostd::get_if<sdk::metrics::LastValuePointData>(&point_data_attr.point_data))
        {
          total += value_to_double(last_value_point_data->value_);
        }
      }
    }
    return total;
  }

protected:
  struct ObservableResultDouble
  {
    double value{};
    std::map<std::string, std::string> attributes{};

    static void Callback(opentelemetry::metrics::ObserverResult observer, void *state)
    {
      auto *self = static_cast<ObservableResultDouble *>(state);
      auto observer_double =
          nostd::get<nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<double>>>(observer);
      observer_double->Observe(self->value, self->attributes);
    }
  };

  std::shared_ptr<sdk::metrics::MeterProvider> provider_{new sdk::metrics::MeterProvider()};
  std::shared_ptr<TestLogHandler> log_handler_{new TestLogHandler()};
  opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter> meter_{nullptr};

  std::shared_ptr<MetricReader> metric_reader_ptr_{new MockMetricReader()};
};

class TestProcessor : public sdk::metrics::AttributesProcessor
{
public:
  explicit TestProcessor() = default;

  sdk::metrics::MetricAttributes process(
      const opentelemetry::common::KeyValueIterable &attributes) const noexcept override
  {
    // Just forward attributes
    return sdk::metrics::MetricAttributes(attributes);
  }

  bool isPresent(nostd::string_view /*key*/) const noexcept override { return true; }
};

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

TEST_F(MeterCreateInstrumentTest, IdenticalSyncInstruments)
{
  auto counter1 = meter_->CreateDoubleCounter("my_counter", "desc", "unit");
  auto counter2 = meter_->CreateDoubleCounter("my_counter", "desc", "unit");

  counter1->Add(1.0, {{"key", "value1"}});
  counter2->Add(2.5, {{"key", "value2"}});

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor descriptor{"my_counter", "desc", "unit", InstrumentType::kCounter,
                                    InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, descriptor), 2u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor), 3.5);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST_F(MeterCreateInstrumentTest, NameCaseConflictSyncInstruments)
{
  auto counter1 = meter_->CreateUInt64Counter("My_CountER", "desc", "unit");
  auto counter2 = meter_->CreateUInt64Counter("my_counter", "desc", "unit");

  counter1->Add(1);
  counter2->Add(2);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor descriptor{"My_CountER", "desc", "unit", InstrumentType::kCounter,
                                    InstrumentValueType::kLong};
    EXPECT_EQ(CountPointData(data, descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor), 3);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_TRUE(log_handler_->HasNameCaseConflictWarning());
  EXPECT_FALSE(log_handler_->HasDuplicateInstrumentWarning());
}

TEST_F(MeterCreateInstrumentTest, ViewCorrectedNameCaseConflictSyncInstruments)
{
  InstrumentDescriptor descriptor{"My_CountER", "desc", "unit", InstrumentType::kCounter,
                                  InstrumentValueType::kLong};

  AddNameCorrectionView(descriptor.name_, descriptor.unit_, descriptor.type_, "my_counter");

  auto counter1 =
      meter_->CreateUInt64Counter("My_CountER", descriptor.description_, descriptor.unit_);
  auto counter2 =
      meter_->CreateUInt64Counter("my_counter", descriptor.description_, descriptor.unit_);

  counter1->Add(1);
  counter2->Add(2);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor descriptor{"my_counter", "desc", "unit", InstrumentType::kCounter,
                                    InstrumentValueType::kLong};
    EXPECT_EQ(CountPointData(data, descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor), 3);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST_F(MeterCreateInstrumentTest, DuplicateSyncInstrumentsByKind)
{
  auto counter1 = meter_->CreateDoubleCounter("my_counter", "desc", "unit");
  auto counter2 = meter_->CreateUInt64Counter("my_counter", "desc", "unit");

  counter1->Add(1, {{"key", "value1"}});
  counter2->Add(1, {{"key", "value2"}});

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 2);
    InstrumentDescriptor double_descriptor{"my_counter", "desc", "unit", InstrumentType::kCounter,
                                           InstrumentValueType::kDouble};
    InstrumentDescriptor long_descriptor{"my_counter", "desc", "unit", InstrumentType::kCounter,
                                         InstrumentValueType::kLong};
    EXPECT_EQ(CountPointData(data, double_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, double_descriptor), 1);
    EXPECT_EQ(CountPointData(data, long_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, long_descriptor), 1);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_TRUE(log_handler_->HasDuplicateInstrumentWarning());
  EXPECT_FALSE(log_handler_->HasNameCaseConflictWarning());
}

TEST_F(MeterCreateInstrumentTest, DuplicateSyncInstrumentsByUnits)
{
  auto counter1 = meter_->CreateDoubleCounter("my_counter", "desc", "unit");
  auto counter2 = meter_->CreateDoubleCounter("my_counter", "desc", "another_unit");

  counter1->Add(1, {{"key", "value1"}});
  counter2->Add(1, {{"key", "value2"}});

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 2);
    InstrumentDescriptor unit_descriptor{"my_counter", "desc", "unit", InstrumentType::kCounter,
                                         InstrumentValueType::kDouble};
    InstrumentDescriptor another_unit_descriptor{"my_counter", "desc", "another_unit",
                                                 InstrumentType::kCounter,
                                                 InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, unit_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, unit_descriptor), 1);
    EXPECT_EQ(CountPointData(data, another_unit_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, another_unit_descriptor), 1);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_TRUE(log_handler_->HasDuplicateInstrumentWarning());
  EXPECT_FALSE(log_handler_->HasNameCaseConflictWarning());
}

TEST_F(MeterCreateInstrumentTest, DuplicateSyncInstrumentsByDescription)
{
  auto counter1 = meter_->CreateDoubleCounter("my_counter", "desc", "unit");
  auto counter2 = meter_->CreateDoubleCounter("my_counter", "another_desc", "unit");

  counter1->Add(1, {{"key", "value1"}});
  counter2->Add(1, {{"key", "value2"}});

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 2);
    InstrumentDescriptor desc_descriptor{"my_counter", "desc", "unit", InstrumentType::kCounter,
                                         InstrumentValueType::kDouble};
    InstrumentDescriptor another_desc_descriptor{"my_counter", "another_desc", "unit",
                                                 InstrumentType::kCounter,
                                                 InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, desc_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, desc_descriptor), 1);
    EXPECT_EQ(CountPointData(data, another_desc_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, another_desc_descriptor), 1);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_TRUE(log_handler_->HasDuplicateInstrumentWarning());
  EXPECT_FALSE(log_handler_->HasNameCaseConflictWarning());
}

TEST_F(MeterCreateInstrumentTest, ViewCorrectedDuplicateSyncInstrumentsByDescription)
{
  InstrumentDescriptor descriptor{"my_counter", "desc", "unit", InstrumentType::kCounter,
                                  InstrumentValueType::kDouble};
  AddDescriptionCorrectionView(descriptor.name_, descriptor.unit_, descriptor.type_,
                               descriptor.description_);

  auto counter1 = meter_->CreateDoubleCounter("my_counter", "desc", "unit");
  auto counter2 = meter_->CreateDoubleCounter("my_counter", "another_desc", "unit");

  counter1->Add(1, {{"key", "value1"}});
  counter2->Add(1, {{"key", "value2"}});

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    // only one metric_data object expected after correction with the view
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor descriptor{"my_counter", "desc", "unit", InstrumentType::kCounter,
                                    InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, descriptor), 2u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor), 2);
    return true;
  });

  // no warnings expected after correction with the view
  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST_F(MeterCreateInstrumentTest, SyncInstrumentWithDropAggregation)
{
  AddView("counter_one", "", InstrumentType::kCounter, "", AggregationType::kDrop);

  auto counter1 = meter_->CreateUInt64Counter("counter_one", "desc", "unit");
  auto counter2 = meter_->CreateUInt64Counter("counter_two", "desc", "unit");

  counter1->Add(1, {{"key", "value1"}});
  counter2->Add(1, {{"key", "value2"}});

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor dropped_descriptor{"counter_one", "desc", "unit", InstrumentType::kCounter,
                                            InstrumentValueType::kLong};
    InstrumentDescriptor kept_descriptor{"counter_two", "desc", "unit", InstrumentType::kCounter,
                                         InstrumentValueType::kLong};
    EXPECT_EQ(CountPointData(data, dropped_descriptor, false), 0u);
    EXPECT_EQ(CountPointData(data, kept_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, kept_descriptor), 1);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST_F(MeterCreateInstrumentTest, SyncInstrumentWithCatchAllDropAggregation)
{
  // Catch-all drop view
  AddView("*", "", InstrumentType::kCounter, "", AggregationType::kDrop);

  // Specific view for counter_one
  AddView("counter_one", "", InstrumentType::kCounter, "", AggregationType::kSum);

  auto counter1 = meter_->CreateUInt64Counter("counter_one", "desc", "unit");
  auto counter2 = meter_->CreateUInt64Counter("counter_two", "desc", "unit");

  counter1->Add(1, {{"key", "value1"}});
  counter2->Add(1, {{"key", "value2"}});

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor dropped_descriptor{"counter_two", "desc", "unit", InstrumentType::kCounter,
                                            InstrumentValueType::kLong};
    InstrumentDescriptor kept_descriptor{"counter_one", "desc", "unit", InstrumentType::kCounter,
                                         InstrumentValueType::kLong};
    EXPECT_EQ(CountPointData(data, dropped_descriptor, false), 0u);
    EXPECT_EQ(CountPointData(data, kept_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, kept_descriptor), 1);
    return true;
  });
  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST_F(MeterCreateInstrumentTest, SyncInstrumentWithConflictingAggregation)
{
  // Specific view for counter_one
  AddView("counter_one", "", InstrumentType::kCounter, "", AggregationType::kSum);

  // Catch-all view
  AddView("*", "", InstrumentType::kCounter, "", AggregationType::kSum);

  auto counter1 = meter_->CreateUInt64Counter("counter_one", "desc", "unit");

  counter1->Add(1, {{"key", "value1"}});

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor descriptor{"counter_one", "desc", "unit", InstrumentType::kCounter,
                                    InstrumentValueType::kLong};
    EXPECT_EQ(CountPointData(data, descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor), 1);
    return true;
  });
  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_TRUE(log_handler_->HasSemanticErrorWarning());
}

TEST_F(MeterCreateInstrumentTest, SyncInstrumentWithMultipleAggregations)
{
  // Specific view for counter_one to add histogram aggregation
  AddView("counter_one", "", InstrumentType::kCounter, "counter_one_as_histogram",
          AggregationType::kHistogram);

  // Another view that matches counter_one with default aggregation and a cardinality limit
  auto aggregation_config                = std::make_shared<sdk::metrics::AggregationConfig>();
  aggregation_config->cardinality_limit_ = 100;
  AddView("*", "", InstrumentType::kCounter, "", AggregationType::kDefault, aggregation_config);

  auto counter1 = meter_->CreateUInt64Counter("counter_one", "desc", "unit");

  counter1->Add(1, {{"key", "value1"}});
  counter1->Add(4, {{"key", "value2"}});

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 2);
    InstrumentDescriptor descriptor_default{"counter_one", "desc", "unit", InstrumentType::kCounter,
                                            InstrumentValueType::kLong};
    InstrumentDescriptor descriptor_histogram{"counter_one_as_histogram", "desc", "unit",
                                              InstrumentType::kCounter, InstrumentValueType::kLong};
    EXPECT_EQ(CountPointData(data, descriptor_default), 2u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor_default), 5);
    EXPECT_EQ(CountPointData(data, descriptor_histogram), 2u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor_histogram), 5);
    return true;
  });
  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST_F(MeterCreateInstrumentTest, IdenticalAsyncInstruments)
{
  auto observable_counter1 =
      meter_->CreateDoubleObservableCounter("observable_counter", "desc", "unit");
  auto observable_counter2 =
      meter_->CreateDoubleObservableCounter("observable_counter", "desc", "unit");

  auto callback1 = ObservableResultDouble{22.2, {{"key", "value1"}}};
  auto callback2 = ObservableResultDouble{55.5, {{"key", "value2"}}};

  observable_counter1->AddCallback(ObservableResultDouble::Callback, &callback1);
  observable_counter2->AddCallback(ObservableResultDouble::Callback, &callback2);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor descriptor{"observable_counter", "desc", "unit",
                                    InstrumentType::kObservableCounter,
                                    InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, descriptor), 2u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor), 77.7);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST_F(MeterCreateInstrumentTest, NameCaseConflictAsyncInstruments)
{
  auto observable_counter1 =
      meter_->CreateDoubleObservableCounter("OBServable_CounTER", "desc", "unit");
  auto observable_counter2 =
      meter_->CreateDoubleObservableCounter("observable_counter", "desc", "unit");

  auto callback1 = ObservableResultDouble{22.2, {{"key", "value1"}}};
  auto callback2 = ObservableResultDouble{55.5, {{"key", "value2"}}};

  observable_counter1->AddCallback(ObservableResultDouble::Callback, &callback1);
  observable_counter2->AddCallback(ObservableResultDouble::Callback, &callback2);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor descriptor{"OBServable_CounTER", "desc", "unit",
                                    InstrumentType::kObservableCounter,
                                    InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, descriptor), 2u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor), 77.7);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasDuplicateInstrumentWarning());
  EXPECT_TRUE(log_handler_->HasNameCaseConflictWarning());
}

TEST_F(MeterCreateInstrumentTest, ViewCorrectedNameCaseConflictAsyncInstruments)
{
  AddNameCorrectionView("OBServable_CounTER", "unit", InstrumentType::kObservableCounter,
                        "observable_counter");

  auto observable_counter1 =
      meter_->CreateDoubleObservableCounter("OBServable_CounTER", "desc", "unit");
  auto observable_counter2 =
      meter_->CreateDoubleObservableCounter("observable_counter", "desc", "unit");

  auto callback1 = ObservableResultDouble{22.2, {{"key", "value1"}}};
  auto callback2 = ObservableResultDouble{55.5, {{"key", "value2"}}};

  observable_counter1->AddCallback(ObservableResultDouble::Callback, &callback1);
  observable_counter2->AddCallback(ObservableResultDouble::Callback, &callback2);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor descriptor{"observable_counter", "desc", "unit",
                                    InstrumentType::kObservableCounter,
                                    InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, descriptor), 2u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor), 77.7);
    return true;
  });

  // no warnings expected after correction with the view
  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST_F(MeterCreateInstrumentTest, DuplicateAsyncInstrumentsByKind)
{
  auto observable_counter1 = meter_->CreateDoubleObservableCounter("observable_counter");
  auto observable_counter2 = meter_->CreateDoubleObservableGauge("observable_counter");

  auto callback1 = ObservableResultDouble{22.2, {{"key", "value1"}}};
  auto callback2 = ObservableResultDouble{55.5, {{"key", "value2"}}};

  observable_counter1->AddCallback(ObservableResultDouble::Callback, &callback1);
  observable_counter2->AddCallback(ObservableResultDouble::Callback, &callback2);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 2);
    InstrumentDescriptor counter_descriptor{"observable_counter", "", "",
                                            InstrumentType::kObservableCounter,
                                            InstrumentValueType::kDouble};
    InstrumentDescriptor gauge_descriptor{"observable_counter", "", "",
                                          InstrumentType::kObservableGauge,
                                          InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, counter_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, counter_descriptor), 22.2);
    EXPECT_EQ(CountPointData(data, gauge_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, gauge_descriptor), 55.5);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_TRUE(log_handler_->HasDuplicateInstrumentWarning());
  EXPECT_FALSE(log_handler_->HasNameCaseConflictWarning());
}

TEST_F(MeterCreateInstrumentTest, DuplicateAsyncInstrumentsByUnits)
{
  auto observable_counter1 =
      meter_->CreateDoubleObservableCounter("observable_counter", "desc", "unit");
  auto observable_counter2 =
      meter_->CreateDoubleObservableCounter("observable_counter", "desc", "another_unit");

  auto callback1 = ObservableResultDouble{22.2, {{"key", "value1"}}};
  auto callback2 = ObservableResultDouble{55.5, {{"key", "value2"}}};

  observable_counter1->AddCallback(ObservableResultDouble::Callback, &callback1);
  observable_counter2->AddCallback(ObservableResultDouble::Callback, &callback2);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 2);
    InstrumentDescriptor unit_descriptor{"observable_counter", "desc", "unit",
                                         InstrumentType::kObservableCounter,
                                         InstrumentValueType::kDouble};
    InstrumentDescriptor another_unit_descriptor{"observable_counter", "desc", "another_unit",
                                                 InstrumentType::kObservableCounter,
                                                 InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, unit_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, unit_descriptor), 22.2);
    EXPECT_EQ(CountPointData(data, another_unit_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, another_unit_descriptor), 55.5);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_TRUE(log_handler_->HasDuplicateInstrumentWarning());
  EXPECT_FALSE(log_handler_->HasNameCaseConflictWarning());
}

TEST_F(MeterCreateInstrumentTest, DuplicateAsyncInstrumentsByDescription)
{
  auto observable_counter1 =
      meter_->CreateDoubleObservableCounter("observable_counter", "desc", "unit");
  auto observable_counter2 =
      meter_->CreateDoubleObservableCounter("observable_counter", "another_desc", "unit");

  auto callback1 = ObservableResultDouble{22.2, {{"key", "value1"}}};
  auto callback2 = ObservableResultDouble{55.5, {{"key", "value2"}}};

  observable_counter1->AddCallback(ObservableResultDouble::Callback, &callback1);
  observable_counter2->AddCallback(ObservableResultDouble::Callback, &callback2);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 2);
    InstrumentDescriptor desc_descriptor{"observable_counter", "desc", "unit",
                                         InstrumentType::kObservableCounter,
                                         InstrumentValueType::kDouble};
    InstrumentDescriptor another_desc_descriptor{"observable_counter", "another_desc", "unit",
                                                 InstrumentType::kObservableCounter,
                                                 InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, desc_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, desc_descriptor), 22.2);
    EXPECT_EQ(CountPointData(data, another_desc_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, another_desc_descriptor), 55.5);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_TRUE(log_handler_->HasDuplicateInstrumentWarning());
  EXPECT_FALSE(log_handler_->HasNameCaseConflictWarning());
}

TEST_F(MeterCreateInstrumentTest, ViewCorrectedDuplicateAsyncInstrumentsByDescription)
{
  InstrumentDescriptor descriptor{"observable_counter", "desc", "unit",
                                  InstrumentType::kObservableCounter, InstrumentValueType::kDouble};

  AddDescriptionCorrectionView(descriptor.name_, descriptor.unit_, descriptor.type_,
                               descriptor.description_);

  auto observable_counter1 = meter_->CreateDoubleObservableCounter(
      descriptor.name_, descriptor.description_, descriptor.unit_);
  auto observable_counter2 =
      meter_->CreateDoubleObservableCounter(descriptor.name_, "another_desc", descriptor.unit_);

  auto callback1 = ObservableResultDouble{22.2, {{"key", "value1"}}};
  auto callback2 = ObservableResultDouble{55.5, {{"key", "value2"}}};

  observable_counter1->AddCallback(ObservableResultDouble::Callback, &callback1);
  observable_counter2->AddCallback(ObservableResultDouble::Callback, &callback2);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor descriptor{"observable_counter", "desc", "unit",
                                    InstrumentType::kObservableCounter,
                                    InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, descriptor), 2u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor), 77.7);
    return true;
  });

  // no warnings expected after correction with the view
  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST_F(MeterCreateInstrumentTest, AsyncInstrumentWithDropAggregation)
{
  AddView("observable_counter_one", "", InstrumentType::kObservableCounter, "",
          AggregationType::kDrop);

  auto observable_counter1 =
      meter_->CreateDoubleObservableCounter("observable_counter_one", "desc", "unit");
  auto observable_counter2 =
      meter_->CreateDoubleObservableCounter("observable_counter_two", "desc", "unit");

  auto callback1 = ObservableResultDouble{22.2, {{"key", "value1"}}};
  auto callback2 = ObservableResultDouble{55.5, {{"key", "value2"}}};

  observable_counter1->AddCallback(ObservableResultDouble::Callback, &callback1);
  observable_counter2->AddCallback(ObservableResultDouble::Callback, &callback2);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor dropped_descriptor{"observable_counter_one", "desc", "unit",
                                            InstrumentType::kObservableCounter,
                                            InstrumentValueType::kDouble};
    InstrumentDescriptor kept_descriptor{"observable_counter_two", "desc", "unit",
                                         InstrumentType::kObservableCounter,
                                         InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, dropped_descriptor, false), 0u);
    EXPECT_EQ(CountPointData(data, kept_descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, kept_descriptor), 55.5);
    return true;
  });

  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST_F(MeterCreateInstrumentTest, ASyncInstrumentWithCatchAllDropAggregation)
{
  // Specific view for counter_one
  AddView("observable_counter_one", "", InstrumentType::kObservableCounter, "",
          AggregationType::kSum);

  // Catch-all drop view
  AddView("*", "", InstrumentType::kObservableCounter, "", AggregationType::kDrop);

  auto observable_counter1 =
      meter_->CreateDoubleObservableCounter("observable_counter_one", "desc", "unit");
  auto observable_counter2 =
      meter_->CreateDoubleObservableCounter("observable_counter_two", "desc", "unit");

  auto callback1 = ObservableResultDouble{22.2, {{"key", "value1"}}};
  auto callback2 = ObservableResultDouble{55.5, {{"key", "value2"}}};

  observable_counter1->AddCallback(ObservableResultDouble::Callback, &callback1);
  observable_counter2->AddCallback(ObservableResultDouble::Callback, &callback2);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor descriptor_kept{"observable_counter_one", "desc", "unit",
                                         InstrumentType::kObservableCounter,
                                         InstrumentValueType::kDouble};
    InstrumentDescriptor descriptor_dropped{"observable_counter_two", "desc", "unit",
                                            InstrumentType::kObservableCounter,
                                            InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, descriptor_dropped, false), 0u);
    EXPECT_EQ(CountPointData(data, descriptor_kept), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor_kept), 22.2);
    return true;
  });
  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST_F(MeterCreateInstrumentTest, ASyncInstrumentWithConflictingAggregation)
{
  // Specific view for counter_one
  AddView("observable_counter_one", "", InstrumentType::kObservableCounter, "",
          AggregationType::kSum);

  // Catch-all non-drop view
  AddView("*", "", InstrumentType::kObservableCounter, "", AggregationType::kSum);

  auto observable_counter1 =
      meter_->CreateDoubleObservableCounter("observable_counter_one", "desc", "unit");

  auto callback1 = ObservableResultDouble{22.2, {{"key", "value1"}}};

  observable_counter1->AddCallback(ObservableResultDouble::Callback, &callback1);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 1);
    InstrumentDescriptor descriptor{"observable_counter_one", "desc", "unit",
                                    InstrumentType::kObservableCounter,
                                    InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, descriptor), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor), 22.2);
    return true;
  });
  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_TRUE(log_handler_->HasSemanticErrorWarning());
}

TEST_F(MeterCreateInstrumentTest, ASyncInstrumentWithMultipleAggregations)
{
  // Specific view for counter_one to add histogram aggregation
  AddView("observable_counter", "", InstrumentType::kObservableCounter,
          "observable_counter_as_histogram", AggregationType::kHistogram);

  // Catch-all to set the default aggregation and a cardinality limit
  auto aggregation_config                = std::make_shared<sdk::metrics::AggregationConfig>();
  aggregation_config->cardinality_limit_ = 100;
  AddView("*", "", InstrumentType::kObservableCounter, "", AggregationType::kDefault,
          aggregation_config);

  auto observable_counter1 =
      meter_->CreateDoubleObservableCounter("observable_counter", "desc", "unit");

  auto callback1 = ObservableResultDouble{55.5};

  observable_counter1->AddCallback(ObservableResultDouble::Callback, &callback1);

  metric_reader_ptr_->Collect([this](ResourceMetrics &metric_data) {
    const auto &data = metric_data.scope_metric_data_.at(0).metric_data_;
    EXPECT_EQ(data.size(), 2);
    InstrumentDescriptor descriptor_default{"observable_counter", "desc", "unit",
                                            InstrumentType::kObservableCounter,
                                            InstrumentValueType::kDouble};
    InstrumentDescriptor descriptor_histogram{"observable_counter_as_histogram", "desc", "unit",
                                              InstrumentType::kObservableCounter,
                                              InstrumentValueType::kDouble};
    EXPECT_EQ(CountPointData(data, descriptor_default), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor_default), 55.5);
    EXPECT_EQ(CountPointData(data, descriptor_histogram), 1u);
    EXPECT_DOUBLE_EQ(SumPointData(data, descriptor_histogram), 55.5);
    return true;
  });
  EXPECT_FALSE(log_handler_->HasErrors());
  EXPECT_FALSE(log_handler_->HasWarnings());
}

TEST(MeterTest, RecordAfterProviderDestructionWithCustomProcessor_NoResetInMain)
{
  std::unique_ptr<AttributesProcessor> processor(new TestProcessor());

  // MeterProvider is owned by unique_ptr for explicit control
  std::unique_ptr<MeterProvider> provider(new MeterProvider());

  // Register a View with custom processor
  std::unique_ptr<View> view(
      new View("my_counter", "", AggregationType::kSum, nullptr, std::move(processor)));
  std::unique_ptr<InstrumentSelector> instr_selector(
      new InstrumentSelector(InstrumentType::kCounter, "my_counter", ""));
  std::unique_ptr<MeterSelector> meter_selector(new MeterSelector("test_meter", "", ""));
  provider->AddView(std::move(instr_selector), std::move(meter_selector), std::move(view));

  auto meter   = provider->GetMeter("test_meter");
  auto counter = meter->CreateUInt64Counter("my_counter");

  // Move the counter to the thread
  std::atomic<bool> thread_ready{false};
  std::atomic<bool> thread_done{false};

  std::thread t([c = std::move(counter), &thread_ready, &thread_done]() mutable {
    thread_ready = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // Safe after provider destruction
    c->Add(12345, {{"thread", "after_provider_destruction"}});
    thread_done = true;
  });

  // Wait for thread to be ready
  while (!thread_ready.load())
    std::this_thread::yield();

  // Destroy the provider (and its storage etc)
  provider.reset();

  // Wait for thread to finish
  while (!thread_done.load())
    std::this_thread::yield();
  t.join();
}
