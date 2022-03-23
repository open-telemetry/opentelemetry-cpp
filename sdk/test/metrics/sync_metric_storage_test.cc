// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"

#  include <gtest/gtest.h>
#  include <map>

#  include <iostream>

using namespace opentelemetry::sdk::metrics;
using M = std::map<std::string, std::string>;

class MockCollectorHandle : public CollectorHandle
{
public:
  MockCollectorHandle(AggregationTemporarily temp) : temporarily(temp) {}

  AggregationTemporarily GetAggregationTemporarily() noexcept override { return temporarily; }

private:
  AggregationTemporarily temporarily;
};

TEST(WritableMetricStorageTest, LongSumCummulative)
{
  auto sdk_start_ts               = std::chrono::system_clock::now();
  long expected_sum               = 0;
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kLong};

  opentelemetry::sdk::metrics::SyncMetricStorage storage(instr_desc, AggregationType::kSum,
                                                         new DefaultAttributesProcessor());
  EXPECT_NO_THROW(storage.RecordLong(10l));
  expected_sum += 10;

  EXPECT_NO_THROW(storage.RecordLong(30l));
  expected_sum += 30;
  /*EXPECT_NO_THROW(storage.RecordLong(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));*/

  EXPECT_NO_THROW(storage.RecordDouble(10.10, opentelemetry::common::KeyValueIterableView<M>({})));
  opentelemetry::common::SystemTimestamp sdk_start_time = std::chrono::system_clock::now();
  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporarily::kCumulative));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  // Some computation here
  auto collection_ts = std::chrono::system_clock::now();
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [expected_sum](const MetricData data) {
                    for (auto data_attr : data.point_data_attr_)
                    {
                      auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
                      EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), expected_sum);
                    }
                    return true;
                  });

  EXPECT_NO_THROW(storage.RecordLong(50l));
  expected_sum += 50;
  EXPECT_NO_THROW(storage.RecordLong(20l));
  expected_sum += 20;

  collection_ts = std::chrono::system_clock::now();
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [expected_sum](const MetricData data) {
                    for (auto data_attr : data.point_data_attr_)
                    {
                      auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
                      EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), expected_sum);
                    }
                    return true;
                  });
}

TEST(WritableMetricStorageTest, LongSumDelta)
{
  auto sdk_start_ts               = std::chrono::system_clock::now();
  long expected_sum               = 0;
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kLong};

  opentelemetry::sdk::metrics::SyncMetricStorage storage(instr_desc, AggregationType::kSum,
                                                         new DefaultAttributesProcessor());
  EXPECT_NO_THROW(storage.RecordLong(10l));
  expected_sum += 10;

  EXPECT_NO_THROW(storage.RecordLong(30l));
  expected_sum += 30;
  /*EXPECT_NO_THROW(storage.RecordLong(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));*/

  EXPECT_NO_THROW(storage.RecordDouble(10.10, opentelemetry::common::KeyValueIterableView<M>({})));
  opentelemetry::common::SystemTimestamp sdk_start_time = std::chrono::system_clock::now();
  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporarily::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  // Some computation here
  auto collection_ts = std::chrono::system_clock::now();
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [expected_sum](const MetricData data) {
                    for (auto data_attr : data.point_data_attr_)
                    {
                      auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
                      EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), expected_sum);
                    }
                    return true;
                  });

  expected_sum = 0;
  EXPECT_NO_THROW(storage.RecordLong(50l));
  expected_sum += 50;
  EXPECT_NO_THROW(storage.RecordLong(20l));
  expected_sum += 20;

  collection_ts = std::chrono::system_clock::now();
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [expected_sum](const MetricData data) {
                    for (auto data_attr : data.point_data_attr_)
                    {
                      auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
                      EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), expected_sum);
                    }
                    return true;
                  });
}

TEST(WritableMetricStorageTest, DoubleSumCummulative)
{
  auto sdk_start_ts               = std::chrono::system_clock::now();
  double expected_sum             = 0;
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kDouble};

  opentelemetry::sdk::metrics::SyncMetricStorage storage(instr_desc, AggregationType::kSum,
                                                         new DefaultAttributesProcessor());
  EXPECT_NO_THROW(storage.RecordDouble(10.0));
  expected_sum += 10;

  EXPECT_NO_THROW(storage.RecordDouble(30.0));
  expected_sum += 30;
  /*EXPECT_NO_THROW(storage.RecordLong(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));*/

  opentelemetry::common::SystemTimestamp sdk_start_time = std::chrono::system_clock::now();
  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporarily::kCumulative));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  // Some computation here
  auto collection_ts = std::chrono::system_clock::now();
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [expected_sum](const MetricData data) {
                    for (auto data_attr : data.point_data_attr_)
                    {
                      auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
                      EXPECT_EQ(opentelemetry::nostd::get<double>(data.value_), expected_sum);
                    }
                    return true;
                  });

  EXPECT_NO_THROW(storage.RecordDouble(50.0));
  expected_sum += 50;
  EXPECT_NO_THROW(storage.RecordDouble(20.0));
  expected_sum += 20;

  collection_ts = std::chrono::system_clock::now();
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [expected_sum](const MetricData data) {
                    for (auto data_attr : data.point_data_attr_)
                    {
                      auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
                      EXPECT_EQ(opentelemetry::nostd::get<double>(data.value_), expected_sum);
                    }
                    return true;
                  });
}

TEST(WritableMetricStorageTest, DoubleSumDelta)
{
  auto sdk_start_ts               = std::chrono::system_clock::now();
  long expected_sum               = 0;
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kDouble};

  opentelemetry::sdk::metrics::SyncMetricStorage storage(instr_desc, AggregationType::kSum,
                                                         new DefaultAttributesProcessor());
  EXPECT_NO_THROW(storage.RecordDouble(10.0));
  expected_sum += 10;

  EXPECT_NO_THROW(storage.RecordDouble(30.0));
  expected_sum += 30;
  /*EXPECT_NO_THROW(storage.RecordLong(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));*/

  opentelemetry::common::SystemTimestamp sdk_start_time = std::chrono::system_clock::now();
  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporarily::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  // Some computation here
  auto collection_ts = std::chrono::system_clock::now();
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [expected_sum](const MetricData data) {
                    for (auto data_attr : data.point_data_attr_)
                    {
                      auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
                      EXPECT_EQ(opentelemetry::nostd::get<double>(data.value_), expected_sum);
                    }
                    return true;
                  });

  expected_sum = 0;
  EXPECT_NO_THROW(storage.RecordDouble(50.0));
  expected_sum += 50;
  EXPECT_NO_THROW(storage.RecordDouble(20.0));
  expected_sum += 20;

  collection_ts = std::chrono::system_clock::now();
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [expected_sum](const MetricData data) {
                    for (auto data_attr : data.point_data_attr_)
                    {
                      auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
                      EXPECT_EQ(opentelemetry::nostd::get<double>(data.value_), expected_sum);
                    }
                    return true;
                  });
}

#endif
