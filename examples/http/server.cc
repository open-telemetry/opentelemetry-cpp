#include "server.h"
#include "tracer_common.h"

#include <iostream>
#include <thread>

namespace
{
uint16_t server_port         = 8800;
constexpr char server_name[] = "localhost";

class RequestHandler : public HTTP_SERVER_NS::HttpRequestCallback
{
public:
  virtual int onHttpRequest(HTTP_SERVER_NS::HttpRequest const &request,
                            HTTP_SERVER_NS::HttpResponse &response) override
  {
    opentelemetry::trace::StartSpanOptions options;
    options.kind          = opentelemetry::trace::SpanKind::kServer;  // server
    std::string span_name = request.uri;

    // extract context from http header
    const HttpTextMapCarrier<std::map<std::string, std::string>> carrier(
        (std::map<std::string, std::string> &)request.headers);
    auto prop = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
    auto new_context = prop->Extract(carrier, current_ctx);
    options.parent   = opentelemetry::trace::propagation::GetSpan(new_context)->GetContext();

    // start span with parent context extracted from http header
    auto span = get_tracer("http-server")
                    ->StartSpan(span_name,
                                {{"http.server_name", server_name},
                                 {"net.host.port", server_port},
                                 {"http.method", request.method},
                                 {"http.scheme", "http"},
                                 {"http.request_content_length", request.content.length()},
                                 {"http.client_ip", request.client}},
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
  initTracer();

  // The port the validation service listens to can be specified via the command line.
  if (argc > 1)
  {
    server_port = atoi(argv[1]);
  }

  HttpServer http_server(server_name, server_port);
  RequestHandler req_handler;
  http_server.AddHandler("/helloworld", &req_handler);
  auto root_span = get_tracer("http_server")->StartSpan(__func__);
  opentelemetry::trace::Scope scope(root_span);
  http_server.Start();
  std::cout << "Server is running..Press ctrl-c to exit...\n";
  while (1)
  {
    std::this_thread::sleep_for(std::chrono::seconds(100));
  }
  http_server.Stop();
  root_span->End();
  return 0;
}
