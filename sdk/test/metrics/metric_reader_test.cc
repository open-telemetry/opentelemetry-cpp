// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "common.h"

#include <gtest/gtest.h>
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::metrics;

TEST(MetricReaderTest, BasicTests)
{
  std::unique_ptr<MetricReader> metric_reader1(new MockMetricReader());
  EXPECT_EQ(metric_reader1->GetAggregationTemporality(InstrumentType::kCounter),
            AggregationTemporality::kCumulative);

  std::shared_ptr<MeterContext> meter_context1(new MeterContext());
  meter_context1->AddMetricReader(std::move(metric_reader1));

  std::unique_ptr<MetricReader> metric_reader2(new MockMetricReader());
  std::shared_ptr<MeterContext> meter_context2(new MeterContext());
  std::shared_ptr<MetricProducer> metric_producer{
      new MetricCollector(meter_context2.get(), std::move(metric_reader2))};
  metric_producer->Collect([](ResourceMetrics & /* metric_data */) { return true; });
}
