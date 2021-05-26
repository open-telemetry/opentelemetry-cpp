// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/ext/http/common/url_parser.h"
#include "tracer_common.h"

namespace
{

void sendRequest(const std::string &url)
{
  auto http_client = opentelemetry::ext::http::client::HttpClientFactory::CreateSync();

  // start active span
  opentelemetry::trace::StartSpanOptions options;
  options.kind = opentelemetry::trace::SpanKind::kClient;  // client
  opentelemetry::ext::http::common::UrlParser url_parser(url);

  std::string span_name = url_parser.path_;
  auto span             = get_tracer("http-client")
                  ->StartSpan(span_name,
                              {{"http.url", url_parser.url_},
                               {"http.scheme", url_parser.scheme_},
                               {"http.method", "GET"}},
                              options);
  auto scope = get_tracer("http-client")->WithActiveSpan(span);

  // inject current context into http header
  auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
  HttpTextMapCarrier<opentelemetry::ext::http::client::Headers> carrier;
  auto prop = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
  prop->Inject(carrier, current_ctx);

  // send http request
  opentelemetry::ext::http::client::Result result = http_client->Get(url, carrier.headers_);
  if (result)
  {
    // set span attributes
    auto status_code = result.GetResponse().GetStatusCode();
    span->SetAttribute("http.status_code", status_code);
    result.GetResponse().ForEachHeader([&span](opentelemetry::nostd::string_view header_name,
                                               opentelemetry::nostd::string_view header_value) {
      span->SetAttribute("http.header." + std::string(header_name.data()), header_value);
      return true;
    });

    if (status_code >= 400)
    {
      span->SetStatus(opentelemetry::trace::StatusCode::kError);
    }
  }
  else
  {
    span->SetStatus(opentelemetry::trace::StatusCode::kError,
                    "Response Status :" +
                        std::to_string(static_cast<typename std::underlying_type<
                                           opentelemetry::ext::http::client::SessionState>::type>(
                            result.GetSessionState())));
  }
  // end span and export data
  span->End();
}

}  // namespace

int main(int argc, char *argv[])
{
  initTracer();
  constexpr char default_host[]   = "localhost";
  constexpr char default_path[]   = "/helloworld";
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

  std::string url = "http://" + std::string(default_host) + ":" + std::to_string(port) +
                    std::string(default_path);
  sendRequest(url);
}
