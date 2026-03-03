// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

//
// Example: Cross-process context propagation via environment variables using fork().
//
// This demonstrates the EnvironmentCarrier for propagating trace context from a parent
// process to a child process through environment variables, as specified in:
// https://opentelemetry.io/docs/specs/otel/context/env-carriers/
//
// Parent process:
//   1. Creates a root span
//   2. Injects context into a map via EnvironmentCarrier
//   3. Forks a child process, which inherits the environment
//
// Child process:
//   1. Extracts context from environment via EnvironmentCarrier
//   2. Creates a child span under the propagated parent
//
// Both spans share the same trace_id, demonstrating cross-process trace continuity.
//

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#include "opentelemetry/context/context.h"
#include "opentelemetry/context/propagation/environment_carrier.h"
#include "opentelemetry/context/propagation/global_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/scope.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_startoptions.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/trace/tracer_provider.h"

namespace trace_api      = opentelemetry::trace;
namespace trace_sdk      = opentelemetry::sdk::trace;
namespace trace_exporter = opentelemetry::exporter::trace;
namespace propagation    = opentelemetry::context::propagation;

namespace
{

void InitTracer()
{
  auto exporter  = trace_exporter::OStreamSpanExporterFactory::Create();
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));

  auto provider = trace_sdk::TracerProviderFactory::Create(std::move(processor));
  trace_api::Provider::SetTracerProvider(
      std::shared_ptr<trace_api::TracerProvider>(std::move(provider)));

  // Set W3C TraceContext as the global propagator
  propagation::GlobalTextMapPropagator::SetGlobalPropagator(
      opentelemetry::nostd::shared_ptr<propagation::TextMapPropagator>(
          new trace_api::propagation::HttpTraceContext()));
}

void CleanupTracer()
{
  std::shared_ptr<trace_api::TracerProvider> noop;
  trace_api::Provider::SetTracerProvider(noop);
}

template <typename T>
std::string Hex(const T &id_item)
{
  char buf[T::kSize * 2];
  id_item.ToLowerBase16(buf);
  return std::string(buf, sizeof(buf));
}

void RunChild()
{
  InitTracer();

  auto tracer = trace_api::Provider::GetTracerProvider()->GetTracer("child-service", "1.0.0");

  // Extract parent context from environment variables
  propagation::EnvironmentCarrier carrier;
  auto propagator = propagation::GlobalTextMapPropagator::GetGlobalPropagator();
  opentelemetry::context::Context ctx;
  auto parent_ctx = propagator->Extract(carrier, ctx);

  // Create child span under the extracted parent context
  trace_api::StartSpanOptions opts;
  opts.parent     = parent_ctx;
  auto child_span = tracer->StartSpan("child-operation", opts);

  std::cout << "[child]  trace_id: " << Hex(child_span->GetContext().trace_id()) << std::endl;
  std::cout << "[child]  span_id:  " << Hex(child_span->GetContext().span_id()) << std::endl;

  child_span->End();
  CleanupTracer();
}

void RunParent()
{
  InitTracer();

  auto tracer = trace_api::Provider::GetTracerProvider()->GetTracer("parent-service", "1.0.0");

  // Start root span
  auto root_span = tracer->StartSpan("parent-operation");
  auto scope     = trace_api::Scope(root_span);

  std::cout << "[parent] trace_id: " << Hex(root_span->GetContext().trace_id()) << std::endl;
  std::cout << "[parent] span_id:  " << Hex(root_span->GetContext().span_id()) << std::endl;

  // Inject context into a map via EnvironmentCarrier
  auto env_map = std::make_shared<std::map<std::string, std::string>>();
  propagation::EnvironmentCarrier carrier(env_map);
  auto propagator = propagation::GlobalTextMapPropagator::GetGlobalPropagator();
  propagator->Inject(carrier, opentelemetry::context::RuntimeContext::GetCurrent());

  // Set injected values as environment variables before forking
  for (const auto &entry : *env_map)
  {
    ::setenv(entry.first.c_str(), entry.second.c_str(), 1);
  }

  // Fork child process
  pid_t pid = fork();
  if (pid == 0)
  {
    // Child process
    RunChild();
    _exit(0);
  }
  else if (pid > 0)
  {
    // Parent process - wait for child
    int status = 0;
    waitpid(pid, &status, 0);
  }
  else
  {
    std::cerr << "fork() failed" << std::endl;
  }

  root_span->End();
  CleanupTracer();
}

}  // namespace

int main()
{
  RunParent();
  return 0;
}
