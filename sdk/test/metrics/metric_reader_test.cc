// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include "common.h"

#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/metrics/cardinality_limits.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
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

TEST(MetricReaderTest, CardinalityLimitsDefaults)
{
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());

  // Default values are kAggregationCardinalityLimit (2000) for all instrument types
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kCounter),
            kAggregationCardinalityLimit);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kHistogram),
            kAggregationCardinalityLimit);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kUpDownCounter),
            kAggregationCardinalityLimit);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableCounter),
            kAggregationCardinalityLimit);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableGauge),
            kAggregationCardinalityLimit);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableUpDownCounter),
            kAggregationCardinalityLimit);
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kGauge),
            kAggregationCardinalityLimit);
#endif
}

TEST(MetricReaderTest, CardinalityLimitsPerInstrumentType)
{
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());

  // Override every field
  CardinalityLimits limits;
  limits.default_limit              = 1000;
  limits.counter                    = 100;
  limits.histogram                  = 200;
  limits.up_down_counter            = 300;
  limits.observable_counter         = 400;
  limits.observable_gauge           = 500;
  limits.observable_up_down_counter = 600;
  limits.gauge                      = 700;

  metric_reader->SetCardinalityLimits(limits);

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

TEST(MetricReaderTest, CardinalityLimitsOverrideDefaultLimit)
{
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());

  // Explicitly set all fields — callers are responsible for resolving
  // default_limit into per-instrument fields before calling SetCardinalityLimits.
  // sdk_builder.cc handles this resolution for YAML config (where 0 = unset).
  CardinalityLimits limits;
  limits.default_limit              = 1500;
  limits.counter                    = 100;
  limits.histogram                  = 200;
  limits.up_down_counter            = 1500;  // explicitly set to default_limit
  limits.observable_counter         = 1500;
  limits.observable_gauge           = 1500;
  limits.observable_up_down_counter = 1500;
  limits.gauge                      = 1500;

  metric_reader->SetCardinalityLimits(limits);

  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kCounter), 100);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kHistogram), 200);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kUpDownCounter), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableCounter), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableGauge), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kObservableUpDownCounter), 1500);
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kGauge), 1500);
#endif
}

// Verify that a user-configured default_limit of 1500 with an explicit
// per-instrument override of 2000 (the SDK default) is honoured as written.
// The value 2000 must be treated as an explicit user choice, not collapsed
// back to the default_limit.  This matters when the YAML config uses
// CardinalityLimitsConfiguration where 0 means "unset", but any non-zero
// value (including 2000) is an intentional override.
TEST(MetricReaderTest, CardinalityLimitsExplicitSdkDefaultIsHonoured)
{
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());

  // Simulate: user sets default=1500, counter=2000 (explicit, not "unset")
  CardinalityLimits limits;
  limits.default_limit = 1500;
  limits.counter       = 2000;  // explicitly set to the SDK default value
  // all other fields explicitly set to default_limit (1500)
  limits.histogram                  = 1500;
  limits.up_down_counter            = 1500;
  limits.observable_counter         = 1500;
  limits.observable_gauge           = 1500;
  limits.observable_up_down_counter = 1500;
  limits.gauge                      = 1500;

  metric_reader->SetCardinalityLimits(limits);

  // counter was explicitly set to 2000 — must be returned as 2000
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kCounter), 2000);
  // histogram was not explicitly set — falls back to default_limit 1500
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kHistogram), 1500);
  EXPECT_EQ(metric_reader->GetCardinalityLimit(InstrumentType::kUpDownCounter), 1500);
}
