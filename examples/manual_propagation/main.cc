// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <functional>
#include <future>
#include <map>
#include <string>
#include <utility>

#include "opentelemetry/context/context.h"
#include "opentelemetry/context/propagation/global_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/context/runtime_context.h"
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
#include "opentelemetry/trace/context.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
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
namespace ctx       = opentelemetry::context;

using HeaderMap = std::map<std::string, std::string>;

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

  ctx::propagation::GlobalTextMapPropagator::SetGlobalPropagator(
      nostd::shared_ptr<ctx::propagation::TextMapPropagator>(
          new trace_api::propagation::HttpTraceContext()));
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
// In this example, the `SpanKind` and `name` are also passed from outside the
// function.

nostd::shared_ptr<trace_api::Span> CreateSpan(
    const std::string &name,
    trace_api::SpanKind kind,
    const nostd::shared_ptr<trace_api::Span> &parent = nostd::shared_ptr<trace_api::Span>{})
{
  trace_api::StartSpanOptions opts;
  opts.kind = kind;
  if (parent)
  {
    opts.parent = parent->GetContext();
  }

  auto span = get_tracer()->StartSpan(name, opts);
  return span;
}

class HttpTextMapCarrier : public ctx::propagation::TextMapCarrier
{
public:
  HttpTextMapCarrier(HeaderMap &headers) : headers_(headers) {}

  nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    auto it = headers_.get().find(std::string(key.data(), key.size()));
    if (it != headers_.get().end())
    {
      return it->second;
    }
    return "";
  }

  void Set(nostd::string_view k, nostd::string_view v) noexcept override
  {
    headers_.get()[std::string(k.data(), k.size())] = std::string(v.data(), v.size());
  }

  std::reference_wrapper<HeaderMap> headers_;
};

// An auxiliary function that injects the tracing information to a given carrier
// structure. In this example a simple implementation of the `TextMapCarrier` as
// an `std::map<std::string, std::string>` was used.
//
// It's important to note that the `trace_api::SetSpan(current_ctx, span)`
// function is needed for the injection to explicitly use the desired span
// context.

void InjectTraceContext(const nostd::shared_ptr<trace_api::Span> &span, HeaderMap &headers)
{
  if (!span)
  {
    return;
  }

  // First, we set the Span into the context explicitly
  auto current_ctx   = ctx::RuntimeContext::GetCurrent();
  auto ctx_with_span = trace_api::SetSpan(current_ctx, span);

  // Then we inject the span info into the headers
  HttpTextMapCarrier carrier(headers);
  ctx::propagation::GlobalTextMapPropagator::GetGlobalPropagator()->Inject(carrier, ctx_with_span);
}

// An auxiliary function that returns a child span from incoming headers that
// contain tracing information. In this example, the `SpanKind` and `name` are
// also passed from outside the function.

nostd::shared_ptr<trace_api::Span> CreateChildSpanFromRemote(HeaderMap &headers,
                                                             const std::string &name,
                                                             trace_api::SpanKind kind)
{
  HttpTextMapCarrier carrier(headers);
  auto current_ctx = ctx::RuntimeContext::GetCurrent();
  auto new_context = ctx::propagation::GlobalTextMapPropagator::GetGlobalPropagator()->Extract(
      carrier, current_ctx);
  auto remote_span = opentelemetry::trace::GetSpan(new_context);

  return CreateSpan(name, kind, remote_span);
}

}  // namespace utils

namespace client
{

void ProcessAnswer(HeaderMap &remote_headers)
{
  // The only context we have here is the headers
  auto remote = utils::CreateChildSpanFromRemote(remote_headers, "process_answer",
                                                 trace_api::SpanKind::kClient);
  remote->AddEvent("Answer processed");
  remote->SetStatus(trace_api::StatusCode::kOk);
  // this span is automatically ended on exit
}

}  // namespace client

namespace server
{

HeaderMap ProcessRequest(HeaderMap &remote_headers)
{
  // Extract remote context and create server-side child span
  auto server_span =
      utils::CreateChildSpanFromRemote(remote_headers, "server", trace_api::SpanKind::kServer);

  // Simulating work with nested spans
  server_span->AddEvent("Processing in server");
  auto nested = utils::CreateSpan("nested", trace_api::SpanKind::kServer, server_span);
  nested->AddEvent("Nested did some work");
  nested->SetStatus(trace_api::StatusCode::kOk);
  nested->End();

  // Filling answer headers
  HeaderMap answer_headers;
  utils::InjectTraceContext(server_span, answer_headers);

  // server_span is automatically ended on exit
  server_span->AddEvent("Replying answer");
  server_span->SetStatus(trace_api::StatusCode::kOk);
  return answer_headers;
}

}  // namespace server
}  // namespace

int main(int /* argc */, char ** /* argv */)
{
  InitTracer();

  // The main client thread is in charge of sending requests and
  // processing when they are answered.
  auto r1 = utils::CreateSpan("request1", trace_api::SpanKind::kClient);
  HeaderMap h1;
  utils::InjectTraceContext(r1, h1);

  // The simulated "server" will handle the request asynchronously
  auto server_future_1 =
      std::async(std::launch::async, [&h1] { return server::ProcessRequest(h1); });

  // The client thread is now free to send another request
  auto r2 = utils::CreateSpan("request2", trace_api::SpanKind::kClient);
  HeaderMap h2;
  utils::InjectTraceContext(r2, h2);

  // The simulated "server" will handle the request asynchronously
  auto server_future_2 =
      std::async(std::launch::async, [&h2] { return server::ProcessRequest(h2); });

  // Order doesn't matter. Let's simulate that we get the second answer before the first one
  HeaderMap answer_headers_2 = server_future_2.get();
  client::ProcessAnswer(answer_headers_2);

  HeaderMap answer_headers_1 = server_future_1.get();
  client::ProcessAnswer(answer_headers_1);

  // Both root spans are kept alive till the end, to prove they can both
  // outlive other parallel and async processing without interfering each other
  // when we explicitly set parent spans with the aux functions
  r1->SetStatus(trace_api::StatusCode::kOk);
  r1->End();

  r2->SetStatus(trace_api::StatusCode::kOk);
  r2->End();

  CleanupTracer();
  return 0;
}
