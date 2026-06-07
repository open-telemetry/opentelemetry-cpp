// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// This example shows how to use TracerProvider::UpdateTracerConfigurator to control which
// tracers are enabled at runtime. Updating the TracerConfigurator affects all existing and future
// tracers provided by the TracerProvider. It is thread safe and can be called concurrently with
// tracer and span creation.
//
// Three instrumentation scopes are shown:
// 1. "my_application" (example instrumented user application code),
// 2. "my_library" (example instrumented user library code),
// 3. "external_library" (example instrumented third-party dependency).
//
// The example simulates a debugging workflow where only the tracer configuration is changed at
// runtime through the provider:
//
// Stage 1: Start with all tracing disabled (low-overhead production default).
// Stage 2: An issue is reported. Enable only the user application and library tracers.
// Stage 3: The issue involves external libraries too. Enable all tracers for full visibility.
// Stage 4: Investigation complete. Disable all tracing again.

#include <chrono>
#include <initializer_list>
#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/random_id_generator.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_config.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/semconv/service_attributes.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/scope.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/trace/tracer_provider.h"

namespace trace_sdk      = opentelemetry::sdk::trace;
namespace trace_exporter = opentelemetry::exporter::trace;
namespace scope_cfg      = opentelemetry::sdk::instrumentationscope;
namespace nostd          = opentelemetry::nostd;

namespace
{

// Example external library
namespace external_library
{
class ExternalModule
{
public:
  ExternalModule()
      : tracer_(opentelemetry::trace::Provider::GetTracerProvider()->GetTracer("external_library"))
  {}

  // Example operation in the external library that always returns false to simulate an error
  bool Execute()
  {
    std::cout << "[external_library] Execute\n";

    auto span = tracer_->StartSpan("ExternalModule.Execute");
    opentelemetry::trace::Scope scope(span);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    span->SetStatus(opentelemetry::trace::StatusCode::kError,
                    "Something went wrong in external_library");
    return false;
  }

private:
  nostd::shared_ptr<opentelemetry::trace::Tracer> tracer_;
};
}  // namespace external_library

// Example user library
namespace my_library
{
class MyModule
{
public:
  MyModule() : tracer_(opentelemetry::trace::Provider::GetTracerProvider()->GetTracer("my_library"))
  {}

  // Example operation in the user library that calls into the external library
  bool Execute()
  {
    std::cout << "[my_library] Execute\n";

    auto span = tracer_->StartSpan("MyModule.Execute");
    opentelemetry::trace::Scope scope(span);

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    const bool result = external_module_.Execute();

    span->SetStatus(result ? opentelemetry::trace::StatusCode::kOk
                           : opentelemetry::trace::StatusCode::kError);

    return result;
  }

private:
  nostd::shared_ptr<opentelemetry::trace::Tracer> tracer_;
  external_library::ExternalModule external_module_;
};
}  // namespace my_library

// Example user application that uses the user library
class MyApplication
{
public:
  MyApplication()
      : tracer_(opentelemetry::trace::Provider::GetTracerProvider()->GetTracer("my_application"))
  {}

  // Example operation in the user application that calls into the user library
  void Execute()
  {
    std::cout << "[my_application] Execute\n";
    auto span = tracer_->StartSpan("MyApplication.Execute");
    opentelemetry::trace::Scope scope(span);

    const bool result = my_module_.Execute();
    span->SetStatus(result ? opentelemetry::trace::StatusCode::kOk
                           : opentelemetry::trace::StatusCode::kError);
  }

private:
  nostd::shared_ptr<opentelemetry::trace::Tracer> tracer_;
  my_library::MyModule my_module_;
};

// Builds a ScopeConfigurator that enables all tracers.
std::unique_ptr<scope_cfg::ScopeConfigurator<trace_sdk::TracerConfig>> EnableAll()
{
  return std::make_unique<scope_cfg::ScopeConfigurator<trace_sdk::TracerConfig>>(
      scope_cfg::ScopeConfigurator<trace_sdk::TracerConfig>::Builder(
          trace_sdk::TracerConfig::Default())
          .Build());
}

// Builds a ScopeConfigurator that enables only the named tracers; all others are disabled.
std::unique_ptr<scope_cfg::ScopeConfigurator<trace_sdk::TracerConfig>> EnableOnly(
    std::initializer_list<nostd::string_view> names)
{
  scope_cfg::ScopeConfigurator<trace_sdk::TracerConfig>::Builder builder(
      trace_sdk::TracerConfig::Disabled());
  for (nostd::string_view name : names)
  {
    builder.AddConditionNameEquals(name, trace_sdk::TracerConfig::Default());
  }
  return std::make_unique<scope_cfg::ScopeConfigurator<trace_sdk::TracerConfig>>(builder.Build());
}

// Builds a ScopeConfigurator that disables all tracers.
std::unique_ptr<scope_cfg::ScopeConfigurator<trace_sdk::TracerConfig>> DisableAll()
{
  return std::make_unique<scope_cfg::ScopeConfigurator<trace_sdk::TracerConfig>>(
      scope_cfg::ScopeConfigurator<trace_sdk::TracerConfig>::Builder(
          trace_sdk::TracerConfig::Disabled())
          .Build());
}

// Creates a TracerProvider with an OStreamSpanExporter and disables all tracers by default.
std::shared_ptr<trace_sdk::TracerProvider> CreateTracerProvider()
{
  auto exporter  = trace_exporter::OStreamSpanExporterFactory::Create();
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));

  auto provider = std::make_shared<trace_sdk::TracerProvider>(
      std::move(processor),
      opentelemetry::sdk::resource::Resource::Create(
          {{opentelemetry::semconv::service::kServiceName, "tracer_configurator_example"}}),
      std::make_unique<trace_sdk::AlwaysOnSampler>(),
      std::make_unique<trace_sdk::RandomIdGenerator>(), DisableAll());
  return provider;
}

}  // namespace

int main()
{
  // Create the SDK TracerProvider with the DisableAll configurator
  auto sdk_tracer_provider = CreateTracerProvider();

  // Set the global provider to our SDK TracerProvider instance.
  opentelemetry::trace::Provider::SetTracerProvider(
      nostd::shared_ptr<opentelemetry::trace::TracerProvider>(sdk_tracer_provider));

  // Create an instance of the user application.
  MyApplication my_app;

  // -------------------------------------------------------------------------
  // Stage 1: all tracing initially disabled
  // -------------------------------------------------------------------------
  std::cout << "=== Stage 1: all tracers initially disabled ===\n";

  my_app.Execute();  // all tracers disabled

  // -------------------------------------------------------------------------
  // Stage 2: enable only user application and library tracing
  //
  // EnableOnly() configurator will disable all but the named tracer scopes
  // -------------------------------------------------------------------------
  std::cout << "\n=== Stage 2: enable only 'my_application' and 'my_library' tracers ===\n";

  sdk_tracer_provider->UpdateTracerConfigurator(EnableOnly({"my_application", "my_library"}));

  my_app.Execute();  // my_application and my_library tracers enabled, external_library tracer
                     // disabled

  // -------------------------------------------------------------------------
  // Stage 3: EnableAll() configurator will enable tracing in all libraries
  // -------------------------------------------------------------------------
  std::cout << "\n=== Stage 3: enable all tracers ===\n";

  sdk_tracer_provider->UpdateTracerConfigurator(EnableAll());

  my_app.Execute();  // all tracers enabled

  // -------------------------------------------------------------------------
  // Stage 4: DisableAll() configurator will disable all tracers again
  // -------------------------------------------------------------------------
  std::cout << "\n=== Stage 4: disable all tracers ===\n";

  sdk_tracer_provider->UpdateTracerConfigurator(DisableAll());

  my_app.Execute();  // all tracers disabled

  sdk_tracer_provider->ForceFlush();
  sdk_tracer_provider->Shutdown();
  return 0;
}
