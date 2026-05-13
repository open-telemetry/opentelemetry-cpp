// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// This example shows how to use TracerProvider::UpdateTracerConfigurator to control which
// tracers are enabled at runtime. Updating the TracerConfigurator affects all existing and future
// tracers provided by the TracerProvider. It is thread safe and can be called concurrently with
// tracer and span creation.
//
// Two instrumentation scopes are shown:
// 1. "my_library" (example instrumented user code),
// 2. "external_library_foo" (example instrumented third-party dependency).
//
// The example simulates a debugging workflow where only the tracer configuration is changed at
// runtime through the provider:
//
//   Stage 1 – Start with all tracing disabled (low-overhead production default).
//   Stage 2 – An issue is reported. Enable only the user library traces to get an initial signal.
//   Stage 3 – The issue involves external libraries too. Enable all traces for full visibility.
//   Stage 4 – Investigation complete. Disable all tracing again.

#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/random_id_generator.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_config.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/version.h"

namespace trace_sdk      = opentelemetry::sdk::trace;
namespace trace_exporter = opentelemetry::exporter::trace;
namespace scope_cfg      = opentelemetry::sdk::instrumentationscope;
namespace nostd          = opentelemetry::nostd;

namespace
{
void DoWork(opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> &tracer,
            nostd::string_view tracer_name,
            nostd::string_view operation)
{
  auto span = tracer->StartSpan(operation);
  std::cout << (span->IsRecording() ? "[active] " : "[off]    ") << tracer_name << " / "
            << operation << "\n";
  span->End();
}

// Builds a ScopeConfigurator that enables all tracers (the default).
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
}  // namespace

int main()
{
  auto exporter  = trace_exporter::OStreamSpanExporterFactory::Create();
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));

  // Start with all tracing disabled
  auto provider = std::make_shared<trace_sdk::TracerProvider>(
      std::move(processor), opentelemetry::sdk::resource::Resource::Create({}),
      std::make_unique<trace_sdk::AlwaysOnSampler>(),
      std::make_unique<trace_sdk::RandomIdGenerator>(), DisableAll());

  auto my_library_tracer           = provider->GetTracer("my_library");
  auto external_library_foo_tracer = provider->GetTracer("external_library_foo");

  // -------------------------------------------------------------------------
  // Stage 1: all tracing disabled
  // -------------------------------------------------------------------------
  std::cout << "=== Stage 1: disable all (production default) ===\n";

  DoWork(my_library_tracer, "my_library", "DoWork");                            // disabled
  DoWork(external_library_foo_tracer, "external_library_foo", "FooOperation");  // disabled

  // -------------------------------------------------------------------------
  // Stage 2: enable only user library tracing
  //
  // EnableOnly() sets Disabled() as the default and adds explicit per-name
  // overrides. Existing tracer handles reflect the change immediately
  // -------------------------------------------------------------------------
  std::cout << "\n=== Stage 2: enable only 'my_library' ===\n";

  provider->UpdateTracerConfigurator(EnableOnly({"my_library"}));

  DoWork(my_library_tracer, "my_library", "DoWork");                            // enabled
  DoWork(external_library_foo_tracer, "external_library_foo", "FooOperation");  // disabled

  // -------------------------------------------------------------------------
  // Stage 3: enable tracing in all libraries
  // -------------------------------------------------------------------------
  std::cout << "\n=== Stage 3: enable all ===\n";

  provider->UpdateTracerConfigurator(EnableAll());

  DoWork(my_library_tracer, "my_library", "DoWork");                            // enabled
  DoWork(external_library_foo_tracer, "external_library_foo", "FooOperation");  // enabled

  // -------------------------------------------------------------------------
  // Stage 4: disable all tracing again
  // -------------------------------------------------------------------------
  std::cout << "\n=== Stage 4: disable all ===\n";

  provider->UpdateTracerConfigurator(DisableAll());

  DoWork(my_library_tracer, "my_library", "DoWork");                            // disabled
  DoWork(external_library_foo_tracer, "external_library_foo", "FooOperation");  // disabled

  provider->ForceFlush();
  provider->Shutdown();
  return 0;
}
