// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/exemplar/no_exemplar_reservoir.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"

#  include <gtest/gtest.h>
#  include <map>

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::common;
using M = std::map<std::string, std::string>;

class MockCollectorHandle : public CollectorHandle
{
public:
  MockCollectorHandle(AggregationTemporality temp) : temporality(temp) {}

  AggregationTemporality GetAggregationTemporality() noexcept override { return temporality; }

private:
  AggregationTemporality temporality;
};

class WritableMetricStorageTestFixture : public ::testing::TestWithParam<AggregationTemporality>
{};

TEST_P(WritableMetricStorageTestFixture, LongSumAggregation)
{
  AggregationTemporality temporality = GetParam();
  auto sdk_start_ts                  = std::chrono::system_clock::now();
  long expected_total_get_requests   = 0;
  long expected_total_put_requests   = 0;
  InstrumentDescriptor instr_desc    = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kLong};
  std::map<std::string, std::string> attributes_get = {{"RequestType", "GET"}};
  std::map<std::string, std::string> attributes_put = {{"RequestType", "PUT"}};

  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kSum, new DefaultAttributesProcessor(),
      NoExemplarReservoir::GetNoExemplarReservoir());

  storage.RecordLong(10l, KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
                     opentelemetry::context::Context{});
  expected_total_get_requests += 10;

  EXPECT_NO_THROW(storage.RecordLong(
      30l, KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
      opentelemetry::context::Context{}));
  expected_total_put_requests += 30;

  storage.RecordLong(20l, KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
                     opentelemetry::context::Context{});
  expected_total_get_requests += 20;

  EXPECT_NO_THROW(storage.RecordLong(
      40l, KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
      opentelemetry::context::Context{}));
  expected_total_put_requests += 40;

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  // Some computation here
  auto collection_ts      = std::chrono::system_clock::now();
  size_t count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData data) {
        for (auto data_attr : data.point_data_attr_)
        {
          auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), expected_total_get_requests);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), expected_total_put_requests);
            count_attributes++;
          }
        }
        return true;
      });

  // In case of delta temporarily, subsequent collection would contain new data points, so resetting
  // the counts
  if (temporality == AggregationTemporality::kDelta)
  {
    expected_total_get_requests = 0;
    expected_total_put_requests = 0;
  }

  EXPECT_NO_THROW(storage.RecordLong(
      50l, KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
      opentelemetry::context::Context{}));
  expected_total_get_requests += 50;
  EXPECT_NO_THROW(storage.RecordLong(
      40l, KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
      opentelemetry::context::Context{}));
  expected_total_put_requests += 40;

  collection_ts    = std::chrono::system_clock::now();
  count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData data) {
        for (auto data_attr : data.point_data_attr_)
        {
          auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), expected_total_get_requests);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), expected_total_put_requests);
            count_attributes++;
          }
        }
        return true;
      });
}
INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestLong,
                         WritableMetricStorageTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

TEST_P(WritableMetricStorageTestFixture, DoubleSumAggregation)
{
  AggregationTemporality temporality = GetParam();
  auto sdk_start_ts                  = std::chrono::system_clock::now();
  double expected_total_get_requests = 0;
  double expected_total_put_requests = 0;
  InstrumentDescriptor instr_desc    = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kDouble};
  std::map<std::string, std::string> attributes_get = {{"RequestType", "GET"}};
  std::map<std::string, std::string> attributes_put = {{"RequestType", "PUT"}};

  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kSum, new DefaultAttributesProcessor(),
      NoExemplarReservoir::GetNoExemplarReservoir());

  storage.RecordDouble(10.0,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
                       opentelemetry::context::Context{});
  expected_total_get_requests += 10;

  EXPECT_NO_THROW(storage.RecordDouble(
      30.0, KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
      opentelemetry::context::Context{}));
  expected_total_put_requests += 30;

  storage.RecordDouble(20.0,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
                       opentelemetry::context::Context{});
  expected_total_get_requests += 20;

  EXPECT_NO_THROW(storage.RecordDouble(
      40.0, KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
      opentelemetry::context::Context{}));
  expected_total_put_requests += 40;

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  // Some computation here
  auto collection_ts      = std::chrono::system_clock::now();
  size_t count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData data) {
        for (auto data_attr : data.point_data_attr_)
        {
          auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.value_), expected_total_get_requests);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.value_), expected_total_put_requests);
            count_attributes++;
          }
        }
        return true;
      });

  // In case of delta temporarily, subsequent collection would contain new data points, so resetting
  // the counts
  if (temporality == AggregationTemporality::kDelta)
  {
    expected_total_get_requests = 0;
    expected_total_put_requests = 0;
  }

  EXPECT_NO_THROW(storage.RecordDouble(
      50.0, KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
      opentelemetry::context::Context{}));
  expected_total_get_requests += 50;
  EXPECT_NO_THROW(storage.RecordDouble(
      40.0, KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
      opentelemetry::context::Context{}));
  expected_total_put_requests += 40;

  collection_ts    = std::chrono::system_clock::now();
  count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData data) {
        for (auto data_attr : data.point_data_attr_)
        {
          auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.value_), expected_total_get_requests);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.value_), expected_total_put_requests);
            count_attributes++;
          }
        }
        return true;
      });
}
INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestDouble,
                         WritableMetricStorageTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

#endif
