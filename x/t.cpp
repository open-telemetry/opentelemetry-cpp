#include <thread>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <vector>

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

    int index = argc == 2 ? atoi(argv[1]) : -1;

  setvbuf(stdout, nullptr, _IONBF,0);
  setvbuf(stderr, nullptr, _IONBF,0);
  setvbuf(stdin, nullptr, _IONBF,0);

  {
    using namespace opentelemetry::sdk::common::internal_log;
    GlobalLogHandler::SetLogLevel(LogLevel::Debug);

  }

  {
    using namespace opentelemetry;
    using namespace sdk::resource;
    ResourceAttributes resources
    {
            { SemanticConventions::kProcessVpid, index },
    };
    auto resource = Resource::Create( resources );
    Resource::GetDefault() = resource;
  }

  //opentelemetry::exporter::otlp::OtlpGrpcMetricExporterOptions exporterOptions;
  auto exporter{ opentelemetry::exporter::otlp::OtlpGrpcMetricExporterFactory::Create( {} ) };
  auto stdoutExporter = opentelemetry::exporter::metrics::OStreamMetricExporterFactory::Create();

  // Initialize and set the global MeterProvider
  opentelemetry::sdk::metrics::PeriodicExportingMetricReaderOptions options;
  options.export_interval_millis = std::chrono::milliseconds(5000);
  options.export_timeout_millis  = std::chrono::milliseconds(2500);
  auto reader{ opentelemetry::sdk::metrics::PeriodicExportingMetricReaderFactory::Create(std::move(exporter), options) };
  auto stdoutReader{ opentelemetry::sdk::metrics::PeriodicExportingMetricReaderFactory::Create(std::move(stdoutExporter), options) };
  auto meterProvider{ opentelemetry::sdk::metrics::MeterProviderFactory::Create() };
  meterProvider->AddMetricReader(std::move(reader));
  meterProvider->AddMetricReader(std::move(stdoutReader));
  opentelemetry::metrics::Provider::SetMeterProvider(std::move(meterProvider));

  std::unique_ptr<opentelemetry::metrics::Counter<double>> counter;

  auto provider = opentelemetry::metrics::Provider::GetMeterProvider();
  auto meter{ provider->GetMeter("malkia_test_meter", "1.2.0") };
  counter = meter->CreateDoubleCounter("malkia_test_counter");

  srand(index);
  //std::this_thread::sleep_for(std::chrono::milliseconds(rand()%5000));
  bool flip{ false };
  int period{ 0 };
  std::this_thread::sleep_for(std::chrono::seconds(rand()%60));
  for(size_t i=0; i<2000; i++) 
  {
    flip = !flip;
    if( flip )
    {
      period = rand()%100;
    }
    flip = true;
    period = 16*6;
    counter->Add(1,{{"test", "one"}});
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(std::chrono::milliseconds(flip ? period : 100 - period));
  }

  if( const auto sp{ 
      std::dynamic_pointer_cast<opentelemetry::sdk::metrics::MeterProvider>( 
        opentelemetry::metrics::Provider::GetMeterProvider() ) }; sp )
  {
    if( const auto p{ sp.get() }; p )
    {
      p->ForceFlush();
      p->Shutdown();
    }
  }
  opentelemetry::metrics::Provider::SetMeterProvider({});
 
  return 0;
}
