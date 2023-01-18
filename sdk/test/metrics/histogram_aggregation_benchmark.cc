// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <benchmark/benchmark.h>

#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

#include <random>

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

namespace
{
void BM_HistogramAggregation(benchmark::State &state)
{
  MeterProvider mp;
  auto m = mp.GetMeter("meter1", "version1", "schema1");

  std::unique_ptr<MockMetricExporter> exporter(new MockMetricExporter());
  std::shared_ptr<MetricReader> reader{new MockMetricReader(std::move(exporter))};
  mp.AddMetricReader(reader);
  auto h = m->CreateDoubleHistogram("histogram1", "histogram1_description", "histogram1_unit");
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0, 1000000);
  while (state.KeepRunning())
  {
    auto value = (double)distribution(generator);
    h->Record(value, {});
  }
}
BENCHMARK(BM_HistogramAggregation);

}  // namespace
BENCHMARK_MAIN();
