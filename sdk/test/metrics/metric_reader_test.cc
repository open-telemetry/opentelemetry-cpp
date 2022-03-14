// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include <gtest/gtest.h>
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/metrics/metric_exporter.h"

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationlibrary;
using namespace opentelemetry::sdk::metrics;

#  include <iostream>

class MockMetricReader : public MetricReader
{
public:
  MockMetricReader(AggregationTemporarily aggr_temporarily) : MetricReader(aggr_temporarily) {}

  virtual bool OnForceFlush() noexcept override { return true; }

  virtual bool OnShutDown() noexcept override { return true; }

  virtual void OnInitialized() noexcept override {}
};

TEST(MetricReaderTest, BasicTests)
{
  AggregationTemporarily aggr_temporarily = AggregationTemporarily::kDelta;
  std::unique_ptr<MetricReader> metric_reader1(new MockMetricReader(aggr_temporarily));
  EXPECT_EQ(metric_reader1->GetAggregationTemporarily(), aggr_temporarily);

  std::vector<std::unique_ptr<sdk::metrics::MetricExporter>> exporters;
  std::shared_ptr<MeterContext> meter_context1(new MeterContext(std::move(exporters)));
  EXPECT_NO_THROW(meter_context1->AddMetricReader(std::move(metric_reader1)));

  std::unique_ptr<MetricReader> metric_reader2(new MockMetricReader(aggr_temporarily));
  std::shared_ptr<MeterContext> meter_context2(new MeterContext(std::move(exporters)));
  MetricProducer *metric_producer =
      new MetricCollector(std::move(meter_context2), std::move(metric_reader2));
  EXPECT_NO_THROW(metric_producer->Collect([](MetricData data) { return true; }));
}
#endif
