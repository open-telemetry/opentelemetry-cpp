// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "server.h"
#include "opentelemetry/trace/context.h"
#include "opentelemetry/trace/semantic_conventions.h"
#include "tracer_common.h"

#include <iostream>
#include <thread>

namespace
{

using namespace opentelemetry::trace;
namespace context = opentelemetry::context;

uint16_t server_port              = 8800;
constexpr const char *server_name = "localhost";

class RequestHandler : public HTTP_SERVER_NS::HttpRequestCallback
{
public:
  virtual int onHttpRequest(HTTP_SERVER_NS::HttpRequest const &request,
                            HTTP_SERVER_NS::HttpResponse &response) override
  {
    StartSpanOptions options;
    options.kind          = SpanKind::kServer;  // server
    std::string span_name = request.uri;

    // extract context from http header
    std::map<std::string, std::string> &request_headers =
        const_cast<std::map<std::string, std::string> &>(request.headers);
    const HttpTextMapCarrier<std::map<std::string, std::string>> carrier(request_headers);
    auto prop        = context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    auto current_ctx = context::RuntimeContext::GetCurrent();
    auto new_context = prop->Extract(carrier, current_ctx);
    options.parent   = GetSpan(new_context)->GetContext();

    // start span with parent context extracted from http header
    auto span = get_tracer("http-server")
                    ->StartSpan(span_name,
                                {{SemanticConventions::kServerAddress, server_name},
                                 {SemanticConventions::kServerPort, server_port},
                                 {SemanticConventions::kHttpRequestMethod, request.method},
                                 {SemanticConventions::kUrlScheme, "http"},
                                 {SemanticConventions::kHttpRequestBodySize,
                                  static_cast<uint64_t>(request.content.length())},
                                 {SemanticConventions::kClientAddress, request.client}},
                                options);

    auto scope = get_tracer("http_server")->WithActiveSpan(span);

    for (auto &kv : request.headers)
    {
      span->SetAttribute("http.header." + std::string(kv.first.data()), kv.second);
    }
    if (request.uri == "/helloworld")
    {
      span->AddEvent("Processing request");
      response.headers[HTTP_SERVER_NS::CONTENT_TYPE] = HTTP_SERVER_NS::CONTENT_TYPE_TEXT;
      span->End();
      return 200;
    }
    span->End();
    return 404;
  }
};
}  // namespace

int main(int argc, char *argv[])
{
  InitTracer();

  // The port the validation service listens to can be specified via the command line.
  if (argc > 1)
  {
    server_port = (uint16_t)atoi(argv[1]);
  }

  HttpServer http_server(server_name, server_port);
  RequestHandler req_handler;
  http_server.AddHandler("/helloworld", &req_handler);
  auto root_span = get_tracer("http_server")->StartSpan(__func__);
  Scope scope(root_span);
  http_server.Start();
  std::cout << "Server is running..Press ctrl-c to exit...\n";
  while (1)
  {
    std::this_thread::sleep_for(std::chrono::seconds(100));
  }
  http_server.Stop();
  root_span->End();
  CleanupTracer();
  return 0;
}
