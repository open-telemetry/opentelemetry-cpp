// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/sdk/common/thread_instrumentation.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace http
{
namespace client
{

namespace
{

class NoopRequest : public Request
{
public:
  void SetMethod(Method) noexcept override {}
  void SetUri(nostd::string_view) noexcept override {}
  void SetSslOptions(const HttpSslOptions &) noexcept override {}
  void SetBody(Body &) noexcept override {}
  void AddHeader(nostd::string_view, nostd::string_view) noexcept override {}
  void ReplaceHeader(nostd::string_view, nostd::string_view) noexcept override {}
  void SetTimeoutMs(std::chrono::milliseconds) noexcept override {}
  void SetCompression(const Compression &) noexcept override {}
  void EnableLogging(bool) noexcept override {}
  void SetRetryPolicy(const RetryPolicy &) noexcept override {}
};

class NoopSession : public Session
{
public:
  std::shared_ptr<Request> CreateRequest() noexcept override
  {
    return std::make_shared<NoopRequest>();
  }

  void SendRequest(std::shared_ptr<EventHandler> handler) noexcept override
  {
    if (!handler)
    {
      return;
    }
    NoopResponse response;
    handler->OnResponse(response);
    handler->OnEvent(SessionState::Response, {});
  }

  bool IsSessionActive() noexcept override { return false; }
  bool CancelSession() noexcept override { return true; }
  bool FinishSession() noexcept override { return true; }
};

class NoopHttpClient : public HttpClient
{
public:
  std::shared_ptr<Session> CreateSession(nostd::string_view) noexcept override
  {
    return std::make_shared<NoopSession>();
  }

  bool CancelAllSessions() noexcept override { return true; }
  bool FinishAllSessions() noexcept override { return true; }
  void SetMaxSessionsPerConnection(std::size_t) noexcept override {}
};

}  // namespace

std::shared_ptr<HttpClient> HttpClientFactory::Create()
{
  return std::make_shared<NoopHttpClient>();
}

std::shared_ptr<HttpClient> HttpClientFactory::Create(
    const std::shared_ptr<sdk::common::ThreadInstrumentation> &)
{
  return std::make_shared<NoopHttpClient>();
}

std::shared_ptr<HttpClientSync> HttpClientFactory::CreateSync()
{
  return nullptr;
}

}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
