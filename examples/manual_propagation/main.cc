// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <future>
#include <string>
#include <utility>

#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/provider.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/span_startoptions.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/trace/tracer_provider.h"

namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace nostd     = opentelemetry::nostd;

namespace
{
void InitTracer()
{
  auto exporter  = opentelemetry::exporter::trace::OStreamSpanExporterFactory::Create();
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
  std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      trace_sdk::TracerProviderFactory::Create(std::move(processor),
                                               opentelemetry::sdk::resource::Resource::Create({}));
  // Set the global tracer provider
  trace_sdk::Provider::SetTracerProvider(provider);
}

void CleanupTracer()
{
  std::shared_ptr<opentelemetry::trace::TracerProvider> none;
  trace_sdk::Provider::SetTracerProvider(none);
}

nostd::shared_ptr<trace_api::Tracer> get_tracer()
{
  auto provider = trace_api::Provider::GetTracerProvider();
  return provider->GetTracer("foo_library");
}

namespace utils
{

// An auxiliary function that returns a child span for a desired arbitrary
// parent (when passed). Otherwise, defaults to root span creation.
// In this example, `name` is also passed from outside the function.

nostd::shared_ptr<trace_api::Span> CreateSpan(
    const std::string &name,
    const nostd::shared_ptr<trace_api::Span> &parent = nostd::shared_ptr<trace_api::Span>{})
{
  trace_api::StartSpanOptions opts;
  opts.kind = trace_api::SpanKind::kInternal;
  if (parent)
  {
    opts.parent = parent->GetContext();
  }

  auto span = get_tracer()->StartSpan(name, opts);
  return span;
}

}  // namespace utils

namespace app
{

void ProcessTask(nostd::shared_ptr<trace_api::Span> &producer_span)
{
  // Simulating work with nested spans
  auto nested = utils::CreateSpan("nested", producer_span);
  nested->SetStatus(trace_api::StatusCode::kOk);
  nested->End();
  return;
}

}  // namespace app
}  // namespace

int main(int /* argc */, char ** /* argv */)
{
  InitTracer();

  // In this example, we simulate handling of 2 independent traces that are alive at the same time.
  // This may happen for various reasons. For example, 2 different tasks
  // are run in parallel within the same process, and we may want to nest
  // spans to each of them, but cannot know which one will be active at runtime.
  auto parent_1 = utils::CreateSpan("parent_1");
  auto parent_2 = utils::CreateSpan("parent_2");

  // To make sure that both the wanted parent-child relationship is met and
  // the parent is kept alive, the parent span is passed.
  auto task_future_1 =
      std::async(std::launch::async, [&parent_1] { return app::ProcessTask(parent_1); });

  auto task_future_2 =
      std::async(std::launch::async, [&parent_2] { return app::ProcessTask(parent_2); });

  // Order doesn't matter. Let's simulate that we get the second answer before the first one
  task_future_2.get();
  task_future_1.get();

  // Both root spans are kept alive till the end, to prove they can both
  // outlive other parallel and async processing without interfering each other
  // when we explicitly set parent spans with the aux functions
  parent_1->SetStatus(trace_api::StatusCode::kOk);
  parent_1->End();

  parent_2->SetStatus(trace_api::StatusCode::kOk);
  parent_2->End();

  CleanupTracer();
  return 0;
}
