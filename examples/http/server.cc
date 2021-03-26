#include "server.hpp"
#include "tracer_common.hpp"

#include <iostream>
#include <thread>

namespace
{
class RequestHandler : public HTTP_SERVER_NS::HttpRequestCallback
{
public:
  virtual int onHttpRequest(HTTP_SERVER_NS::HttpRequest const &request,
                            HTTP_SERVER_NS::HttpResponse &response) override
  {
    opentelemetry::trace::StartSpanOptions options;
    options.kind = opentelemetry::trace::SpanKind::kServer;  // server

    auto span =
        get_tracer("http-server")
            ->StartSpan(
                "handle_request",
                {{"req_url", request.uri}, {"req_method", request.method}, {"component", "http"}},
                options);
    auto scope = get_tracer("http_server")->WithActiveSpan(span);
    for (auto &kv : request.headers)
    {
      span->SetAttribute("http.req.header" + std::string(kv.first.data()), kv.second);
    }
    if (request.uri == "/helloworld")
    {
      span->AddEvent("Processing request");
      response.headers["Content-Type"] = "text/plain";
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
  constexpr char default_host[]   = "localhost";
  constexpr uint16_t default_port = 8800;
  uint16_t port;

  // The port the validation service listens to can be specified via the command line.
  if (argc > 1)
  {
    port = atoi(argv[1]);
  }
  else
  {
    port = default_port;
  }

  HttpServer http_server(default_host, port);
  RequestHandler req_handler;
  http_server.AddHandler("/helloworld", &req_handler);
  http_server.Start();
  std::cout << "Server is running..Press ctrl-c to exit...\n";
  while (1)
  {
    std::this_thread::sleep_for(std::chrono::seconds(100));
  }

  http_server.Stop();
  return 0;
}
