// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "common.h"

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/sdk/metrics/async_instruments.h"
#include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/observer_result.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/metrics/state/async_metric_storage.h"
#include "opentelemetry/sdk/metrics/state/metric_collector.h"
#include "opentelemetry/sdk/metrics/state/observable_registry.h"

#include <gtest/gtest.h>
#include <cstdint>
#include <memory>
#include <vector>

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::resource;
using namespace opentelemetry::common;
namespace nostd = opentelemetry::nostd;

using M = std::map<std::string, std::string>;

class WritableMetricStorageTestFixture : public ::testing::TestWithParam<AggregationTemporality>
{};

class WritableMetricStorageTestUpDownFixture
    : public ::testing::TestWithParam<AggregationTemporality>
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

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum, ExemplarReservoir::GetNoExemplarReservoir(), nullptr);
  int64_t get_count1                                                                  = 20;
  int64_t put_count1                                                                  = 10;
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements1 = {
      {{{"RequestType", "GET"}}, get_count1}, {{{"RequestType", "PUT"}}, put_count1}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), get_count1);
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), put_count1);
          }
        }
        return true;
      });
  // subsequent recording after collection shouldn't fail
  // monotonic increasing values;
  int64_t get_count2 = 50;
  int64_t put_count2 = 70;

  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements2 = {
      {{{"RequestType", "GET"}}, get_count2}, {{{"RequestType", "PUT"}}, put_count2}};
  storage.RecordLong(measurements2,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), get_count2);
            }
            else
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), get_count2 - get_count1);
            }
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), put_count2);
            }
            else
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), put_count2 - put_count1);
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

TEST_P(WritableMetricStorageTestUpDownFixture, TestAggregation)
{
  AggregationTemporality temporality = GetParam();

  InstrumentDescriptor instr_desc = {"name", "desc", "1unit",
                                     InstrumentType::kObservableUpDownCounter,
                                     InstrumentValueType::kLong};

  auto sdk_start_ts = std::chrono::system_clock::now();
  // Some computation here
  auto collection_ts = std::chrono::system_clock::now() + std::chrono::seconds(5);

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kDefault, ExemplarReservoir::GetNoExemplarReservoir(), nullptr);
  int64_t get_count1                                                                  = 20;
  int64_t put_count1                                                                  = 10;
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements1 = {
      {{{"RequestType", "GET"}}, get_count1}, {{{"RequestType", "PUT"}}, put_count1}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), get_count1);
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), put_count1);
          }
        }
        return true;
      });
  // subsequent recording after collection shouldn't fail
  // monotonic increasing values;
  int64_t get_count2 = -50;
  int64_t put_count2 = -70;

  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements2 = {
      {{{"RequestType", "GET"}}, get_count2}, {{{"RequestType", "PUT"}}, put_count2}};
  storage.RecordLong(measurements2,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), get_count2);
            }
            else
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), get_count2 - get_count1);
            }
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), put_count2);
            }
            else
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), put_count2 - put_count1);
            }
          }
        }
        return true;
      });
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestUpDownLong,
                         WritableMetricStorageTestUpDownFixture,
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

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, ExemplarReservoir::GetNoExemplarReservoir(),
      nullptr);
  int64_t freq_cpu0                                                                   = 3;
  int64_t freq_cpu1                                                                   = 5;
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements1 = {
      {{{"CPU", "0"}}, freq_cpu0}, {{{"CPU", "1"}}, freq_cpu1}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData metric_data) {
        for (auto data_attr : metric_data.point_data_attr_)
        {
          auto data = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(data_attr.attributes.find("CPU")->second) ==
              "0")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), freq_cpu0);
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("CPU")->second) == "1")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), freq_cpu1);
          }
        }
        return true;
      });

  freq_cpu0 = 6;
  freq_cpu1 = 8;

  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements2 = {
      {{{"CPU", "0"}}, freq_cpu0}, {{{"CPU", "1"}}, freq_cpu1}};
  storage.RecordLong(measurements2,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData metric_data) {
        for (auto data_attr : metric_data.point_data_attr_)
        {
          auto data = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(data_attr.attributes.find("CPU")->second) ==
              "0")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), freq_cpu0);
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("CPU")->second) == "1")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), freq_cpu1);
          }
        }
        return true;
      });
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestObservableGaugeFixtureLong,
                         WritableMetricStorageTestObservableGaugeFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));
