/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/exporters/ostream/span_exporter.h"
#include "opentelemetry/ext/http/client/curl/http_client_curl.h"
#include "opentelemetry/ext/http/server/http_server.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/scope.h"

#include <algorithm>
#include "nlohmann/json.hpp"

namespace
{
static opentelemetry::trace::propagation::HttpTraceContext propagator_format;

class TextMapCarrierTest : public opentelemetry::context::propagation::TextMapCarrier
{
public:
  TextMapCarrierTest(std::map<std::string, std::string> &headers) : headers_(headers) {}
  virtual nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    auto it = headers_.find(std::string(key));
    if (it != headers_.end())
    {
      return nostd::string_view(it->second);
    }
    return "";
  }
  virtual void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    headers_[std::string(key)] = std::string(value);
  }

  std::map<std::string, std::string> &headers_;
};

void initTracer()
{
  auto exporter = std::unique_ptr<sdktrace::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);
  auto processor = std::unique_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  std::vector<std::unique_ptr<sdktrace::SpanProcessor>> processors;
  processors.push_back(std::move(processor));
  auto context  = std::make_shared<sdktrace::TracerContext>(std::move(processors));
  auto provider = nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
      new sdktrace::TracerProvider(context));
  // Set the global trace provider
  opentelemetry::trace::Provider::SetTracerProvider(provider);
}

nostd::shared_ptr<opentelemetry::trace::Tracer> get_tracer()
{
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
  return provider->GetTracer("w3c_tracecontext_test");
}

struct Uri
{
  std::string host;
  uint16_t port;
  std::string path;

  Uri(std::string uri)
  {
    size_t host_end = uri.substr(7, std::string::npos).find(":");
    size_t port_end = uri.substr(host_end + 1, std::string::npos).find("/");

    host = uri.substr(0, host_end + 7);
    port = std::stoi(uri.substr(7 + host_end + 1, port_end));
    path = uri.substr(host_end + port_end + 2, std::string::npos);
  }
};

// A noop event handler for making HTTP requests. We don't care about response bodies and error
// messages.
class NoopEventHandler : public opentelemetry::ext::http::client::EventHandler
{
public:
  void OnEvent(opentelemetry::ext::http::client::SessionState state,
               opentelemetry::nostd::string_view reason) noexcept override
  {}

  void OnConnecting(const opentelemetry::ext::http::client::SSLCertificate &) noexcept override {}

  void OnResponse(opentelemetry::ext::http::client::Response &response) noexcept override {}
};
}  // namespace

// Sends an HTTP POST request to the given url, with the given body.
void send_request(opentelemetry::ext::http::client::curl::HttpClient &client,
                  const std::string &url,
                  const std::string &body)
{
  static std::unique_ptr<opentelemetry::ext::http::client::EventHandler> handler(
      new NoopEventHandler());

  auto request_span = get_tracer()->StartSpan(__func__);
  opentelemetry::trace::Scope scope(request_span);

  Uri uri{url};

  auto session = client.CreateSession(uri.host, uri.port);
  auto request = session->CreateRequest();

  request->SetMethod(opentelemetry::ext::http::client::Method::Post);
  request->SetUri(uri.path);
  opentelemetry::ext::http::client::Body b = {body.c_str(), body.c_str() + body.size()};
  request->SetBody(b);
  request->AddHeader("Content-Type", "application/json");
  request->AddHeader("Content-Length", std::to_string(body.size()));

  std::map<std::string, std::string> headers;
  TextMapCarrierTest carrier(headers);
  propagator_format.Inject(carrier, opentelemetry::context::RuntimeContext::GetCurrent());

  for (auto const &hdr : headers)
  {
    request->AddHeader(hdr.first, hdr.second);
  }

  session->SendRequest(*handler);
  session->FinishSession();
}

// This application receives requests from the W3C test service. Each request has a JSON body which
// consists of an array of objects, each containing an URL to which to post to, and arguments which
// need to be used as body when posting to the given URL.
int main(int argc, char *argv[])
{
  initTracer();

  constexpr char default_host[]   = "localhost";
  constexpr uint16_t default_port = 30000;
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

  auto root_span = get_tracer()->StartSpan(__func__);
  opentelemetry::trace::Scope scope(root_span);

  testing::HttpServer server(default_host, port);
  opentelemetry::ext::http::client::curl::HttpClient client;

  testing::HttpRequestCallback test_cb{
      [&](testing::HttpRequest const &req, testing::HttpResponse &resp) {
        auto body = nlohmann::json::parse(req.content);

        std::cout << "Received request with body :\n" << req.content << "\n";

        for (auto &part : body)
        {
          const TextMapCarrierTest carrier((std::map<std::string, std::string> &)req.headers);
          auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
          auto ctx         = propagator_format.Extract(carrier, current_ctx);
          auto token       = opentelemetry::context::RuntimeContext::Attach(ctx);

          auto url       = part["url"].get<std::string>();
          auto arguments = part["arguments"].dump();

          std::cout << "  Sending request to " << url << "\n";

          send_request(client, url, arguments);
        }

        std::cout << "\n";

        resp.code = 200;
        return 0;
      }};

  server["/test"] = test_cb;

  // Start server
  server.start();

  std::cout << "Listening at http://" << default_host << ":" << port << "/test\n";

  // Wait for console input
  std::cin.get();

  // Stop server
  server.stop();
}
