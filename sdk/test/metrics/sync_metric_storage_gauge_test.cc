// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "common.h"

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/metrics/exemplar/histogram_exemplar_reservoir.h"
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

class WritableMetricStorageGaugeTestFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

TEST_P(WritableMetricStorageGaugeTestFixture, LongGaugeLastValueAggregation)
{
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  AggregationTemporality temporality = GetParam();
  auto sdk_start_ts                  = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc    = {"name", "desc", "1unit", InstrumentType::kGauge,
                                     InstrumentValueType::kLong};
  std::map<std::string, std::string> attributes_roomA = {{"Room.Id", "Rack A"}};
  std::map<std::string, std::string> attributes_roomB = {{"Room.Id", "Rack B"}};

  std::unique_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, default_attributes_processor.get(),
      ExemplarReservoir::GetNoExemplarReservoir(), nullptr);

  uint64_t noiselevel_1_roomA = 10, noiselevel_1_roomB = 30;
  storage.RecordLong(noiselevel_1_roomA,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                     opentelemetry::context::Context{});
  storage.RecordLong(noiselevel_1_roomB,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                     opentelemetry::context::Context{});

  uint64_t noiselevel_2_roomA = 12, noiselevel_2_roomB = 29;
  storage.RecordLong(noiselevel_2_roomA,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                     opentelemetry::context::Context{});
  storage.RecordLong(noiselevel_2_roomB,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                     opentelemetry::context::Context{});

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
          auto lastvalue_data = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("Room.Id")->second) == "Rack A")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lastvalue_data.value_),
                        noiselevel_2_roomA);
            }
            else
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lastvalue_data.value_),
                        noiselevel_2_roomA - noiselevel_1_roomA);
            }
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("Room.Id")->second) == "Rack B")
          {
            if (temporality == AggregationTemporality::kCumulative)
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lastvalue_data.value_),
                        noiselevel_2_roomB);
            }
            else
            {
              EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lastvalue_data.value_),
                        noiselevel_2_roomB - noiselevel_1_roomB);
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
                         WritableMetricStorageGaugeTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative));
