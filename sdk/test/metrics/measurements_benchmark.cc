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
  std::vector<std::thread> collectionThreads;
  constexpr size_t MAX_MEASUREMENTS    = 1000000;
  constexpr size_t POSSIBLE_ATTRIBUTES = 1000;
  std::map<std::string, size_t> attributes[POSSIBLE_ATTRIBUTES];
  size_t total_index = 0;
  for (size_t i = 0; i < 10; i++)
  {
    for (size_t j = 0; j < 10; j++)
      for (size_t k = 0; k < 10; k++)
        attributes[total_index++] = {{"dim1", i}, {"dim2", j}, {"dim3", k}};
  }
  while (state.KeepRunning())
  {
    for (size_t i = 0; i < MAX_MEASUREMENTS; i++)
    {
      size_t index = rand() % POSSIBLE_ATTRIBUTES;
      h->Add(1.0, attributes[index], opentelemetry::context::Context{});
    }
  }
  exporter->Collect([&](ResourceMetrics &rm) { return true; });
}
BENCHMARK(BM_MeasurementsTest);

}  // namespace
BENCHMARK_MAIN();
