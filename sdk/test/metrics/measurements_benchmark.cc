// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

#include <benchmark/benchmark.h>
#include <memory>
#include <random>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::common;

class MockMetricExporter : public MetricReader
{
public:
  MockMetricExporter() = default;

  opentelemetry::sdk::metrics::AggregationTemporality GetAggregationTemporality(
      opentelemetry::sdk::metrics::InstrumentType) const noexcept override
  {
    return AggregationTemporality::kCumulative;
  }

private:
  bool OnForceFlush(std::chrono::microseconds /*timeout*/) noexcept override { return true; }

  bool OnShutDown(std::chrono::microseconds /*timeout*/) noexcept override { return true; }

  void OnInitialized() noexcept override {}
};

namespace
{
void BM_MeasurementsTest(benchmark::State &state)
{
  MeterProvider mp;
  auto m = mp.GetMeter("meter1", "version1", "schema1");

  std::shared_ptr<MetricReader> exporter(new MockMetricExporter());
  mp.AddMetricReader(exporter);
  auto h = m->CreateDoubleCounter("counter1", "counter1_description", "counter1_unit");
  std::vector<SumPointData> actuals;
  std::vector<std::thread> collectionThreads;
  std::function<void()> collectMetrics = [&exporter, &actuals]() {
    exporter->Collect([&](ResourceMetrics &rm) {
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
  };
  constexpr int NUM_CORES           = 1;
  constexpr size_t MAX_MEASUREMENTS = 1000000;
  std::atomic<long long> measurements_processed{0l};
  while (state.KeepRunning())
  {
    measurements_processed = 0;
    while (measurements_processed++ <= MAX_MEASUREMENTS)
    {
      size_t val1 = rand() % 10;
      size_t val2 = rand() % 10;
      size_t val3 = rand() % 10;
      h->Add(1.0, {{"dim1", val1}, {"dim2", val2}, {"dim3", val3}},
             opentelemetry::context::Context{});
    }
  }
}

BENCHMARK(BM_MeasurementsTest);

}  // namespace
BENCHMARK_MAIN();