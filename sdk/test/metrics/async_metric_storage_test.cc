// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/async_metric_storage.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/observer_result.h"

#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/resource/resource.h"

#  include <gtest/gtest.h>
#  include <map>

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::instrumentationlibrary;
using namespace opentelemetry::sdk::resource;

class MockCollectorHandle : public CollectorHandle
{
public:
  AggregationTemporarily GetAggregationTemporarily() noexcept override
  {
    return AggregationTemporarily::kCummulative;
  }
};

void measurement_fetch(opentelemetry::metrics::ObserverResult<long> &observer_result)
{
  observer_result.Observe(20l);
  observer_result.Observe(10l);
}

TEST(AsyncMetricStorageTest, BasicTests)
{
  auto metric_callback            = [](MetricData &metric_data) { return true; };
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kLong};

  MockCollectorHandle collector;
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  auto sdk_start_ts = std::chrono::system_clock::now();
  // Some computation here
  auto collection_ts = std::chrono::system_clock::now() + std::chrono::seconds(5);

  opentelemetry::sdk::metrics::AsyncMetricStorage<long> storage(
      instr_desc, AggregationType::kSum, &measurement_fetch, new DefaultAttributesProcessor());
  EXPECT_NO_THROW(
      storage.Collect(&collector, collectors, sdk_start_ts, collection_ts, metric_callback));
}
#endif