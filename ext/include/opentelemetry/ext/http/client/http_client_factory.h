#pragma once
#include "opentelemetry/ext/http/client/http_client.h"
#ifdef WITH_CURL
#  include "opentelemetry/ext/http/client/curl/http_client_curl.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace http
{
namespace client
{
class HttpClientFactory
{
public:
#ifdef WITH_CURL
  static std::shared_ptr<SessionManager> Create()
  {
    return std::make_shared<ext::http::client::curl::SessionManager>();
  }
#endif
};
}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
