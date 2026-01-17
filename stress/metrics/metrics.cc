// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "stress.h"

#include <chrono>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_options.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/meter_provider_factory.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/metrics/state/filtered_ordered_attribute_map.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector_factory.h"
#include "opentelemetry/sdk/metrics/view/meter_selector.h"
#include "opentelemetry/sdk/metrics/view/meter_selector_factory.h"
#include "opentelemetry/sdk/metrics/view/view.h"
#include "opentelemetry/sdk/metrics/view/view_factory.h"

Stress *globalStressTest = nullptr;

void signalHandler(int)
{
  if (globalStressTest)
  {
    globalStressTest->stop();
  }
}

namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace metrics_api = opentelemetry::metrics;

namespace
{
class MockMetricExporter : public opentelemetry::sdk::metrics::PushMetricExporter
{
public:
  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::sdk::metrics::ResourceMetrics & /*data*/) noexcept override
  {
    // Ignore all metrics and return success
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  bool ForceFlush(std::chrono::microseconds /*timeout*/) noexcept override
  {
    return true;  // No-op
  }

  bool Shutdown(std::chrono::microseconds /*timeout*/) noexcept override
  {
    return true;  // No-op
  }

  opentelemetry::sdk::metrics::AggregationTemporality GetAggregationTemporality(
      opentelemetry::sdk::metrics::InstrumentType) const noexcept override
  {
    return opentelemetry::sdk::metrics::AggregationTemporality::kDelta;
  }
};

// Pre-generate random attributes
std::vector<std::map<std::string, uint32_t>> GenerateAttributeSet(size_t count)
{
  std::vector<std::map<std::string, uint32_t>> attributes_set;
  for (size_t i = 0; i < count; ++i)
  {
    std::map<std::string, uint32_t> attributes;
    attributes["dim1"] = rand() % 100;  // Random value between 0 and 99
    attributes["dim2"] = rand() % 100;  // Random value between 0 and 99
    attributes["dim3"] = rand() % 100;  // Random value between 0 and 99
    attributes_set.push_back(attributes);
  }
  return attributes_set;
}

void InitMetrics(const std::string /*&name*/)
{
  metrics_sdk::PeriodicExportingMetricReaderOptions options;
  options.export_interval_millis = std::chrono::milliseconds(1000);
  options.export_timeout_millis  = std::chrono::milliseconds(500);
  auto exporter                  = std::make_unique<MockMetricExporter>();
  auto reader =
      metrics_sdk::PeriodicExportingMetricReaderFactory::Create(std::move(exporter), options);
  auto provider = metrics_sdk::MeterProviderFactory::Create();
  provider->AddMetricReader(std::move(reader));
  std::shared_ptr<opentelemetry::metrics::MeterProvider> api_provider(std::move(provider));
  metrics_api::Provider::SetMeterProvider(api_provider);
}

void CleanupMetrics()
{
  std::shared_ptr<metrics_api::MeterProvider> none;
  metrics_api::Provider::SetMeterProvider(none);
}

void CounterExample(opentelemetry::nostd::unique_ptr<metrics_api::Counter<double>> &counter,
                    const std::vector<std::map<std::string, uint32_t>> &attributes_set)
{
  // Pick a random attribute set
  size_t random_index    = rand() % attributes_set.size();
  const auto &attributes = attributes_set[random_index];

  // Record the metric with the selected attributes
  counter->Add(
      1.0, opentelemetry::common::KeyValueIterableView<std::map<std::string, uint32_t>>(attributes),
      opentelemetry::context::Context{});
}
}  // namespace

int main(int /*argc*/, char ** /*argv[]*/)
{
  std::srand(static_cast<unsigned int>(std::time(nullptr)));  // Seed the random number generator
  // Pre-generate a set of random attributes
  size_t attribute_count = 1000;  // Number of attribute sets to pre-generate
  auto attributes_set    = GenerateAttributeSet(attribute_count);

  InitMetrics("metrics_stress_test");
  auto provider = metrics_api::Provider::GetMeterProvider();
  auto meter    = provider->GetMeter("metrics_stress_test", "1.0.0");
  auto counter  = meter->CreateDoubleCounter("metrics_stress_test_counter");
  auto func     = [&counter, &attributes_set]() { CounterExample(counter, attributes_set); };
  Stress stressTest(func, std::thread::hardware_concurrency());
  globalStressTest = &stressTest;
  std::signal(SIGINT, signalHandler);
  stressTest.run();
  CleanupMetrics();
  return 0;
}
