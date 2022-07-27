// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/async_metric_storage.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
//#include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/metrics/async_instruments.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/metrics/metric_exporter.h"
#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include "opentelemetry/sdk/metrics/observer_result.h"
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"
#  include "opentelemetry/sdk/metrics/state/observable_registry.h"

#  include <gtest/gtest.h>
#  include <vector>

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::instrumentationlibrary;
using namespace opentelemetry::sdk::resource;

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

class MeasurementFetcher
{
public:
  static void Fetcher(opentelemetry::metrics::ObserverResult observer_result, void * /*state*/)
  {
    fetch_count++;
    if (fetch_count == 1)
    {
      opentelemetry::nostd::get<0>(observer_result)->Observe(20l, {{"RequestType", "GET"}});
      opentelemetry::nostd::get<0>(observer_result)->Observe(10l, {{"RequestType", "PUT"}});
      number_of_get += 20l;
      number_of_put += 10l;
    }
    else if (fetch_count == 2)
    {
      opentelemetry::nostd::get<0>(observer_result)->Observe(40l, {{"RequestType", "GET"}});
      opentelemetry::nostd::get<0>(observer_result)->Observe(20l, {{"RequestType", "PUT"}});
      number_of_get += 40l;
      number_of_put += 20l;
    }
  }

  static void init_values()
  {
    fetch_count   = 0;
    number_of_get = 0;
    number_of_put = 0;
  }

  static size_t fetch_count;
  static long number_of_get;
  static long number_of_put;
  static const size_t number_of_attributes = 2;
};

size_t MeasurementFetcher::fetch_count;
long MeasurementFetcher::number_of_get;
long MeasurementFetcher::number_of_put;
const size_t MeasurementFetcher::number_of_attributes;

TEST_P(WritableMetricStorageTestFixture, TestAggregation)
{
  MeasurementFetcher::init_values();
  AggregationTemporality temporality = GetParam();

  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kObservableCounter,
                                     InstrumentValueType::kLong};

  auto sdk_start_ts = std::chrono::system_clock::now();
  // Some computation here
  auto collection_ts = std::chrono::system_clock::now() + std::chrono::seconds(5);

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);
  size_t count_attributes = 0;

  MeasurementFetcher measurement_fetcher;
  opentelemetry::sdk::metrics::AsyncMetricStorage storage(instr_desc, AggregationType::kSum,
                                                          new DefaultAttributesProcessor());
  long get_count                                                                  = 20l;
  long put_count                                                                  = 10l;
  size_t attribute_count                                                          = 2;
  std::unordered_map<MetricAttributes, long, AttributeHashGenerator> measurements = {
      {{{"RequestType", "GET"}}, get_count}, {{{"RequestType", "PUT"}}, put_count}};
  storage.RecordLong(measurements,
                     opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now()));

  storage.Collect(collector.get(), collectors, sdk_start_ts, collection_ts,
                  [&](const MetricData data) {
                    for (auto data_attr : data.point_data_attr_)
                    {
                      auto data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
                      if (opentelemetry::nostd::get<std::string>(
                              data_attr.attributes.find("RequestType")->second) == "GET")
                      {
                        EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), get_count);
                      }
                      else if (opentelemetry::nostd::get<std::string>(
                                   data_attr.attributes.find("RequestType")->second) == "PUT")
                      {
                        EXPECT_EQ(opentelemetry::nostd::get<long>(data.value_), put_count);
                      }
                    }
                    return true;
                  });
  EXPECT_EQ(MeasurementFetcher::number_of_attributes, attribute_count);
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestLong,
                         WritableMetricStorageTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

#endif