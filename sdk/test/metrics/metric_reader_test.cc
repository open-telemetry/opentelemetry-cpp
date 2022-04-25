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

class MockMetricReader : public MetricReader
{
public:
  MockMetricReader(AggregationTemporality aggr_temporality) : MetricReader(aggr_temporality) {}

  virtual bool OnForceFlush(std::chrono::microseconds timeout) noexcept override { return true; }
  virtual bool OnShutDown(std::chrono::microseconds timeout) noexcept override { return true; }
  virtual void OnInitialized() noexcept override {}
};

TEST(MetricReaderTest, BasicTests)
{
  AggregationTemporality aggr_temporality = AggregationTemporality::kDelta;
  std::unique_ptr<MetricReader> metric_reader1(new MockMetricReader(aggr_temporality));
  EXPECT_EQ(metric_reader1->GetAggregationTemporality(), aggr_temporality);

  std::shared_ptr<MeterContext> meter_context1(new MeterContext());
  EXPECT_NO_THROW(meter_context1->AddMetricReader(std::move(metric_reader1)));

  std::unique_ptr<MetricReader> metric_reader2(new MockMetricReader(aggr_temporality));
  std::shared_ptr<MeterContext> meter_context2(new MeterContext());
  MetricProducer *metric_producer =
      new MetricCollector(std::move(meter_context2), std::move(metric_reader2));
  EXPECT_NO_THROW(metric_producer->Collect([](ResourceMetrics &metric_data) { return true; }));
}
#endif
