// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "common.h"

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/state/async_metric_storage.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/sdk/metrics/state/filtered_ordered_attribute_map.h"
#include "opentelemetry/sdk/metrics/state/metric_collector.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#endif

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::resource;
using namespace opentelemetry::common;

namespace
{

class AsyncWritableMetricStorageTestFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

class WritableMetricStorageTestUpDownFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

class WritableMetricStorageTestObservableGaugeFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

class AsyncMetricStorageStaleAttributeFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

TEST_P(AsyncWritableMetricStorageTestFixture, TestAggregation)
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
      instr_desc, AggregationType::kSum,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);
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
                         AsyncWritableMetricStorageTestFixture,
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
      instr_desc, AggregationType::kDefault,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);
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
  // Note: When the cardinality limit is set to n, the attributes hashmap emits n-1 distinct
  // attribute sets, plus an overflow bucket for additional attributes. The test logic below is made
  // generic to succeed for either n or n-1 total cardinality. If this behavior is unexpected,
  // please investigate and file an issue.
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
      instr_desc, AggregationType::kLastValue,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);
  int64_t freq_cpu0                                                                   = 3;
  int64_t freq_cpu1                                                                   = 5;
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements1 = {
      {{{"CPU", "0"}}, freq_cpu0}, {{{"CPU", "1"}}, freq_cpu1}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
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
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
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

// Async instruments must NOT carry forward attribute sets that were not reported by the callback in
// the current collection cycle.
TEST_P(AsyncMetricStorageStaleAttributeFixture, StaleAttributeSetDropped)
{
  const AggregationTemporality temporality = GetParam();

  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableCounter,
                                     InstrumentValueType::kLong};

  auto sdk_start_ts = std::chrono::system_clock::now();
  // Some computation here
  auto collection_ts = sdk_start_ts + std::chrono::seconds(5);

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

  // Collection 1: both GET and PUT reported.
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements1 = {
      {{{"RequestType", "GET"}}, 10}, {{{"RequestType", "PUT"}}, 5}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  int get_count = 0;
  int put_count = 0;
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [&](const MetricData &metric_data) {
                    for (const auto &data_attr : metric_data.point_data_attr_)
                    {
                      const auto &key = opentelemetry::nostd::get<std::string>(
                          data_attr.attributes.find("RequestType")->second);
                      if (key == "GET")
                        get_count++;
                      else if (key == "PUT")
                        put_count++;
                    }
                    return true;
                  });
  EXPECT_EQ(get_count, 1);
  EXPECT_EQ(put_count, 1);

  // Collection 2: only GET reported – PUT is dropped by callback.
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements2 = {
      {{{"RequestType", "GET"}}, 20}};
  storage.RecordLong(measurements2,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  get_count         = 0;
  put_count         = 0;
  int64_t get_value = 0;
  storage.Collect(collector.get(), collectors, sdk_start_ts,
                  collection_ts + std::chrono::seconds(5), [&](const MetricData &metric_data) {
                    for (const auto &data_attr : metric_data.point_data_attr_)
                    {
                      const auto &key = opentelemetry::nostd::get<std::string>(
                          data_attr.attributes.find("RequestType")->second);
                      if (key == "GET")
                      {
                        get_count++;
                        get_value = opentelemetry::nostd::get<int64_t>(
                            opentelemetry::nostd::get<SumPointData>(data_attr.point_data).value_);
                      }
                      else if (key == "PUT")
                      {
                        put_count++;
                      }
                    }
                    return true;
                  });

  // PUT must not appear – it was absent from the callback this cycle.
  EXPECT_EQ(put_count, 0) << "Stale PUT attribute set must be dropped";
  EXPECT_EQ(get_count, 1);
  // Cumulative exports the absolute value (20); delta exports the increment since last seen (10).
  const int64_t expected_get_value = (temporality == AggregationTemporality::kCumulative) ? 20 : 10;
  EXPECT_EQ(get_value, expected_get_value);
}

// An attribute set that disappears for one collection cycle and then reappears must preserve the
// cumulative baseline across the absent cycle. On reappearance cumulative exports the full absolute
// value, while delta exports only the increment since the attribute was last seen.
TEST_P(AsyncMetricStorageStaleAttributeFixture, AttributeReappearanceAfterGap)
{
  const AggregationTemporality temporality = GetParam();

  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableCounter,
                                     InstrumentValueType::kLong};

  auto sdk_start_ts = std::chrono::system_clock::now();
  // Some computation here
  auto collection_ts = sdk_start_ts + std::chrono::seconds(5);

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

  // Collection 1: A=10 -> both temporalities export 10.
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements1 = {
      {{{"attr", "A"}}, 10}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  int64_t value = -1;
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [&](const MetricData &metric_data) {
                    for (const auto &data_attr : metric_data.point_data_attr_)
                    {
                      value = opentelemetry::nostd::get<int64_t>(
                          opentelemetry::nostd::get<SumPointData>(data_attr.point_data).value_);
                    }
                    return true;
                  });
  EXPECT_EQ(value, 10);

  // Collection 2: attribute A absent – nothing recorded, nothing emitted.
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements2;
  storage.RecordLong(measurements2,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  int attr_count = 0;
  storage.Collect(collector.get(), collectors, sdk_start_ts,
                  collection_ts + std::chrono::seconds(5), [&](const MetricData &metric_data) {
                    attr_count += static_cast<int>(metric_data.point_data_attr_.size());
                    return true;
                  });
  EXPECT_EQ(attr_count, 0) << "No data points expected when attribute set is absent";

  // Collection 3: A reappears with absolute value 30.
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements3 = {
      {{{"attr", "A"}}, 30}};
  storage.RecordLong(measurements3,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  value = -1;
  storage.Collect(collector.get(), collectors, sdk_start_ts,
                  collection_ts + std::chrono::seconds(10), [&](const MetricData &metric_data) {
                    for (const auto &data_attr : metric_data.point_data_attr_)
                    {
                      value = opentelemetry::nostd::get<int64_t>(
                          opentelemetry::nostd::get<SumPointData>(data_attr.point_data).value_);
                    }
                    return true;
                  });
  // Baseline (10) preserved across the gap: cumulative = 30, delta = 30 - 10 = 20.
  const int64_t expected_value = (temporality == AggregationTemporality::kCumulative) ? 30 : 20;
  EXPECT_EQ(value, expected_value)
      << "After a gap, reappearing attribute must preserve the baseline across the absent cycle";
}

// Stale suppression must apply to every collector, not just the one that drains the shared delta
// first. With two collectors, an attribute dropped by the callback must disappear from both
// collectors' exports while the still-reported attribute keeps its correct value for both.
TEST_P(AsyncMetricStorageStaleAttributeFixture, StaleAttributeSetDroppedMultiCollector)
{
  const AggregationTemporality temporality = GetParam();

  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableCounter,
                                     InstrumentValueType::kLong};

  auto sdk_start_ts   = std::chrono::system_clock::now();
  auto collection_ts1 = sdk_start_ts + std::chrono::seconds(5);
  auto collection_ts2 = sdk_start_ts + std::chrono::seconds(10);

  std::shared_ptr<CollectorHandle> collector1(new MockCollectorHandle(temporality));
  std::shared_ptr<CollectorHandle> collector2(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector1);
  collectors.push_back(collector2);

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

  auto collect_request_types = [&](CollectorHandle *collector,
                                   opentelemetry::common::SystemTimestamp collection_ts,
                                   int &get_count, int &put_count, int64_t &get_value) {
    get_count = 0;
    put_count = 0;
    get_value = 0;
    storage.Collect(collector, collectors, sdk_start_ts, collection_ts,
                    [&](const MetricData &metric_data) {
                      for (const auto &data_attr : metric_data.point_data_attr_)
                      {
                        const auto &key = opentelemetry::nostd::get<std::string>(
                            data_attr.attributes.find("RequestType")->second);
                        if (key == "GET")
                        {
                          get_count++;
                          get_value = opentelemetry::nostd::get<int64_t>(
                              opentelemetry::nostd::get<SumPointData>(data_attr.point_data).value_);
                        }
                        else if (key == "PUT")
                        {
                          put_count++;
                        }
                      }
                      return true;
                    });
  };

  // Collection 1: both GET and PUT reported – both collectors see both attribute sets.
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements1 = {
      {{{"RequestType", "GET"}}, 10}, {{{"RequestType", "PUT"}}, 5}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  int c1_get = 0, c1_put = 0, c2_get = 0, c2_put = 0;
  int64_t c1_get_value = 0, c2_get_value = 0;
  collect_request_types(collector1.get(), collection_ts1, c1_get, c1_put, c1_get_value);
  collect_request_types(collector2.get(), collection_ts1, c2_get, c2_put, c2_get_value);
  EXPECT_EQ(c1_get, 1);
  EXPECT_EQ(c1_put, 1);
  EXPECT_EQ(c2_get, 1);
  EXPECT_EQ(c2_put, 1) << "Second collector must observe PUT even though the first drained the "
                          "shared delta";

  // Collection 2: only GET reported – PUT is dropped by the callback.
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements2 = {
      {{{"RequestType", "GET"}}, 20}};
  storage.RecordLong(measurements2,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  collect_request_types(collector1.get(), collection_ts2, c1_get, c1_put, c1_get_value);
  collect_request_types(collector2.get(), collection_ts2, c2_get, c2_put, c2_get_value);

  // Cumulative exports the absolute value (20); delta exports the increment since last seen (10).
  const int64_t expected_get_value = (temporality == AggregationTemporality::kCumulative) ? 20 : 10;

  EXPECT_EQ(c1_put, 0) << "Stale PUT must be dropped for the first collector";
  EXPECT_EQ(c2_put, 0) << "Stale PUT must be dropped for the second collector too";
  EXPECT_EQ(c1_get, 1);
  EXPECT_EQ(c2_get, 1);
  EXPECT_EQ(c1_get_value, expected_get_value);
  EXPECT_EQ(c2_get_value, expected_get_value)
      << "Second collector must independently receive the still-reported GET value";
}

// A collector that skips a collection cycle must not lose values observed during its interval.
// Deltas drained by another collector are stashed for every collector, so when the lagging
// collector finally collects it receives the accumulated value — even though delta_metrics is empty
// at that moment (already drained by the other collector). This is the exact case the per-collector
// observed set fixes: suppression is driven by the collector's own unreported deltas, not by
// delta_metrics.
TEST_P(AsyncMetricStorageStaleAttributeFixture, MultiCollectorLaggingCollector)
{
  const AggregationTemporality temporality = GetParam();

  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableCounter,
                                     InstrumentValueType::kLong};

  auto sdk_start_ts   = std::chrono::system_clock::now();
  auto collection_ts1 = sdk_start_ts + std::chrono::seconds(5);
  auto collection_ts2 = sdk_start_ts + std::chrono::seconds(10);

  std::shared_ptr<CollectorHandle> collector1(new MockCollectorHandle(temporality));
  std::shared_ptr<CollectorHandle> collector2(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector1);
  collectors.push_back(collector2);

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

  auto collect = [&](CollectorHandle *collector,
                     opentelemetry::common::SystemTimestamp collection_ts, int &count,
                     int64_t &value_out) {
    count     = 0;
    value_out = 0;
    storage.Collect(collector, collectors, sdk_start_ts, collection_ts,
                    [&](const MetricData &metric_data) {
                      for (const auto &data_attr : metric_data.point_data_attr_)
                      {
                        count++;
                        value_out = opentelemetry::nostd::get<int64_t>(
                            opentelemetry::nostd::get<SumPointData>(data_attr.point_data).value_);
                      }
                      return true;
                    });
  };

  int c1_count = 0, c2_count = 0;
  int64_t c1_value = 0, c2_value = 0;

  // Cycle 1: A=10 observed. Only collector1 collects; collector2 lags behind.
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements1 = {
      {{{"attr", "A"}}, 10}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));
  collect(collector1.get(), collection_ts1, c1_count, c1_value);
  EXPECT_EQ(c1_count, 1);
  EXPECT_EQ(c1_value, 10);

  // Cycle 2: A=20 observed. Only collector1 collects again; collector2 still lags.
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements2 = {
      {{{"attr", "A"}}, 20}};
  storage.RecordLong(measurements2,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));
  collect(collector1.get(), collection_ts2, c1_count, c1_value);
  EXPECT_EQ(c1_count, 1);
  // Cumulative: absolute 20; delta: increment since previous collection (20 - 10 = 10).
  EXPECT_EQ(c1_value, (temporality == AggregationTemporality::kCumulative) ? 20 : 10);

  // collector2 finally collects. delta_metrics is empty now (collector1 already drained it), but
  // collector2's stash accumulated both cycles -> it must still receive the value.
  collect(collector2.get(), collection_ts2, c2_count, c2_value);
  EXPECT_EQ(c2_count, 1) << "Lagging collector must not drop a value observed during its interval";
  // Both temporalities yield 20 here: cumulative absolute is 20, and delta since collector2's first
  // (never) collection is the full accumulated 10 + 10 = 20.
  EXPECT_EQ(c2_value, 20)
      << "Lagging collector must receive the accumulated value from the cycles it missed";
}

// With more than one collector, delta temporality no longer short-circuits on the single-collector
// fast path — it goes through the slow path instead. This exercises the delta branch that sets
// start_ts to the previous collection's timestamp, and verifies both collectors independently
// receive the correct value. Under cumulative every collector must observe the absolute value even
// though only the first collector of a cycle drains the shared delta (the per-collector observed
// set drives stale suppression, not delta_metrics), and start_ts stays at the SDK start.
TEST_P(AsyncMetricStorageStaleAttributeFixture, MultiCollector)
{
  const AggregationTemporality temporality = GetParam();

  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableCounter,
                                     InstrumentValueType::kLong};

  auto sdk_start_ts   = std::chrono::system_clock::now();
  auto collection_ts1 = sdk_start_ts + std::chrono::seconds(5);
  auto collection_ts2 = sdk_start_ts + std::chrono::seconds(10);

  std::shared_ptr<CollectorHandle> collector1(new MockCollectorHandle(temporality));
  std::shared_ptr<CollectorHandle> collector2(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector1);
  collectors.push_back(collector2);

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

  auto collect_value = [&](CollectorHandle *collector,
                           opentelemetry::common::SystemTimestamp collection_ts, int64_t &value_out,
                           opentelemetry::common::SystemTimestamp &start_ts_out,
                           opentelemetry::common::SystemTimestamp &end_ts_out) {
    storage.Collect(collector, collectors, sdk_start_ts, collection_ts,
                    [&](const MetricData &metric_data) {
                      start_ts_out = metric_data.start_ts;
                      end_ts_out   = metric_data.end_ts;
                      for (const auto &data_attr : metric_data.point_data_attr_)
                      {
                        value_out = opentelemetry::nostd::get<int64_t>(
                            opentelemetry::nostd::get<SumPointData>(data_attr.point_data).value_);
                      }
                      return true;
                    });
  };

  // Cycle 1: A=10 observed once, both collectors drain the same value -> each sees 10.
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements1 = {
      {{{"attr", "A"}}, 10}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  int64_t c1_value = -1;
  int64_t c2_value = -1;
  opentelemetry::common::SystemTimestamp c1_start;
  opentelemetry::common::SystemTimestamp c2_start;
  opentelemetry::common::SystemTimestamp c1_end;
  opentelemetry::common::SystemTimestamp c2_end;
  collect_value(collector1.get(), collection_ts1, c1_value, c1_start, c1_end);
  collect_value(collector2.get(), collection_ts1, c2_value, c2_start, c2_end);
  EXPECT_EQ(c1_value, 10);
  EXPECT_EQ(c2_value, 10) << "Second collector must observe the value even though the first "
                             "collector drained the shared delta";
  EXPECT_EQ(c1_end, opentelemetry::common::SystemTimestamp(collection_ts1))
      << "end_ts must equal the collection timestamp";
  EXPECT_EQ(c2_end, opentelemetry::common::SystemTimestamp(collection_ts1))
      << "end_ts must equal the collection timestamp";

  // Cycle 2: A=30 observed once.
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements2 = {
      {{{"attr", "A"}}, 30}};
  storage.RecordLong(measurements2,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  c1_value = -1;
  c2_value = -1;
  collect_value(collector1.get(), collection_ts2, c1_value, c1_start, c1_end);
  collect_value(collector2.get(), collection_ts2, c2_value, c2_start, c2_end);

  // end_ts always advances to the current collection timestamp, for both temporalities.
  EXPECT_EQ(c1_end, opentelemetry::common::SystemTimestamp(collection_ts2))
      << "end_ts must equal the current collection timestamp";
  EXPECT_EQ(c2_end, opentelemetry::common::SystemTimestamp(collection_ts2))
      << "end_ts must equal the current collection timestamp";

  if (temporality == AggregationTemporality::kCumulative)
  {
    // Cumulative reports the absolute value and keeps start_ts at the SDK start.
    EXPECT_EQ(c1_value, 30);
    EXPECT_EQ(c2_value, 30) << "Second collector must independently receive the absolute value";
    EXPECT_EQ(c1_start, opentelemetry::common::SystemTimestamp(sdk_start_ts))
        << "Cumulative start_ts must stay at the SDK start";
    EXPECT_EQ(c2_start, opentelemetry::common::SystemTimestamp(sdk_start_ts))
        << "Cumulative start_ts must stay at the SDK start";
  }
  else
  {
    // Delta reports the increment since each collector's previous collection.
    EXPECT_EQ(c1_value, 20) << "Delta since previous collection must be 30 - 10 = 20";
    EXPECT_EQ(c2_value, 20) << "Second collector must independently receive the same increment";
    // The slow-path delta branch sets start_ts to the previous collection's timestamp.
    EXPECT_EQ(c1_start, opentelemetry::common::SystemTimestamp(collection_ts1))
        << "Delta start_ts must continue from the previous collection";
    EXPECT_EQ(c2_start, opentelemetry::common::SystemTimestamp(collection_ts1))
        << "Delta start_ts must continue from the previous collection";
  }
}

INSTANTIATE_TEST_SUITE_P(AsyncMetricStorageRegression,
                         AsyncMetricStorageStaleAttributeFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

}  // namespace
