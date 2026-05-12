// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <string>

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

class LoggingRequest : public Request
{
public:
  void SetMethod(Method method) noexcept override
  {
    switch (method)
    {
      case Method::Get:
        method_ = "GET";
        break;
      case Method::Post:
        method_ = "POST";
        break;
      case Method::Put:
        method_ = "PUT";
        break;
      default:
        method_ = "OTHER";
        break;
    }
  }

  void SetUri(nostd::string_view uri) noexcept override { uri_ = std::string(uri); }

  void SetBody(Body &body) noexcept override { body_size_ = body.size(); }

  void SetSslOptions(const HttpSslOptions &) noexcept override {}
  void AddHeader(nostd::string_view, nostd::string_view) noexcept override {}
  void ReplaceHeader(nostd::string_view, nostd::string_view) noexcept override {}
  void SetTimeoutMs(std::chrono::milliseconds) noexcept override {}
  void SetCompression(const Compression &) noexcept override {}
  void EnableLogging(bool) noexcept override {}
  void SetRetryPolicy(const RetryPolicy &) noexcept override {}

  const std::string &GetMethod() const { return method_; }
  const std::string &GetUri() const { return uri_; }
  std::size_t GetBodySize() const { return body_size_; }

private:
  std::string method_;
  std::string uri_;
  std::size_t body_size_ = 0;
};

class OkResponse : public NoopResponse
{
public:
  StatusCode GetStatusCode() const noexcept override { return 200; }
};

class LoggingSession : public Session
{
public:
  std::shared_ptr<Request> CreateRequest() noexcept override
  {
    request_ = std::make_shared<LoggingRequest>();
    return request_;
  }

  void SendRequest(std::shared_ptr<EventHandler> handler) noexcept override
  {
    if (request_)
    {
      std::cout << "Custom HTTP client: " << request_->GetMethod() << " " << request_->GetUri()
                << " (" << request_->GetBodySize() << " bytes)\n";
    }
    if (!handler)
    {
      return;
    }
    OkResponse response;
    handler->OnResponse(response);
    handler->OnEvent(SessionState::Response, {});
    std::cout << "Custom HTTP client: export acknowledged\n";
  }

  bool IsSessionActive() noexcept override { return false; }
  bool CancelSession() noexcept override { return true; }
  bool FinishSession() noexcept override { return true; }

private:
  std::shared_ptr<LoggingRequest> request_;
};

class LoggingHttpClient : public HttpClient
{
public:
  std::shared_ptr<Session> CreateSession(nostd::string_view url) noexcept override
  {
    std::cout << "Custom HTTP client: creating session for " << std::string(url) << "\n";
    return std::make_shared<LoggingSession>();
  }

  bool CancelAllSessions() noexcept override { return true; }
  bool FinishAllSessions() noexcept override { return true; }
  void SetMaxSessionsPerConnection(std::size_t) noexcept override {}
};

}  // namespace

std::shared_ptr<HttpClient> HttpClientFactory::Create()
{
  return std::make_shared<LoggingHttpClient>();
}

std::shared_ptr<HttpClient> HttpClientFactory::Create(
    const std::shared_ptr<sdk::common::ThreadInstrumentation> &)
{
  return std::make_shared<LoggingHttpClient>();
}

std::shared_ptr<HttpClientSync> HttpClientFactory::CreateSync()
{
  return nullptr;
}

}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
