// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "common.h"

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/metrics/exemplar/no_exemplar_reservoir.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"

#include <gtest/gtest.h>
#include <map>
#include <memory>

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::common;
using M         = std::map<std::string, std::string>;
namespace nostd = opentelemetry::nostd;

class WritableMetricStorageHistogramTestFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

TEST_P(WritableMetricStorageHistogramTestFixture, LongHistogram)
{
  AggregationTemporality temporality  = GetParam();
  auto sdk_start_ts                   = std::chrono::system_clock::now();
  int64_t expected_total_get_requests = 0;
  int64_t expected_total_put_requests = 0;
  InstrumentDescriptor instr_desc     = {"name", "desc", "1unit", InstrumentType::kHistogram,
                                     InstrumentValueType::kLong};
  std::map<std::string, std::string> attributes_get = {{"RequestType", "GET"}};
  std::map<std::string, std::string> attributes_put = {{"RequestType", "PUT"}};

  std::unique_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kHistogram, default_attributes_processor.get(),
      NoExemplarReservoir::GetNoExemplarReservoir(), nullptr);

  storage.RecordLong(10, KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
                     opentelemetry::context::Context{});
  expected_total_get_requests += 10;

  storage.RecordLong(30, KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
                     opentelemetry::context::Context{});
  expected_total_put_requests += 30;

  storage.RecordLong(20, KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
                     opentelemetry::context::Context{});
  expected_total_get_requests += 20;

  storage.RecordLong(40, KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
                     opentelemetry::context::Context{});
  expected_total_put_requests += 40;

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  // Some computation here
  auto collection_ts      = std::chrono::system_clock::now();
  size_t count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<HistogramPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.sum_), expected_total_get_requests);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.sum_), expected_total_put_requests);
            count_attributes++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_attributes, 2);  // GET and PUT
  // In case of delta temporarily, subsequent collection would contain new data points, so resetting
  // the counts
  if (temporality == AggregationTemporality::kDelta)
  {
    expected_total_get_requests = 0;
    expected_total_put_requests = 0;
  }

  // collect one more time.
  collection_ts    = std::chrono::system_clock::now();
  count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        if (temporality == AggregationTemporality::kCumulative)
        {
          EXPECT_EQ(metric_data.start_ts, sdk_start_ts);
        }
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<HistogramPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            count_attributes++;
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.sum_), expected_total_get_requests);
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            count_attributes++;
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.sum_), expected_total_put_requests);
          }
        }
        return true;
      });
  if (temporality == AggregationTemporality::kCumulative)
  {
    EXPECT_EQ(count_attributes, 2);  // GET AND PUT
  }

  storage.RecordLong(50, KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
                     opentelemetry::context::Context{});
  expected_total_get_requests += 50;
  storage.RecordLong(40, KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
                     opentelemetry::context::Context{});
  expected_total_put_requests += 40;

  collection_ts    = std::chrono::system_clock::now();
  count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<HistogramPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.sum_), expected_total_get_requests);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.sum_), expected_total_put_requests);
            count_attributes++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_attributes, 2);  // GET and PUT
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageHistogramTestLong,
                         WritableMetricStorageHistogramTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

TEST_P(WritableMetricStorageHistogramTestFixture, DoubleHistogram)
{
  AggregationTemporality temporality = GetParam();
  auto sdk_start_ts                  = std::chrono::system_clock::now();
  double expected_total_get_requests = 0;
  double expected_total_put_requests = 0;
  InstrumentDescriptor instr_desc    = {"name", "desc", "1unit", InstrumentType::kHistogram,
                                     InstrumentValueType::kDouble};
  std::map<std::string, std::string> attributes_get = {{"RequestType", "GET"}};
  std::map<std::string, std::string> attributes_put = {{"RequestType", "PUT"}};

  std::unique_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kHistogram, default_attributes_processor.get(),
      NoExemplarReservoir::GetNoExemplarReservoir(), nullptr);

  storage.RecordDouble(10.0,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
                       opentelemetry::context::Context{});
  expected_total_get_requests += 10;

  storage.RecordDouble(30.0,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
                       opentelemetry::context::Context{});
  expected_total_put_requests += 30;

  storage.RecordDouble(20.0,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
                       opentelemetry::context::Context{});
  expected_total_get_requests += 20;

  storage.RecordDouble(40.0,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
                       opentelemetry::context::Context{});
  expected_total_put_requests += 40;

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  // Some computation here
  auto collection_ts      = std::chrono::system_clock::now();
  size_t count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<HistogramPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.sum_), expected_total_get_requests);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.sum_), expected_total_put_requests);
            count_attributes++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_attributes, 2);  // GET and PUT

  // In case of delta temporarily, subsequent collection would contain new data points, so resetting
  // the counts
  if (temporality == AggregationTemporality::kDelta)
  {
    expected_total_get_requests = 0;
    expected_total_put_requests = 0;
  }

  // collect one more time.
  collection_ts    = std::chrono::system_clock::now();
  count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        if (temporality == AggregationTemporality::kCumulative)
        {
          EXPECT_EQ(metric_data.start_ts, sdk_start_ts);
        }
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<HistogramPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            count_attributes++;
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.sum_), expected_total_get_requests);
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            count_attributes++;
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.sum_), expected_total_put_requests);
          }
        }
        return true;
      });
  if (temporality == AggregationTemporality::kCumulative)
  {
    EXPECT_EQ(count_attributes, 2);  // GET AND PUT
  }

  storage.RecordDouble(50.0,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_get),
                       opentelemetry::context::Context{});
  expected_total_get_requests += 50;
  storage.RecordDouble(40.0,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_put),
                       opentelemetry::context::Context{});
  expected_total_put_requests += 40;

  collection_ts    = std::chrono::system_clock::now();
  count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<HistogramPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.sum_), expected_total_get_requests);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.sum_), expected_total_put_requests);
            count_attributes++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_attributes, 2);  // GET and PUT
}
INSTANTIATE_TEST_SUITE_P(WritableMetricStorageHistogramTestDouble,
                         WritableMetricStorageHistogramTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));
