// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>

#include "opentelemetry/common/attribute_value.h"  // IWYU pragma: keep
#include "opentelemetry/nostd/utility.h"           // IWYU pragma: keep
#include "opentelemetry/sdk/metrics/instruments.h"

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
#  include <stddef.h>
#  include <stdint.h>
#  include <chrono>
#  include <map>
#  include <memory>
#  include <utility>
#  include <vector>
#  include "common.h"

#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/nostd/function_ref.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"
#  include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#endif

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::common;

namespace
{

class GaugeWritableMetricStorageTestFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

class WritableMetricStorageDeltaMultiReaderTestFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

TEST_P(GaugeWritableMetricStorageTestFixture, LongGaugeLastValueAggregation)
{
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  AggregationTemporality temporality = GetParam();
  auto sdk_start_ts                  = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc    = {"name", "desc", "1unit", InstrumentType::kGauge,
                                        InstrumentValueType::kLong};
  std::map<std::string, std::string> attributes_roomA = {{"Room.id", "Rack A"}};
  std::map<std::string, std::string> attributes_roomB = {{"Room.id", "Rack B"}};

  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, default_attributes_processor,
#  ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#  endif
      nullptr);

  int64_t bg_noise_level_1_roomA = 10;
  int64_t bg_noise_level_1_roomB = 20;

  storage.RecordLong(bg_noise_level_1_roomA,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                     opentelemetry::context::Context{});
  storage.RecordLong(bg_noise_level_1_roomB,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                     opentelemetry::context::Context{});

  int64_t bg_noise_level_2_roomA = 43;
  int64_t bg_noise_level_2_roomB = 25;

  storage.RecordLong(bg_noise_level_2_roomA,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                     opentelemetry::context::Context{});
  storage.RecordLong(bg_noise_level_2_roomB,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                     opentelemetry::context::Context{});

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
          auto lastvalue_data = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("Room.id")->second) == "Rack A")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lastvalue_data.value_),
                        bg_noise_level_2_roomA);
            }
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("Room.id")->second) == "Rack B")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lastvalue_data.value_),
                        bg_noise_level_2_roomB);
            }
            count_attributes++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_attributes, 2);  // RackA and RackB
#else
  EXPECT_TRUE(true);
#endif
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestLong,
                         GaugeWritableMetricStorageTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative));

TEST_P(GaugeWritableMetricStorageTestFixture, DoubleGaugeLastValueAggregation)
{
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  AggregationTemporality temporality = GetParam();
  auto sdk_start_ts                  = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc    = {"name", "desc", "1unit", InstrumentType::kGauge,
                                        InstrumentValueType::kDouble};
  std::map<std::string, std::string> attributes_roomA = {{"Room.id", "Rack A"}};
  std::map<std::string, std::string> attributes_roomB = {{"Room.id", "Rack B"}};

  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, default_attributes_processor,
#  ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#  endif
      nullptr);

  double bg_noise_level_1_roomA = 4.3;
  double bg_noise_level_1_roomB = 2.5;

  storage.RecordDouble(bg_noise_level_1_roomA,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                       opentelemetry::context::Context{});
  storage.RecordDouble(bg_noise_level_1_roomB,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                       opentelemetry::context::Context{});

  double bg_noise_level_2_roomA = 10.5;
  double bg_noise_level_2_roomB = 20.5;

  storage.RecordDouble(bg_noise_level_2_roomA,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                       opentelemetry::context::Context{});
  storage.RecordDouble(bg_noise_level_2_roomB,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                       opentelemetry::context::Context{});

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
          auto lastvalue_data = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("Room.id")->second) == "Rack A")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_DOUBLE_EQ(opentelemetry::nostd::get<double>(lastvalue_data.value_),
                               bg_noise_level_2_roomA);
            }
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("Room.id")->second) == "Rack B")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_DOUBLE_EQ(opentelemetry::nostd::get<double>(lastvalue_data.value_),
                               bg_noise_level_2_roomB);
            }
            count_attributes++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_attributes, 2);  // RackA and RackB
#else
  EXPECT_TRUE(true);
#endif
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestDouble,
                         GaugeWritableMetricStorageTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative));

TEST_P(WritableMetricStorageDeltaMultiReaderTestFixture,
       LongGaugeLastValueAggregationDeltaTemporalityMultiReader)
{
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  // Slow-path (collectors.size() > 1) delta temporality for gauge instruments.
  // Verifies that:
  //   1. Each collector independently tracks its own measurement intervals.
  //   2. The first interval start_ts is the instrument creation time (spec issue #4062).
  //   3. Subsequent interval start_ts equals the previous collection end_ts.
  //   4. Empty intervals (no new measurements) do not invoke the callback but DO
  //      advance the per-collector start timestamp, so the next interval is
  //      correctly bounded (mirrors fast-path behaviour).

  auto sdk_start_ts    = std::chrono::system_clock::now();
  auto before_creation = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc{"name", "desc", "1unit", InstrumentType::kGauge,
                                  InstrumentValueType::kLong};
  std::map<std::string, std::string> attributes_roomA = {{"Room.id", "Rack A"}};
  std::map<std::string, std::string> attributes_roomB = {{"Room.id", "Rack B"}};

  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, default_attributes_processor,
#  ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#  endif
      nullptr);
  auto after_creation = std::chrono::system_clock::now();

  // Two collectors force the slow path (collectors.size() > 1).
  std::shared_ptr<CollectorHandle> collector_a(new MockCollectorHandle(GetParam()));
  std::shared_ptr<CollectorHandle> collector_b(new MockCollectorHandle(GetParam()));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector_a, collector_b};

  // --- Interval 1: record values and collect from collector_a ---
  int64_t level1_roomA = 10;
  int64_t level1_roomB = 20;
  storage.RecordLong(level1_roomA,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                     opentelemetry::context::Context{});
  storage.RecordLong(level1_roomB,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                     opentelemetry::context::Context{});

  auto collection1_ts = std::chrono::system_clock::now();
  size_t count_a      = 0;
  MetricData metric_a_cycle1;
  storage.Collect(collector_a.get(), collectors, sdk_start_ts, collection1_ts,
                  [&](const MetricData &metric_data) {
                    metric_a_cycle1 = metric_data;
                    for (const auto &data_attr : metric_data.point_data_attr_)
                    {
                      auto lv = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
                      const auto &room_id = opentelemetry::nostd::get<std::string>(
                          data_attr.attributes.find("Room.id")->second);
                      if (room_id == "Rack A")
                      {
                        EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lv.value_), level1_roomA);
                        count_a++;
                      }
                      else if (room_id == "Rack B")
                      {
                        EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lv.value_), level1_roomB);
                        count_a++;
                      }
                    }
                    return true;
                  });
  EXPECT_EQ(count_a, 2);
  // Per spec (#4062): first delta interval start_ts must be instrument creation time.
  EXPECT_GE(metric_a_cycle1.start_ts.time_since_epoch(), before_creation.time_since_epoch());
  EXPECT_LE(metric_a_cycle1.start_ts.time_since_epoch(), after_creation.time_since_epoch());
  EXPECT_GT(metric_a_cycle1.start_ts.time_since_epoch(), sdk_start_ts.time_since_epoch());
  EXPECT_EQ(metric_a_cycle1.end_ts, collection1_ts);

  // --- Empty interval for collector_a (no new records) ---
  // The callback must NOT be invoked; the interval timestamp must be updated.
  auto collection2_ts   = std::chrono::system_clock::now();
  bool callback_invoked = false;
  count_a               = 0;
  storage.Collect(collector_a.get(), collectors, sdk_start_ts, collection2_ts,
                  [&](const MetricData &metric_data) {
                    callback_invoked = true;
                    count_a += metric_data.point_data_attr_.size();
                    return true;
                  });
  EXPECT_FALSE(callback_invoked);  // callback must not be invoked for empty interval
  EXPECT_EQ(count_a, 0);

  // --- Interval 3: record new values and collect from collector_a ---
  int64_t level2_roomA = 43;
  int64_t level2_roomB = 25;
  storage.RecordLong(level2_roomA,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                     opentelemetry::context::Context{});
  storage.RecordLong(level2_roomB,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                     opentelemetry::context::Context{});

  auto collection3_ts = std::chrono::system_clock::now();
  count_a             = 0;
  MetricData metric_a_cycle3;
  storage.Collect(collector_a.get(), collectors, sdk_start_ts, collection3_ts,
                  [&](const MetricData &metric_data) {
                    metric_a_cycle3 = metric_data;
                    for (const auto &data_attr : metric_data.point_data_attr_)
                    {
                      auto lv = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
                      const auto &room_id = opentelemetry::nostd::get<std::string>(
                          data_attr.attributes.find("Room.id")->second);
                      if (room_id == "Rack A")
                      {
                        EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lv.value_), level2_roomA);
                        count_a++;
                      }
                      else if (room_id == "Rack B")
                      {
                        EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lv.value_), level2_roomB);
                        count_a++;
                      }
                    }
                    return true;
                  });
  EXPECT_EQ(count_a, 2);
  // start_ts must be collection2_ts: the empty cycle advanced the interval boundary.
  EXPECT_EQ(metric_a_cycle3.start_ts, collection2_ts);
  EXPECT_EQ(metric_a_cycle3.end_ts, collection3_ts);

  // --- First collection from collector_b ---
  // collector_b has never collected; its unreported stash holds all delta
  // snapshots pushed during collector_a's Collect calls (dm_level1, dm_level2).
  // The merged result must be the most recently observed value (level2), and
  // start_ts must be instrument creation time.
  size_t count_b = 0;
  MetricData metric_b_cycle1;
  storage.Collect(collector_b.get(), collectors, sdk_start_ts, collection3_ts,
                  [&](const MetricData &metric_data) {
                    metric_b_cycle1 = metric_data;
                    for (const auto &data_attr : metric_data.point_data_attr_)
                    {
                      auto lv = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
                      const auto &room_id = opentelemetry::nostd::get<std::string>(
                          data_attr.attributes.find("Room.id")->second);
                      if (room_id == "Rack A")
                      {
                        // LastValue merge selects the most recently sampled value.
                        EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lv.value_), level2_roomA);
                        count_b++;
                      }
                      else if (room_id == "Rack B")
                      {
                        EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lv.value_), level2_roomB);
                        count_b++;
                      }
                    }
                    return true;
                  });
  EXPECT_EQ(count_b, 2);
  // collector_b's first interval: start_ts must be instrument creation time.
  EXPECT_GE(metric_b_cycle1.start_ts.time_since_epoch(), before_creation.time_since_epoch());
  EXPECT_LE(metric_b_cycle1.start_ts.time_since_epoch(), after_creation.time_since_epoch());
  EXPECT_GT(metric_b_cycle1.start_ts.time_since_epoch(), sdk_start_ts.time_since_epoch());
  EXPECT_EQ(metric_b_cycle1.end_ts, collection3_ts);

  // --- Empty interval for collector_b (no new records) ---
  // Stale gauge values must not be re-emitted on the shared unreported_metrics_ path.
  auto collection4_ts     = std::chrono::system_clock::now();
  bool callback_invoked_b = false;
  count_b                 = 0;
  storage.Collect(collector_b.get(), collectors, sdk_start_ts, collection4_ts,
                  [&](const MetricData &metric_data) {
                    callback_invoked_b = true;
                    count_b += metric_data.point_data_attr_.size();
                    return true;
                  });
  EXPECT_FALSE(callback_invoked_b);  // callback must not be invoked for empty interval
  EXPECT_EQ(count_b, 0);             // stale values must not be re-emitted
#else
  EXPECT_TRUE(true);
#endif  // OPENTELEMETRY_ABI_VERSION_NO >= 2
}
INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestLongDeltaMultiReader,
                         WritableMetricStorageDeltaMultiReaderTestFixture,
                         ::testing::Values(AggregationTemporality::kDelta));

TEST_P(WritableMetricStorageDeltaMultiReaderTestFixture,
       DoubleGaugeLastValueAggregationDeltaTemporalityMultiReader)
{
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  // Slow-path (collectors.size() > 1) delta temporality for gauge instruments (double).
  // Mirrors LongGaugeLastValueAggregationDeltaTemporalityMultiReader but uses
  // double values and RecordDouble.
  auto sdk_start_ts = std::chrono::system_clock::now();

  auto before_creation = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc{"name", "desc", "1unit", InstrumentType::kGauge,
                                  InstrumentValueType::kDouble};
  std::map<std::string, std::string> attributes_roomA = {{"Room.id", "Rack A"}};
  std::map<std::string, std::string> attributes_roomB = {{"Room.id", "Rack B"}};

  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, default_attributes_processor,
#  ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#  endif
      nullptr);
  auto after_creation = std::chrono::system_clock::now();

  std::shared_ptr<CollectorHandle> collector_a(new MockCollectorHandle(GetParam()));
  std::shared_ptr<CollectorHandle> collector_b(new MockCollectorHandle(GetParam()));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector_a, collector_b};

  // --- Interval 1: record values and collect from collector_a ---
  double level1_roomA = 1.5;
  double level1_roomB = 2.5;
  storage.RecordDouble(level1_roomA,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                       opentelemetry::context::Context{});
  storage.RecordDouble(level1_roomB,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                       opentelemetry::context::Context{});

  auto collection1_ts = std::chrono::system_clock::now();
  size_t count_a      = 0;
  MetricData metric_a_cycle1;
  storage.Collect(
      collector_a.get(), collectors, sdk_start_ts, collection1_ts,
      [&](const MetricData &metric_data) {
        metric_a_cycle1 = metric_data;
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          auto lv = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          const auto &room_id =
              opentelemetry::nostd::get<std::string>(data_attr.attributes.find("Room.id")->second);
          if (room_id == "Rack A")
          {
            EXPECT_DOUBLE_EQ(opentelemetry::nostd::get<double>(lv.value_), level1_roomA);
            count_a++;
          }
          else if (room_id == "Rack B")
          {
            EXPECT_DOUBLE_EQ(opentelemetry::nostd::get<double>(lv.value_), level1_roomB);
            count_a++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_a, 2);
  // Per spec (#4062): first delta interval start_ts must be instrument creation time.
  EXPECT_GE(metric_a_cycle1.start_ts.time_since_epoch(), before_creation.time_since_epoch());
  EXPECT_LE(metric_a_cycle1.start_ts.time_since_epoch(), after_creation.time_since_epoch());
  EXPECT_GT(metric_a_cycle1.start_ts.time_since_epoch(), sdk_start_ts.time_since_epoch());
  EXPECT_EQ(metric_a_cycle1.end_ts, collection1_ts);

  // --- Empty interval for collector_a (no new records) ---
  auto collection2_ts   = std::chrono::system_clock::now();
  bool callback_invoked = false;
  count_a               = 0;
  storage.Collect(collector_a.get(), collectors, sdk_start_ts, collection2_ts,
                  [&](const MetricData &metric_data) {
                    callback_invoked = true;
                    count_a += metric_data.point_data_attr_.size();
                    return true;
                  });
  EXPECT_FALSE(callback_invoked);  // callback must not be invoked for empty interval
  EXPECT_EQ(count_a, 0);

  // --- Interval 3: record new values and collect from collector_a ---
  double level2_roomA = 4.3;
  double level2_roomB = 2.1;
  storage.RecordDouble(level2_roomA,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                       opentelemetry::context::Context{});
  storage.RecordDouble(level2_roomB,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                       opentelemetry::context::Context{});

  auto collection3_ts = std::chrono::system_clock::now();
  count_a             = 0;
  MetricData metric_a_cycle3;
  storage.Collect(
      collector_a.get(), collectors, sdk_start_ts, collection3_ts,
      [&](const MetricData &metric_data) {
        metric_a_cycle3 = metric_data;
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          auto lv = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          const auto &room_id =
              opentelemetry::nostd::get<std::string>(data_attr.attributes.find("Room.id")->second);
          if (room_id == "Rack A")
          {
            EXPECT_DOUBLE_EQ(opentelemetry::nostd::get<double>(lv.value_), level2_roomA);
            count_a++;
          }
          else if (room_id == "Rack B")
          {
            EXPECT_DOUBLE_EQ(opentelemetry::nostd::get<double>(lv.value_), level2_roomB);
            count_a++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_a, 2);
  // start_ts must be collection2_ts: the empty cycle advanced the interval boundary.
  EXPECT_EQ(metric_a_cycle3.start_ts, collection2_ts);
  EXPECT_EQ(metric_a_cycle3.end_ts, collection3_ts);

  // --- First collection from collector_b ---
  // collector_b has never collected; its unreported stash holds all delta
  // snapshots pushed during collector_a's Collect calls (dm_level1, dm_level2).
  // The merged result must be the most recently observed value (level2), and
  // start_ts must be instrument creation time.
  size_t count_b = 0;
  MetricData metric_b_cycle1;
  storage.Collect(
      collector_b.get(), collectors, sdk_start_ts, collection3_ts,
      [&](const MetricData &metric_data) {
        metric_b_cycle1 = metric_data;
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          auto lv = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          const auto &room_id =
              opentelemetry::nostd::get<std::string>(data_attr.attributes.find("Room.id")->second);
          if (room_id == "Rack A")
          {
            // LastValue merge selects the most recently sampled value.
            EXPECT_DOUBLE_EQ(opentelemetry::nostd::get<double>(lv.value_), level2_roomA);
            count_b++;
          }
          else if (room_id == "Rack B")
          {
            EXPECT_DOUBLE_EQ(opentelemetry::nostd::get<double>(lv.value_), level2_roomB);
            count_b++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_b, 2);
  // collector_b's first interval: start_ts must be instrument creation time.
  EXPECT_GE(metric_b_cycle1.start_ts.time_since_epoch(), before_creation.time_since_epoch());
  EXPECT_LE(metric_b_cycle1.start_ts.time_since_epoch(), after_creation.time_since_epoch());
  EXPECT_GT(metric_b_cycle1.start_ts.time_since_epoch(), sdk_start_ts.time_since_epoch());
  EXPECT_EQ(metric_b_cycle1.end_ts, collection3_ts);

  // --- Empty interval for collector_b (no new records) ---
  // Stale gauge values must not be re-emitted on the shared unreported_metrics_ path.
  auto collection4_ts     = std::chrono::system_clock::now();
  bool callback_invoked_b = false;
  count_b                 = 0;
  storage.Collect(collector_b.get(), collectors, sdk_start_ts, collection4_ts,
                  [&](const MetricData &metric_data) {
                    callback_invoked_b = true;
                    count_b += metric_data.point_data_attr_.size();
                    return true;
                  });
  EXPECT_FALSE(callback_invoked_b);  // callback must not be invoked for empty interval
  EXPECT_EQ(count_b, 0);             // stale values must not be re-emitted
#else
  EXPECT_TRUE(true);
#endif  // OPENTELEMETRY_ABI_VERSION_NO >= 2
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestDoubleDeltaMultiReader,
                         WritableMetricStorageDeltaMultiReaderTestFixture,
                         ::testing::Values(AggregationTemporality::kDelta));

}  // namespace
