// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/async_metric_storage.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/async_instruments.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/metrics/metric_exporter.h"
#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include "opentelemetry/sdk/metrics/observer_result.h"
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"
#  include "opentelemetry/sdk/metrics/state/observable_registry.h"

#  include <gtest/gtest.h>
#  include <memory>
#  include <vector>

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::resource;

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::common;
using M         = std::map<std::string, std::string>;
namespace nostd = opentelemetry::nostd;

class MockCollectorHandle : public CollectorHandle
{
public:
  MockCollectorHandle(AggregationTemporality temp) : temporality(temp) {}

  virtual ~MockCollectorHandle() = default;

  AggregationTemporality GetAggregationTemporality(
      InstrumentType /* instrument_type */) noexcept override
  {
    return temporality;
  }

private:
  AggregationTemporality temporality;
};

class WritableMetricStorageTestFixture : public ::testing::TestWithParam<AggregationTemporality>
{};

class WritableMetricStorageTestObservableGaugeFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

TEST_P(WritableMetricStorageTestFixture, TestAggregation)
{
  AggregationTemporality temporality = GetParam();

  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableCounter,
                                     InstrumentValueType::kLong};

  auto sdk_start_ts = std::chrono::system_clock::now();
  // Some computation here
  auto collection_ts = std::chrono::system_clock::now() + std::chrono::seconds(5);

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  std::unique_ptr<AttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum, default_attributes_processor.get(),
      std::shared_ptr<opentelemetry::sdk::metrics::AggregationConfig>{});
  long get_count1                                                                  = 20l;
  long put_count1                                                                  = 10l;
  size_t attribute_count                                                           = 2;
  std::unordered_map<MetricAttributes, long, AttributeHashGenerator> measurements1 = {
      {{{"RequestType", "GET"}}, get_count1}, {{{"RequestType", "PUT"}}, put_count1}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [&](const MetricData data) {
                    for (auto data_attr : data.point_data_attr_)
                    {
                      auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
                      if (opentelemetry::nostd::get<std::string>(
                              data_attr.attributes.find("RequestType")->second) == "GET")
                      {
                        EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), get_count1);
                      }
                      else if (opentelemetry::nostd::get<std::string>(
                                   data_attr.attributes.find("RequestType")->second) == "PUT")
                      {
                        EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), put_count1);
                      }
                    }
                    return true;
                  });
  // subsequent recording after collection shouldn't fail
  // monotonic increasing values;
  long get_count2 = 50l;
  long put_count2 = 70l;

  std::unordered_map<MetricAttributes, long, AttributeHashGenerator> measurements2 = {
      {{{"RequestType", "GET"}}, get_count2}, {{{"RequestType", "PUT"}}, put_count2}};
  storage.RecordLong(measurements2,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData data) {
        for (auto data_attr : data.point_data_attr_)
        {
          auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), get_count2);
            }
            else
            {
              EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), get_count2 - get_count1);
            }
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), put_count2);
            }
            else
            {
              EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), put_count2 - put_count1);
            }
          }
        }
        return true;
      });
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestLong,
                         WritableMetricStorageTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

TEST_P(WritableMetricStorageTestObservableGaugeFixture, TestAggregation)
{
  AggregationTemporality temporality = GetParam();

  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableGauge,
                                     InstrumentValueType::kLong};

  auto sdk_start_ts = std::chrono::system_clock::now();
  // Some computation here
  auto collection_ts = std::chrono::system_clock::now() + std::chrono::seconds(5);

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  std::unique_ptr<AttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, default_attributes_processor.get(),
      std::shared_ptr<opentelemetry::sdk::metrics::AggregationConfig>{});
  long freq_cpu0                                                                   = 3l;
  long freq_cpu1                                                                   = 5l;
  size_t attribute_count                                                           = 2;
  std::unordered_map<MetricAttributes, long, AttributeHashGenerator> measurements1 = {
      {{{"CPU", "0"}}, freq_cpu0}, {{{"CPU", "1"}}, freq_cpu1}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData data) {
        for (auto data_attr : data.point_data_attr_)
        {
          auto data = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(data_attr.attributes.find("CPU")->second) ==
              "0")
          {
            EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), freq_cpu0);
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("CPU")->second) == "1")
          {
            EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), freq_cpu1);
          }
        }
        return true;
      });

  freq_cpu0 = 6l;
  freq_cpu1 = 8l;

  std::unordered_map<MetricAttributes, long, AttributeHashGenerator> measurements2 = {
      {{{"CPU", "0"}}, freq_cpu0}, {{{"CPU", "1"}}, freq_cpu1}};
  storage.RecordLong(measurements2,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData data) {
        for (auto data_attr : data.point_data_attr_)
        {
          auto data = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(data_attr.attributes.find("CPU")->second) ==
              "0")
          {
            EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), freq_cpu0);
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("CPU")->second) == "1")
          {
            EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), freq_cpu1);
          }
        }
        return true;
      });
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestObservableGaugeFixtureLong,
                         WritableMetricStorageTestObservableGaugeFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

#endif
