// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include "common.h"

#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/state/metric_collector.h"

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
  metric_producer->Produce();
}

TEST(MetricReaderTest, CardinalityLimitOptions)
{
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());

  // Test default values (all 0)
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kCounter), 0);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kHistogram), 0);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kUpDownCounter), 0);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableCounter), 0);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableGauge), 0);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableUpDownCounter), 0);
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kGauge), 0);
#endif

  // Set cardinality limit options
  CardinalityLimitOptions options;
  options.default_limit              = 1000;
  options.counter                    = 100;
  options.histogram                  = 200;
  options.up_down_counter            = 300;
  options.observable_counter         = 400;
  options.observable_gauge           = 500;
  options.observable_up_down_counter = 600;
  options.gauge                      = 700;

  metric_reader->SetCardinalityLimitOptions(options);

  // Test instrument-specific limits
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kCounter), 100);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kHistogram), 200);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kUpDownCounter), 300);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableCounter), 400);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableGauge), 500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableUpDownCounter), 600);
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kGauge), 700);
#endif
}

TEST(MetricReaderTest, CardinalityLimitOptionsDefaultFallback)
{
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());

  // Set only default_limit, leave instrument-specific limits at 0
  CardinalityLimitOptions options;
  options.default_limit = 1500;

  metric_reader->SetCardinalityLimitOptions(options);

  // All instrument types should fall back to default_limit
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kCounter), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kHistogram), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kUpDownCounter), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableCounter), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableGauge), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableUpDownCounter), 1500);
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kGauge), 1500);
#endif
}

TEST(MetricReaderTest, CardinalityLimitOptionsMixedConfiguration)
{
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());

  // Set default and only some instrument-specific limits
  CardinalityLimitOptions options;
  options.default_limit = 1000;
  options.counter       = 100;
  options.histogram     = 200;
  // Leave other instrument types at 0 to test fallback

  metric_reader->SetCardinalityLimitOptions(options);

  // Configured instruments use their specific values
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kCounter), 100);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kHistogram), 200);

  // Unconfigured instruments fall back to default
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kUpDownCounter), 1000);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableCounter), 1000);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableGauge), 1000);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableUpDownCounter), 1000);
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kGauge), 1000);
#endif
}
