// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stddef.h>
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
      instr_desc, AggregationType::kSum, std::make_shared<DefaultAttributesProcessor>(),
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
      instr_desc, AggregationType::kDefault, std::make_shared<DefaultAttributesProcessor>(),
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
      instr_desc, AggregationType::kLastValue, std::make_shared<DefaultAttributesProcessor>(),
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

class WritableMetricStorageTestFilteredAttributesFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

// Observations the view collapses onto one attribute set are summed, not overwritten.
TEST_P(WritableMetricStorageTestFilteredAttributesFixture, TestAggregation)
{
  AggregationTemporality temporality = GetParam();

  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableCounter,
                                     InstrumentValueType::kLong};

  auto sdk_start_ts  = std::chrono::system_clock::now();
  auto collection_ts = std::chrono::system_clock::now() + std::chrono::seconds(5);

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  FilterAttributeMap allowed_attributes;
  allowed_attributes["RequestType"] = true;
  std::shared_ptr<const AttributesProcessor> attributes_processor{
      new FilteringAttributesProcessor(allowed_attributes)};

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum, attributes_processor,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

  int64_t get_count_v1 = 20;
  int64_t get_count_v2 = 10;
  int64_t put_count_v1 = 5;

  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements1 = {
      {{{"RequestType", "GET"}, {"version", "1"}}, get_count_v1},
      {{{"RequestType", "GET"}, {"version", "2"}}, get_count_v2},
      {{{"RequestType", "PUT"}, {"version", "1"}}, put_count_v1}};
  storage.RecordLong(measurements1,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  size_t collected_points = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          EXPECT_EQ(data_attr.attributes.end(), data_attr.attributes.find("version"));
          ++collected_points;
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), get_count_v1 + get_count_v2);
          }
          else
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), put_count_v1);
          }
        }
        return true;
      });
  EXPECT_EQ(collected_points, 2);

  int64_t get_count_v1_2 = 50;
  int64_t get_count_v2_2 = 30;
  int64_t put_count_v1_2 = 8;

  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> measurements2 = {
      {{{"RequestType", "GET"}, {"version", "1"}}, get_count_v1_2},
      {{{"RequestType", "GET"}, {"version", "2"}}, get_count_v2_2},
      {{{"RequestType", "PUT"}, {"version", "1"}}, put_count_v1_2}};
  storage.RecordLong(measurements2,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          EXPECT_EQ(data_attr.attributes.end(), data_attr.attributes.find("version"));
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_),
                        get_count_v1_2 + get_count_v2_2);
            }
            else
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_),
                        (get_count_v1_2 + get_count_v2_2) - (get_count_v1 + get_count_v2));
            }
          }
          else
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), put_count_v1_2);
            }
            else
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_),
                        put_count_v1_2 - put_count_v1);
            }
          }
        }
        return true;
      });
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestFilteredAttributesLong,
                         WritableMetricStorageTestFilteredAttributesFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

// Every dimension dropped: all observations collapse onto the empty attribute set.
TEST(WritableMetricStorageTestFilteredAttributes, TestUpDownCounterAllAttributesDropped)
{
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit",
                                     InstrumentType::kObservableUpDownCounter,
                                     InstrumentValueType::kDouble};

  auto sdk_start_ts  = std::chrono::system_clock::now();
  auto collection_ts = std::chrono::system_clock::now() + std::chrono::seconds(5);

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kCumulative));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  std::shared_ptr<const AttributesProcessor> attributes_processor{
      new FilteringAttributesProcessor(FilterAttributeMap{})};

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum, attributes_processor,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

  std::unordered_map<MetricAttributes, double, AttributeHashGenerator> measurements = {
      {{{"version", "1"}}, 1.0}, {{{"version", "2"}}, 2.0}, {{{"version", "3"}}, -4.0}};
  storage.RecordDouble(measurements,
                       opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  size_t collected_points = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          ++collected_points;
          EXPECT_EQ(0, data_attr.attributes.size());
          EXPECT_DOUBLE_EQ(opentelemetry::nostd::get<double>(data.value_), -1.0);
        }
        return true;
      });
  EXPECT_EQ(collected_points, 1);
}

namespace
{
// Collects the emitted sum points, so assertions don't depend on iteration order.
template <class T>
std::vector<std::pair<PointAttributes, T>> CollectSumPoints(
    opentelemetry::sdk::metrics::AsyncMetricStorage &storage,
    const std::shared_ptr<CollectorHandle> &collector,
    std::vector<std::shared_ptr<CollectorHandle>> &collectors)
{
  std::vector<std::pair<PointAttributes, T>> points;
  auto sdk_start_ts  = std::chrono::system_clock::now();
  auto collection_ts = std::chrono::system_clock::now() + std::chrono::seconds(5);
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          points.emplace_back(data_attr.attributes, opentelemetry::nostd::get<T>(data.value_));
        }
        return true;
      });
  return points;
}

std::shared_ptr<const AttributesProcessor> AllowOnly(const std::string &key)
{
  FilterAttributeMap allowed;
  allowed[key] = true;
  return std::shared_ptr<const AttributesProcessor>(new FilteringAttributesProcessor(allowed));
}

std::shared_ptr<const AttributesProcessor> DropEverything()
{
  return std::shared_ptr<const AttributesProcessor>(
      new FilteringAttributesProcessor(FilterAttributeMap{}));
}
}  // namespace

// Each callback produces its own Record() call; observations the view collapses onto one point
// must combine across them rather than overwrite.
TEST(WritableMetricStorageMultiCallback, MonotonicSumCombinesAcrossCallbacks)
{
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableCounter,
                                     InstrumentValueType::kLong};

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kCumulative));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum, DropEverything(),
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> from_first_callback = {
      {{{"version", "v1"}}, 20}};
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> from_second_callback = {
      {{{"version", "v2"}}, 10}};
  storage.RecordLong(from_first_callback,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));
  storage.RecordLong(from_second_callback,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  auto points = CollectSumPoints<int64_t>(storage, collector, collectors);
  ASSERT_EQ(1, points.size());
  EXPECT_EQ(0, points[0].first.size());
  EXPECT_EQ(30, points[0].second);
}

// Differenced per source series, so a series which stops being reported doesn't drag the merged
// group negative: 20 + 10 followed by 25 is a rise of 5, not a drop of 5.
TEST(WritableMetricStorageDisappearingSeries, MonotonicSumKeepsEarlierContribution)
{
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableCounter,
                                     InstrumentValueType::kLong};

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kCumulative));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum, DropEverything(),
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> first_round = {
      {{{"version", "v1"}}, 20}, {{{"version", "v2"}}, 10}};
  storage.RecordLong(first_round,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  auto points = CollectSumPoints<int64_t>(storage, collector, collectors);
  ASSERT_EQ(1, points.size());
  EXPECT_EQ(30, points[0].second);

  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> second_round = {
      {{{"version", "v1"}}, 25}};
  storage.RecordLong(second_round,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  points = CollectSumPoints<int64_t>(storage, collector, collectors);
  ASSERT_EQ(1, points.size());
  EXPECT_EQ(35, points[0].second);
}

// Non-monotonic sums total the round first and difference once, so the reported total follows a
// series disappearing: 100 + 50 becomes 100 when the second process exits.
TEST(WritableMetricStorageDisappearingSeries, UpDownCounterFollowsTheTotal)
{
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit",
                                     InstrumentType::kObservableUpDownCounter,
                                     InstrumentValueType::kLong};

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kCumulative));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum, AllowOnly("host"),
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> first_process = {
      {{{"host", "h1"}, {"process", "p1"}}, 100}};
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> second_process = {
      {{{"host", "h1"}, {"process", "p2"}}, 50}};
  storage.RecordLong(first_process,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));
  storage.RecordLong(second_process,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  auto points = CollectSumPoints<int64_t>(storage, collector, collectors);
  ASSERT_EQ(1, points.size());
  EXPECT_EQ(1, points[0].first.size());
  EXPECT_EQ(150, points[0].second);

  storage.RecordLong(first_process,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  points = CollectSumPoints<int64_t>(storage, collector, collectors);
  ASSERT_EQ(1, points.size());
  EXPECT_EQ(100, points[0].second);
}

// Attribute sets beyond the cardinality limit combine into `otel.metric.overflow`; no
// contribution may be dropped, including from a later callback in the same round.
TEST(WritableMetricStorageCardinalityLimit, OverflowCombinesContributions)
{
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableCounter,
                                     InstrumentValueType::kLong};

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kCumulative));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};

  constexpr size_t kCardinalityLimit = 2;
  AggregationConfig aggregation_config{kCardinalityLimit};

  opentelemetry::sdk::metrics::AsyncMetricStorage storage(
      instr_desc, AggregationType::kSum, AllowOnly("id"),
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      &aggregation_config);

  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> from_first_callback = {
      {{{"id", "a"}, {"version", "v1"}}, 1},
      {{{"id", "b"}, {"version", "v1"}}, 2},
      {{{"id", "c"}, {"version", "v1"}}, 3}};
  std::unordered_map<MetricAttributes, int64_t, AttributeHashGenerator> from_second_callback = {
      {{{"id", "d"}, {"version", "v1"}}, 4}, {{{"id", "e"}, {"version", "v1"}}, 5}};
  storage.RecordLong(from_first_callback,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));
  storage.RecordLong(from_second_callback,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  auto points = CollectSumPoints<int64_t>(storage, collector, collectors);

  ASSERT_EQ(kCardinalityLimit + 1, points.size());

  int64_t total          = 0;
  bool has_overflow      = false;
  int64_t overflow_value = 0;
  for (const auto &point : points)
  {
    total += point.second;
    const auto overflow_it = point.first.find(kAttributesLimitOverflowKey);
    if (overflow_it != point.first.end())
    {
      has_overflow = true;
      EXPECT_EQ(true, opentelemetry::nostd::get<bool>(overflow_it->second));
      overflow_value = point.second;
    }
  }

  EXPECT_TRUE(has_overflow);
  EXPECT_EQ(1 + 2 + 3 + 4 + 5, total);
  EXPECT_GT(overflow_value, 0);
}

}  // namespace
