// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include <memory>
#  include <thread>
#  include "opentelemetry/exporters/prometheus/exporter.h"
#  include "opentelemetry/metrics/provider.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#  include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h"
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/sdk/metrics/meter_provider.h"

#  ifdef BAZEL_BUILD
#    include "examples/common/metrics_foo_library/foo_library.h"
#  else
#    include "metrics_foo_library/foo_library.h"
#  endif

namespace metric_sdk      = opentelemetry::sdk::metrics;
namespace nostd           = opentelemetry::nostd;
namespace common          = opentelemetry::common;
namespace exportermetrics = opentelemetry::exporter::metrics;
namespace metrics_api     = opentelemetry::metrics;

namespace
{

void initMetrics(const std::string &name)
{
  std::string addr{"localhost:8080"};
  std::puts("PrometheusExporter example program running ...");

  std::unique_ptr<metric_sdk::MetricExporter> exporter{
      new exportermetrics::PrometheusExporter(addr)};
  std::vector<std::unique_ptr<metric_sdk::MetricExporter>> exporters;

  std::string version{"1.2.0"};
  std::string schema{"https://opentelemetry.io/schemas/1.2.0"};

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

  // counter view
  std::unique_ptr<metric_sdk::InstrumentSelector> instrument_selector{
      new metric_sdk::InstrumentSelector(metric_sdk::InstrumentType::kCounter, name)};
  std::unique_ptr<metric_sdk::MeterSelector> meter_selector{
      new metric_sdk::MeterSelector(name, version, schema)};
  std::unique_ptr<metric_sdk::View> sum_view{
      new metric_sdk::View{name, "description", metric_sdk::AggregationType::kSum}};
  p->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(sum_view));

  // histogram view
  std::unique_ptr<metric_sdk::InstrumentSelector> histogram_instrument_selector{
      new metric_sdk::InstrumentSelector(metric_sdk::InstrumentType::kHistogram, name)};
  std::unique_ptr<metric_sdk::MeterSelector> histogram_meter_selector{
      new metric_sdk::MeterSelector(name, version, schema)};
  std::unique_ptr<metric_sdk::View> histogram_view{
      new metric_sdk::View{name, "description", metric_sdk::AggregationType::kHistogram}};
  p->AddView(std::move(histogram_instrument_selector), std::move(histogram_meter_selector),
             std::move(histogram_view));
  metrics_api::Provider::SetMeterProvider(provider);
}
}  // namespace
int main(int argc, char **argv)
{
  if (argc != 2)
  {
    std::puts("invalid number of command line arguments");
    return 0;
  }

  std::string example_type(argv[1]);
  std::string name{"prometheus_example"};
  initMetrics(name);

  if (example_type == "counter")
  {
    foo_library::counter_example(name);
  }
  else
  {
    foo_library::histogram_example(name);
  }
}
#else
int main() {}
#endif
