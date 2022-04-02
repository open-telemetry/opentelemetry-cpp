// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#ifndef ENABLE_METRICS_PREVIEW
#  include <chrono>
#  include <thread>
#  include "opentelemetry/exporters/ostream/metric_exporter.h"
#  include "opentelemetry/metrics/provider.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#  include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h"
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/sdk/metrics/meter_provider.h"

#  include <iostream>

namespace metric_sdk      = opentelemetry::sdk::metrics;
namespace nostd           = opentelemetry::nostd;
namespace common          = opentelemetry::common;
namespace exportermetrics = opentelemetry::exporter::metrics;
namespace metrics_api     = opentelemetry::metrics;

void sync_sum()
{
  std::unique_ptr<metric_sdk::MetricExporter> exporter{new exportermetrics::OStreamMetricExporter};
  std::vector<std::unique_ptr<metric_sdk::MetricExporter>> exporters;

  std::string name{"ostream_metric_example"};
  std::string version{"1.2.0"};
  std::string schema{"ostream_metric_example"};

  // Initialize and set the global MeterProvider
  metric_sdk::PeriodicExportingMetricReaderOptions options;
  options.export_interval_millis = std::chrono::milliseconds(1000);
  options.export_timeout_millis  = std::chrono::milliseconds(500);
  std::unique_ptr<metric_sdk::MetricReader> reader{
      new metric_sdk::PeriodicExportingMetricReader(std::move(exporter), options)};
  auto provider = std::shared_ptr<metrics_api::MeterProvider>(
      new metric_sdk::MeterProvider(std::move(exporters)));
  auto p = std::static_pointer_cast<metric_sdk::MeterProvider>(provider);
  p->AddMetricReader(std::move(reader));
  std::unique_ptr<metric_sdk::InstrumentSelector> instrument_selector{
      new metric_sdk::InstrumentSelector(metric_sdk::InstrumentType::kCounter, name)};
  std::unique_ptr<metric_sdk::MeterSelector> meter_selector{
      new metric_sdk::MeterSelector(name, version, schema)};
  std::unique_ptr<metric_sdk::View> view{
      new metric_sdk::View{name, "description", metric_sdk::AggregationType::kSum}};
  p->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(view));
  metrics_api::Provider::SetMeterProvider(provider);

  // Get the Meter from the MeterProvider
  nostd::shared_ptr<metrics_api::Meter> meter = provider->GetMeter(name, "1.2.0");
  auto double_counter                         = meter->CreateDoubleCounter(name);
  double_counter->Add(28.5);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  double_counter->Add(3.14);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  double_counter->Add(23.5);
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  p->ForceFlush();
}

int main()
{
  sync_sum();
}
#endif
