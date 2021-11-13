// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

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

namespace trace_api   = opentelemetry::trace;
namespace http_client = opentelemetry::ext::http::client;
namespace curl        = opentelemetry::ext::http::client::curl;
namespace context     = opentelemetry::context;
namespace nostd       = opentelemetry::nostd;
namespace trace_sdk   = opentelemetry::sdk::trace;

namespace
{
static trace_api::propagation::HttpTraceContext propagator_format;

class TextMapCarrierTest : public context::propagation::TextMapCarrier
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
  auto exporter = std::unique_ptr<trace_sdk::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);
  auto processor = std::unique_ptr<trace_sdk::SpanProcessor>(
      new trace_sdk::SimpleSpanProcessor(std::move(exporter)));
  std::vector<std::unique_ptr<trace_sdk::SpanProcessor>> processors;
  processors.push_back(std::move(processor));
  auto context = std::make_shared<trace_sdk::TracerContext>(std::move(processors));
  auto provider =
      nostd::shared_ptr<trace_api::TracerProvider>(new trace_sdk::TracerProvider(context));
  // Set the global trace provider
  trace_api::Provider::SetTracerProvider(provider);
}

nostd::shared_ptr<trace_api::Tracer> get_tracer()
{
  auto provider = trace_api::Provider::GetTracerProvider();
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
class NoopEventHandler : public http_client::EventHandler
{
public:
  void OnEvent(http_client::SessionState state, nostd::string_view reason) noexcept override {}

  void OnConnecting(const http_client::SSLCertificate &) noexcept override {}

  void OnResponse(http_client::Response &response) noexcept override {}
};
}  // namespace

// Sends an HTTP POST request to the given url, with the given body.
void send_request(curl::HttpClient &client, const std::string &url, const std::string &body)
{
  static std::unique_ptr<http_client::EventHandler> handler(new NoopEventHandler());

  auto request_span = get_tracer()->StartSpan(__func__);
  trace_api::Scope scope(request_span);

  Uri uri{url};

  auto session = client.CreateSession(url);
  auto request = session->CreateRequest();

  request->SetMethod(http_client::Method::Post);
  request->SetUri(uri.path);
  http_client::Body b = {body.c_str(), body.c_str() + body.size()};
  request->SetBody(b);
  request->AddHeader("Content-Type", "application/json");
  request->AddHeader("Content-Length", std::to_string(body.size()));

  std::map<std::string, std::string> headers;
  TextMapCarrierTest carrier(headers);
  propagator_format.Inject(carrier, context::RuntimeContext::GetCurrent());

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
  trace_api::Scope scope(root_span);

  testing::HttpServer server(default_host, port);
  curl::HttpClient client;

  testing::HttpRequestCallback test_cb{
      [&](testing::HttpRequest const &req, testing::HttpResponse &resp) {
        auto body = nlohmann::json::parse(req.content);

        std::cout << "Received request with body :\n" << req.content << "\n";

        for (auto &part : body)
        {
          const TextMapCarrierTest carrier((std::map<std::string, std::string> &)req.headers);
          auto current_ctx = context::RuntimeContext::GetCurrent();
          auto ctx         = propagator_format.Extract(carrier, current_ctx);
          auto token       = context::RuntimeContext::Attach(ctx);

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
