#include "opentelemetry/ext/http/client/curl/http_client_curl.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"

std::shared_ptr<opentelemetry::ext::http::client::SessionManager>
opentelemetry::ext::http::client::HttpClientFactory::Create()
{
  return std::make_shared<opentelemetry::ext::http::client::curl::SessionManager>();
}