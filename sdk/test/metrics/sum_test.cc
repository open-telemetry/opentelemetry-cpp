// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/common/macros.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

#include <gtest/gtest.h>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::metrics;

class MockMetricExporter : public PushMetricExporter
{
public:
  MockMetricExporter() = default;
  opentelemetry::sdk::common::ExportResult Export(
      const ResourceMetrics & /* records */) noexcept override
  {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  AggregationTemporality GetAggregationTemporality(
      InstrumentType /* instrument_type */) const noexcept override
  {
    return AggregationTemporality::kCumulative;
  }

  bool ForceFlush(std::chrono::microseconds /* timeout */) noexcept override { return true; }

  bool Shutdown(std::chrono::microseconds /* timeout */) noexcept override { return true; }
};

class MockMetricReader : public MetricReader
{
public:
  MockMetricReader(std::unique_ptr<PushMetricExporter> exporter) : exporter_(std::move(exporter)) {}
  AggregationTemporality GetAggregationTemporality(
      InstrumentType instrument_type) const noexcept override
  {
    return exporter_->GetAggregationTemporality(instrument_type);
  }
  virtual bool OnForceFlush(std::chrono::microseconds /* timeout */) noexcept override
  {
    return true;
  }
  virtual bool OnShutDown(std::chrono::microseconds /* timeout */) noexcept override
  {
    return true;
  }
  virtual void OnInitialized() noexcept override {}

private:
  std::unique_ptr<PushMetricExporter> exporter_;
};

TEST(HistogramToSum, Double)
{
  MeterProvider mp;
  auto m = mp.GetMeter("meter1", "version1", "schema1");

  std::unique_ptr<MockMetricExporter> exporter(new MockMetricExporter());
  std::shared_ptr<MetricReader> reader{new MockMetricReader(std::move(exporter))};
  mp.AddMetricReader(reader);

  std::unique_ptr<View> view{new View("view1", "view1_description", AggregationType::kSum)};
  std::unique_ptr<InstrumentSelector> instrument_selector{
      new InstrumentSelector(InstrumentType::kHistogram, "histogram1")};
  std::unique_ptr<MeterSelector> meter_selector{new MeterSelector("meter1", "version1", "schema1")};
  mp.AddView(std::move(instrument_selector), std::move(meter_selector), std::move(view));

  auto h = m->CreateDoubleHistogram("histogram1", "histogram1_description", "histogram1_unit");

  h->Record(5, {});
  h->Record(10, {});
  h->Record(15, {});
  h->Record(20, {});
  h->Record(25, {});
  h->Record(30, {});
  h->Record(35, {});
  h->Record(40, {});
  h->Record(45, {});
  h->Record(50, {});
  h->Record(1e6, {});

  std::vector<SumPointData> actuals;
  reader->Collect([&](ResourceMetrics &rm) {
    for (const ScopeMetrics &smd : rm.scope_metric_data_)
    {
      for (const MetricData &md : smd.metric_data_)
      {
        for (const PointDataAttributes &dp : md.point_data_attr_)
        {
          actuals.push_back(opentelemetry::nostd::get<SumPointData>(dp.point_data));
        }
      }
    }
    return true;
  });

  ASSERT_EQ(1, actuals.size());

  const auto &actual = actuals.at(0);
  ASSERT_EQ(1000275.0, opentelemetry::nostd::get<double>(actual.value_));
#if 0
  ASSERT_EQ(11, actual.count_);
  ASSERT_EQ(5.0, opentelemetry::nostd::get<double>(actual.min_));
  ASSERT_EQ(1e6, opentelemetry::nostd::get<double>(actual.max_));
  ASSERT_EQ(std::vector<double>(
                {0, 5, 10, 25, 50, 75, 100, 250, 500, 750, 1000, 2500, 5000, 7500, 10000}),
            actual.boundaries_);
  ASSERT_EQ(std::vector<uint64_t>({0, 1, 1, 3, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}),
            actual.counts_);
#endif
}