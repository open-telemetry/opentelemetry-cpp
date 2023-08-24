// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_context_factory.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/meter_provider_factory.h"

#include <memory>
#include <thread>

#ifdef BAZEL_BUILD
#  include "examples/common/metrics_foo_library/foo_library.h"
#else
#  include "metrics_foo_library/foo_library.h"
#endif

namespace metric_sdk    = opentelemetry::sdk::metrics;
namespace common        = opentelemetry::common;
namespace metrics_api   = opentelemetry::metrics;
namespace otlp_exporter = opentelemetry::exporter::otlp;

namespace
{

otlp_exporter::OtlpGrpcMetricExporterOptions exporter_options;

void InitMetrics()
{
  auto exporter = otlp_exporter::OtlpGrpcMetricExporterFactory::Create(exporter_options);

  std::string version{"1.2.0"};
  std::string schema{"https://opentelemetry.io/schemas/1.2.0"};

  // Initialize and set the global MeterProvider
  metric_sdk::PeriodicExportingMetricReaderOptions reader_options;
  reader_options.export_interval_millis = std::chrono::milliseconds(1000);
  reader_options.export_timeout_millis  = std::chrono::milliseconds(500);

  auto reader =
      metric_sdk::PeriodicExportingMetricReaderFactory::Create(std::move(exporter), reader_options);

  auto context = metric_sdk::MeterContextFactory::Create();
  context->AddMetricReader(std::move(reader));

  auto u_provider = metric_sdk::MeterProviderFactory::Create(std::move(context));
  std::shared_ptr<opentelemetry::metrics::MeterProvider> provider(std::move(u_provider));

  metrics_api::Provider::SetMeterProvider(provider);
}

void CleanupMetrics()
{
  std::shared_ptr<metrics_api::MeterProvider> none;
  metrics_api::Provider::SetMeterProvider(none);
}
}  // namespace

int main(int argc, char *argv[])
{
  std::string example_type;
  if (argc > 1)
  {
    exporter_options.endpoint = argv[1];
    if (argc > 2)
    {
      example_type = argv[2];
      if (argc > 3)
      {
        exporter_options.use_ssl_credentials         = true;
        exporter_options.ssl_credentials_cacert_path = argv[3];
      }
    }
  }
  // Removing this line will leave the default noop MetricProvider in place.
  InitMetrics();
  std::string name{"otlp_grpc_metric_example"};

  if (example_type == "counter")
  {
    foo_library::counter_example(name);
  }
  else if (example_type == "observable_counter")
  {
    foo_library::observable_counter_example(name);
  }
  else if (example_type == "histogram")
  {
    foo_library::histogram_example(name);
  }
  else
  {
    std::thread counter_example{&foo_library::counter_example, name};
    std::thread observable_counter_example{&foo_library::observable_counter_example, name};
    std::thread histogram_example{&foo_library::histogram_example, name};

    counter_example.join();
    observable_counter_example.join();
    histogram_example.join();
  }

  CleanupMetrics();
}
