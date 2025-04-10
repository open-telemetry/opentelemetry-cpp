#include <thread>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <vector>
#include <memory>

#pragma comment(lib,"advapi32")
#include "otel_sdk.h"
#include "otel_api.h"

#include "opentelemetry/sdk/version/version.h"

// api headers (logs, traces, metrics)
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/trace/provider.h"

// sdk providers (logs, traces, metrics)
#include "opentelemetry/sdk/logs/logger_provider_factory.h"
#include "opentelemetry/sdk/metrics/meter_provider_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"

// sdk metric views
#include "opentelemetry/sdk/metrics/view/instrument_selector_factory.h"
#include "opentelemetry/sdk/metrics/view/meter_selector_factory.h"
#include "opentelemetry/sdk/metrics/view/view_factory.h"

// sdk processors (simple logs, simple traces, periodic exporting metrics)
#include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"

// sdk exporter factories (logs, traces, metrics)
#include "opentelemetry/exporters/ostream/log_record_exporter.h"
#include "opentelemetry/exporters/ostream/metric_exporter_factory.h"
#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_forward_proxy.h"
#include "opentelemetry/exporters/otlp/otlp_environment.h"

#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/sdk/common/global_log_handler.h>
#include <opentelemetry/sdk/common/env_variables.h>
#include <opentelemetry/sdk/resource/semantic_conventions.h>
#include <opentelemetry/sdk/metrics/meter_provider_factory.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h>

#include <opentelemetry/version.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/sdk/common/global_log_handler.h>
#include <opentelemetry/sdk/common/env_variables.h>
#include <opentelemetry/sdk/resource/semantic_conventions.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include <opentelemetry/sdk/trace/batch_span_processor_options.h>
#include <opentelemetry/sdk/trace/batch_span_processor_factory.h>
#include "opentelemetry/sdk/trace/processor.h"
#include <opentelemetry/sdk/logs/logger_provider_factory.h>
#include <opentelemetry/sdk/logs/batch_log_record_processor_options.h>
#include <opentelemetry/sdk/logs/batch_log_record_processor_factory.h>
#include <opentelemetry/sdk/logs/processor.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h>

#include <opentelemetry/logs/provider.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/metrics/provider.h>

#include <opentelemetry/sdk/common/global_log_handler.h>
#include <opentelemetry/sdk/common/env_variables.h>

#include <opentelemetry/sdk/resource/semantic_conventions.h>

#include <opentelemetry/sdk/logs/logger_provider_factory.h>
#include <opentelemetry/sdk/logs/simple_log_record_processor_factory.h>
#include <opentelemetry/sdk/logs/batch_log_record_processor_options.h>
#include <opentelemetry/sdk/logs/batch_log_record_processor_factory.h>
#include <opentelemetry/sdk/logs/processor.h>

#include <opentelemetry/sdk/metrics/meter_provider_factory.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h>
#include <opentelemetry/sdk/metrics/view/instrument_selector_factory.h>
#include <opentelemetry/sdk/metrics/view/meter_selector_factory.h>
#include <opentelemetry/sdk/metrics/view/view_factory.h>

#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h>

#include "opentelemetry/exporters/ostream/log_record_exporter.h"
#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/exporters/ostream/metric_exporter_factory.h"

int main(int argc, const char* argv[])
{
  using namespace opentelemetry;

  otel_sdk::instance instance;
  if( !otel_api::enabled() )
  {
    fprintf(stderr,"FATAL: Otel not enabled\n");
    return 1;
  }
  {
    using namespace sdk::common::internal_log;
    GlobalLogHandler::SetLogLevel(LogLevel::Debug);
  }

  int index = argc == 2 ? atoi(argv[1]) : -1;

  setvbuf(stdout, nullptr, _IONBF,0);
  setvbuf(stderr, nullptr, _IONBF,0);
  setvbuf(stdin, nullptr, _IONBF,0);

  //exporter::otlp::OtlpGrpcMetricExporterOptions exporterOptions;
  auto exporter{ exporter::otlp::OtlpGrpcMetricExporterFactory::Create( {} ) };
  auto stdoutExporter = exporter::metrics::OStreamMetricExporterFactory::Create();

  // Initialize and set the global MeterProvider
  sdk::metrics::PeriodicExportingMetricReaderOptions options;
  options.export_interval_millis = std::chrono::milliseconds(500);
  options.export_timeout_millis  = std::chrono::milliseconds(250);
  auto reader{ sdk::metrics::PeriodicExportingMetricReaderFactory::Create(std::move(exporter), options) };
  auto stdoutReader{ sdk::metrics::PeriodicExportingMetricReaderFactory::Create(std::move(stdoutExporter), options) };
  auto meterProvider{ sdk::metrics::MeterProviderFactory::Create() };
  meterProvider->AddMetricReader(std::move(reader));
  meterProvider->AddMetricReader(std::move(stdoutReader));
  metrics::Provider::SetMeterProvider(std::move(meterProvider));

  std::unique_ptr<metrics::Counter<double>> counter;

  auto provider = metrics::Provider::GetMeterProvider();
  auto meter{ provider->GetMeter("malkia_test_meter", "1.2.0") };
  std::vector<std::unique_ptr<metrics::Counter<double>>> counters;
  counters.reserve(10);
  for(int i=0; i<10; i++)
  {
    char buf[100];
    sprintf(buf,"counter_%d", i);
    counters.push_back( std::move( meter->CreateDoubleCounter(buf)));
  }
  for(int i=0; i<10; i++)
  {
      counters[i]->Add(1);
  }
  Sleep(1000);

  // if( const auto sp{ 
  //     std::dynamic_pointer_cast<sdk::metrics::MeterProvider>( 
  //       metrics::Provider::GetMeterProvider() ) }; sp )
  // {
  //   if( const auto p{ sp.get() }; p )
  //   {
  //     p->ForceFlush();
  //     p->Shutdown();
  //   }
  // }
  // metrics::Provider::SetMeterProvider({});

  FatalExit(0);
  _Exit(0);

  return 0;
}
