// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/async_metric_storage.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
# include "opentelemetry/sdk/metrics/observer_result.h"
#  include "opentelemetry/sdk/metrics/instruments.h"

#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/resource/resource.h"

#  include <gtest/gtest.h>
#  include <map>

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::instrumentationlibrary;
using namespace opentelemetry::sdk::resource;


void measurement_fetch (opentelemetry::metrics::ObserverResult<long>& observer_result)
{
    observer_result.Observe(20l);
    observer_result.Observe(10l);
}

TEST(AsyncMetricStorageTest, BasicTests)
{
  auto metric_callback = [](MetricData &metric_data) { return true;};
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kLong};
  auto instrumentation_library = InstrumentationLibrary::Create("instr_lib");
  auto resource = Resource::Create({});
  MetricCollector* collector;
  std::vector<MetricCollector*> collectors;

  opentelemetry::sdk::metrics::AsyncMetricStorage<long> storage(instr_desc, AggregationType::kSum, &measurement_fetch, new DefaultAttributesProcessor());
  EXPECT_NO_THROW(storage.Collect(collector, collectors, instrumentation_library.get(), &resource, metric_callback ));
}
#endif