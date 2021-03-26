#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "tracer_common.hpp"

namespace
{

void sendRequest(std::string url)
{
  auto http_client = opentelemetry::ext::http::client::HttpClientFactory::CreateSync();

  // start active span
  opentelemetry::trace::StartSpanOptions options;
  options.kind = opentelemetry::trace::SpanKind::kClient;  // server
  auto span =
      get_tracer("http-client")
          ->StartSpan("sentRequest", {{"component", "http"}, {"http.method", "GET"}}, options);
  auto scope = get_tracer("http-client")->WithActiveSpan(span);

  opentelemetry::ext::http::client::Result result = http_client->Get(url);
  span->SetAttribute("http.url", url);
  if (result)
  {
    // set span attributes
    span->SetAttribute("http.status_code", result.GetResponse().GetStatusCode());
    result.GetResponse().ForEachHeader([&span](opentelemetry::nostd::string_view header_name,
                                               opentelemetry::nostd::string_view header_value) {
      span->SetAttribute("http.header." + std::string(header_name.data()), header_value);
      return true;
    });
  }
  span->SetAttribute(
      "http.session_state",
      static_cast<
          typename std::underlying_type<opentelemetry::ext::http::client::SessionState>::type>(
          result.GetSessionState()));
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
