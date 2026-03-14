// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <future>
#include <map>
#include <string>
#include <utility>

#include "opentelemetry/context/propagation/global_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
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
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/trace/tracer_provider.h"

namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace nostd     = opentelemetry::nostd;
namespace ctx       = opentelemetry::context;

using header_map = std::map<std::string, std::string>;

namespace
{
void InitTracer()
{
  auto exporter  = opentelemetry::exporter::trace::OStreamSpanExporterFactory::Create();
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
  std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      trace_sdk::TracerProviderFactory::Create(std::move(processor),
                                               opentelemetry::sdk::resource::Resource::Create({}));
  // Set the global trace provider
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
}  // namespace

namespace utils
{

nostd::shared_ptr<trace_api::Span> create_span(const std::string &name, trace_api::SpanKind kind)
{
  trace_api::StartSpanOptions opts;
  opts.kind = kind;

  auto span = get_tracer()->StartSpan(name, opts);
  return span;
}

nostd::shared_ptr<trace_api::Span> create_child_span(
    const std::string &name,
    const nostd::shared_ptr<trace_api::Span> &parent,
    trace_api::SpanKind kind)
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
  HttpTextMapCarrier(header_map &headers) : headers_(headers) {}

  nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    auto it = headers_.find(std::string(key.data(), key.size()));
    if (it != headers_.end())
    {
      return it->second;
    }
    return "";
  }

  void Set(nostd::string_view k, nostd::string_view v) noexcept override
  {
    headers_.emplace(std::string(k.data(), k.size()), std::string(v.data(), v.size()));
  }

  header_map &headers_;
};

void inject_trace_context(const nostd::shared_ptr<trace_api::Span> &span, header_map &headers)
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

nostd::shared_ptr<trace_api::Span> create_child_span_from_remote(header_map &headers,
                                                                 const std::string &name,
                                                                 const trace_api::SpanKind kind)
{
  HttpTextMapCarrier carrier(headers);
  auto current_ctx = ctx::RuntimeContext::GetCurrent();
  auto new_context = ctx::propagation::GlobalTextMapPropagator::GetGlobalPropagator()->Extract(
      carrier, current_ctx);
  auto remote_span = opentelemetry::trace::GetSpan(new_context);

  return create_child_span(name, remote_span, kind);
}

}  // namespace utils

namespace client
{

void process_answer(header_map &remote_headers)
{
  // The only context we have here is the headers
  auto remote = utils::create_child_span_from_remote(remote_headers, "process_answer",
                                                     trace_api::SpanKind::kClient);
  remote->AddEvent("Answer processed");
  remote->SetStatus(trace_api::StatusCode::kOk);
  // this span is automatically ended on exit
}

}  // namespace client

namespace server
{

header_map process_request(header_map &remote_headers)
{
  // Extract remote context and create server-side child span
  auto server_span =
      utils::create_child_span_from_remote(remote_headers, "server", trace_api::SpanKind::kServer);

  // Simulating work with nested spans
  server_span->AddEvent("Processing in server");
  auto nested = utils::create_child_span("nested", server_span, trace_api::SpanKind::kServer);
  nested->AddEvent("Nested did some work");
  nested->SetStatus(trace_api::StatusCode::kOk);
  nested->End();

  // Filling answer headers
  header_map answer_headers;
  utils::inject_trace_context(server_span, answer_headers);

  // server_span is automatically ended on exit
  server_span->AddEvent("Replying answer");
  server_span->SetStatus(trace_api::StatusCode::kOk);
  return answer_headers;
}

}  // namespace server

int main(int /* argc */, char ** /* argv */)
{
  InitTracer();

  // The main client thread is in charge of sending requests and
  // processing when they are answered.
  auto r1 = utils::create_span("request1", trace_api::SpanKind::kClient);
  header_map h1;
  utils::inject_trace_context(r1, h1);

  // The simulated "server" will handle the request asynchronously
  auto server_future_1 =
      std::async(std::launch::async, [&h1] { return server::process_request(h1); });

  // The client thread is now free to send another request
  auto r2 = utils::create_span("request2", trace_api::SpanKind::kClient);
  header_map h2;
  utils::inject_trace_context(r2, h2);

  // The simulated "server" will handle the request asynchronously
  auto server_future_2 =
      std::async(std::launch::async, [&h2] { return server::process_request(h2); });

  // Order doesn't matter. Let's simulate that we get the second answer before the first one
  header_map answer_headers_2 = server_future_2.get();
  client::process_answer(answer_headers_2);

  header_map answer_headers_1 = server_future_1.get();
  client::process_answer(answer_headers_1);

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
