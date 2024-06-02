// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/ostream/log_record_exporter.h"
#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/logger_provider_factory.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

#ifdef BAZEL_BUILD
#  include "examples/common/logs_foo_library/foo_library.h"
#else
#  include "logs_foo_library/foo_library.h"
#endif

namespace logs_api      = opentelemetry::logs;
namespace logs_sdk      = opentelemetry::sdk::logs;
namespace logs_exporter = opentelemetry::exporter::logs;

namespace trace_api      = opentelemetry::trace;
namespace trace_sdk      = opentelemetry::sdk::trace;
namespace trace_exporter = opentelemetry::exporter::trace;

namespace
{
void InitTracer()
{
  // Create ostream span exporter instance
  auto exporter  = trace_exporter::OStreamSpanExporterFactory::Create();
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));

#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY
  std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(processor));
#else
  std::shared_ptr<opentelemetry::sdk::trace::TracerProvider> provider =
      opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(processor));
#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */

  // Set the global trace provider
  std::shared_ptr<trace_api::TracerProvider> api_provider = provider;
  trace_api::Provider::SetTracerProvider(api_provider);
}

void CleanupTracer()
{
  std::shared_ptr<trace_api::TracerProvider> none;
  trace_api::Provider::SetTracerProvider(none);
}

void InitLogger()
{
  // Create ostream log exporter instance
  auto exporter =
      std::unique_ptr<logs_sdk::LogRecordExporter>(new logs_exporter::OStreamLogRecordExporter);
  auto processor = logs_sdk::SimpleLogRecordProcessorFactory::Create(std::move(exporter));

#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY
  std::shared_ptr<opentelemetry::logs::LoggerProvider> provider(
      opentelemetry::sdk::logs::LoggerProviderFactory::Create(std::move(processor)));
#else
  std::shared_ptr<opentelemetry::sdk::logs::LoggerProvider> provider(
      opentelemetry::sdk::logs::LoggerProviderFactory::Create(std::move(processor)));
#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */

  // Set the global logger provider
  std::shared_ptr<logs_api::LoggerProvider> api_provider = provider;
  logs_api::Provider::SetLoggerProvider(api_provider);
}

void CleanupLogger()
{
  std::shared_ptr<logs_api::LoggerProvider> none;
  logs_api::Provider::SetLoggerProvider(none);
}

}  // namespace

int main()
{
  InitTracer();
  InitLogger();
  foo_library();
  CleanupTracer();
  CleanupLogger();
}
