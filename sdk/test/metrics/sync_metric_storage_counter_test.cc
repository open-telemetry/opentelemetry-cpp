// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stddef.h>
#include <stdint.h>
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "common.h"

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/state/metric_collector.h"
#include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#endif

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::common;

namespace
{

class CounterWritableMetricStorageTestFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

TEST_P(CounterWritableMetricStorageTestFixture, LongCounterSumAggregation)
{
  AggregationTemporality temporality  = GetParam();
  auto sdk_start_ts                   = std::chrono::system_clock::now();
  int64_t expected_total_get_requests = 0;
  int64_t expected_total_put_requests = 0;
  InstrumentDescriptor instr_desc     = {"name", "desc", "1unit", InstrumentType::kCounter,
                                         InstrumentValueType::kLong};
  std::map<std::string, std::string> attributes_get = {{"RequestType", "GET"}};
  std::map<std::string, std::string> attributes_put = {{"RequestType", "PUT"}};

  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kSum, default_attributes_processor,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

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
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), expected_total_get_requests);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), expected_total_put_requests);
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
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            count_attributes++;
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), expected_total_get_requests);
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            count_attributes++;
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), expected_total_put_requests);
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
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), expected_total_get_requests);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_), expected_total_put_requests);
            count_attributes++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_attributes, 2);  // GET and PUT
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestLong,
                         CounterWritableMetricStorageTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

TEST_P(CounterWritableMetricStorageTestFixture, DoubleCounterSumAggregation)
{
  AggregationTemporality temporality = GetParam();
  auto sdk_start_ts                  = std::chrono::system_clock::now();
  double expected_total_get_requests = 0;
  double expected_total_put_requests = 0;
  InstrumentDescriptor instr_desc    = {"name", "desc", "1unit", InstrumentType::kCounter,
                                        InstrumentValueType::kDouble};
  std::map<std::string, std::string> attributes_get = {{"RequestType", "GET"}};
  std::map<std::string, std::string> attributes_put = {{"RequestType", "PUT"}};

  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kSum, default_attributes_processor,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

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
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
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
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("RequestType")->second) == "GET")
          {
            count_attributes++;
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.value_), expected_total_get_requests);
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("RequestType")->second) == "PUT")
          {
            count_attributes++;
            EXPECT_EQ(opentelemetry::nostd::get<double>(data.value_), expected_total_put_requests);
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
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
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
  EXPECT_EQ(count_attributes, 2);  // GET and PUT
}

TEST(SyncMetricStorageTest, DeltaCounterStartTimestampTracksEmptyCycles)
{
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kLong};
  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kSum, default_attributes_processor,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);

  std::map<std::string, std::string> attributes = {{"RequestType", "GET"}};
  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  auto sdk_start_ts   = std::chrono::system_clock::now();
  auto collection_ts1 = sdk_start_ts + std::chrono::seconds(1);
  auto collection_ts2 = sdk_start_ts + std::chrono::seconds(2);
  auto collection_ts3 = sdk_start_ts + std::chrono::seconds(3);

  storage.RecordLong(10, KeyValueIterableView<std::map<std::string, std::string>>(attributes),
                     opentelemetry::context::Context{});

  MetricData metric_cycle1;
  bool cycle1_called = false;
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts1,
                  [&](const MetricData &metric_data) {
                    metric_cycle1 = metric_data;
                    cycle1_called = true;
                    return true;
                  });
  EXPECT_TRUE(cycle1_called);

  bool cycle2_called = false;
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts2,
                  [&](const MetricData &) {
                    cycle2_called = true;
                    return true;
                  });
  EXPECT_FALSE(cycle2_called);  // Check if Empty cycle in the middle

  storage.RecordLong(20, KeyValueIterableView<std::map<std::string, std::string>>(attributes),
                     opentelemetry::context::Context{});

  MetricData metric_cycle3;
  bool cycle3_called = false;
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts3,
                  [&](const MetricData &metric_data) {
                    metric_cycle3 = metric_data;
                    cycle3_called = true;
                    return true;
                  });
  EXPECT_TRUE(cycle3_called);
  // Check that the fast path correctly preserved the timestamp from the empty
  // collection cycle (cycle 2)
  EXPECT_EQ(metric_cycle3.start_ts, collection_ts2);
  // Per OTel spec (#4062), the first delta interval's start_ts is the
  // instrument's creation time (captured at storage construction above), not
  // the sdk_start_ts that the MeterProvider would otherwise pass in. Storage
  // was constructed before sdk_start_ts was sampled, so the start_ts must be
  // less-or-equal to sdk_start_ts. SystemTimestamp does not define ordering
  // operators directly, so compare via time_since_epoch().
  EXPECT_LE(metric_cycle1.start_ts.time_since_epoch(), sdk_start_ts.time_since_epoch());
  EXPECT_EQ(metric_cycle1.end_ts, collection_ts1);
  EXPECT_EQ(metric_cycle3.end_ts, collection_ts3);
}

TEST(SyncMetricStorageTest, DeltaCounterFirstIntervalUsesInstrumentCreationTime)
{
  // Spec-compliance test for issue #4062: the start_ts of the first delta
  // collection interval MUST be the per-instrument creation time, NOT the
  // MeterProvider/SDK start time. Simulate a MeterProvider that was created
  // long before the instrument by passing an artificial sdk_start_ts that is
  // strictly earlier than the storage's construction time.
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kLong};
  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};

  auto before_creation = std::chrono::system_clock::now();
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kSum, default_attributes_processor,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);
  auto after_creation = std::chrono::system_clock::now();

  // sdk_start_ts is intentionally well before storage construction.
  auto sdk_start_ts  = before_creation - std::chrono::seconds(60);
  auto collection_ts = after_creation + std::chrono::seconds(1);

  std::map<std::string, std::string> attributes = {{"RequestType", "GET"}};
  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  storage.RecordLong(10, KeyValueIterableView<std::map<std::string, std::string>>(attributes),
                     opentelemetry::context::Context{});

  MetricData metric_data;
  bool collected = false;
  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [&](const MetricData &md) {
                    metric_data = md;
                    collected   = true;
                    return true;
                  });
  ASSERT_TRUE(collected);

  // start_ts must be the instrument creation time (between the two clock
  // samples taken around the storage constructor), and strictly greater than
  // the simulated MeterProvider start time. SystemTimestamp does not define
  // ordering operators directly; compare via time_since_epoch() durations.
  EXPECT_GE(metric_data.start_ts.time_since_epoch(), before_creation.time_since_epoch());
  EXPECT_LE(metric_data.start_ts.time_since_epoch(), after_creation.time_since_epoch());
  EXPECT_GT(metric_data.start_ts.time_since_epoch(), sdk_start_ts.time_since_epoch());
  EXPECT_EQ(metric_data.end_ts, collection_ts);
}

TEST(SyncMetricStorageTest, DeltaCounterMultiCollectorFirstIntervalUsesInstrumentCreationTime)
{
  // Slow-path counterpart of DeltaCounterFirstIntervalUsesInstrumentCreationTime.
  // With more than one collector, buildMetrics takes the slow path; that path
  // must also use instrument_creation_ts_ as the start_ts for the first delta
  // collection interval (issue #4062 explicitly mentions both paths).
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kLong};
  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};

  auto before_creation = std::chrono::system_clock::now();
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kSum, default_attributes_processor,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr);
  auto after_creation = std::chrono::system_clock::now();

  auto sdk_start_ts  = before_creation - std::chrono::seconds(60);
  auto collection_ts = after_creation + std::chrono::seconds(1);

  std::map<std::string, std::string> attributes = {{"RequestType", "GET"}};
  // Two collectors force collectors.size() > 1, which bypasses the fast path.
  std::shared_ptr<CollectorHandle> collector_a(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::shared_ptr<CollectorHandle> collector_b(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector_a, collector_b};

  storage.RecordLong(10, KeyValueIterableView<std::map<std::string, std::string>>(attributes),
                     opentelemetry::context::Context{});

  MetricData metric_data;
  bool collected = false;
  storage.Collect(collector_a.get(), collectors, sdk_start_ts, collection_ts,
                  [&](const MetricData &md) {
                    metric_data = md;
                    collected   = true;
                    return true;
                  });
  ASSERT_TRUE(collected);

  // Same assertion shape as the fast-path test.
  EXPECT_GE(metric_data.start_ts.time_since_epoch(), before_creation.time_since_epoch());
  EXPECT_LE(metric_data.start_ts.time_since_epoch(), after_creation.time_since_epoch());
  EXPECT_GT(metric_data.start_ts.time_since_epoch(), sdk_start_ts.time_since_epoch());
  EXPECT_EQ(metric_data.end_ts, collection_ts);
}
INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestDouble,
                         CounterWritableMetricStorageTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

}  // namespace
