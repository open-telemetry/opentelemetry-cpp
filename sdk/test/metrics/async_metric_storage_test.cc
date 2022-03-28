// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/async_metric_storage.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/metrics/metric_exporter.h"
#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include "opentelemetry/sdk/metrics/observer_result.h"
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"

#  include <gtest/gtest.h>
#  include <vector>

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::instrumentationlibrary;
using namespace opentelemetry::sdk::resource;

class MockMetricReader : public MetricReader
{
public:
  MockMetricReader(AggregationTemporality aggr_temporality) : MetricReader(aggr_temporality) {}

  virtual bool OnForceFlush(std::chrono::microseconds timeout) noexcept override { return true; }

  virtual bool OnShutDown(std::chrono::microseconds timeout) noexcept override { return true; }

  virtual void OnInitialized() noexcept override {}
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

  std::vector<std::unique_ptr<opentelemetry::sdk::metrics::MetricExporter>> exporters;
  std::shared_ptr<MeterContext> meter_context(new MeterContext(std::move(exporters)));
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader(AggregationTemporality::kDelta));

  std::shared_ptr<CollectorHandle> collector = std::shared_ptr<CollectorHandle>(
      new MetricCollector(std::move(meter_context), std::move(metric_reader)));

  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};

  opentelemetry::sdk::metrics::AsyncMetricStorage<long> storage(
      instr_desc, AggregationType::kSum, &measurement_fetch, new DefaultAttributesProcessor());
  storage.Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), metric_callback);
}
#endif