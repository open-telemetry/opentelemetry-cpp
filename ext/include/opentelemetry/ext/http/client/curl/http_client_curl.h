// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "http_operation_curl.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/common/url_parser.h"
#include "opentelemetry/version.h"

#include <map>
#include <string>
#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace http
{
namespace client
{
namespace curl
{

const opentelemetry::ext::http::client::StatusCode Http_Ok = 200;

class Request : public opentelemetry::ext::http::client::Request
{
public:
  Request() : method_(opentelemetry::ext::http::client::Method::Get), uri_("/") {}

  void SetMethod(opentelemetry::ext::http::client::Method method) noexcept override
  {
    method_ = method;
  }

  void SetBody(opentelemetry::ext::http::client::Body &body) noexcept override
  {
    body_ = std::move(body);
  }

  void AddHeader(nostd::string_view name, nostd::string_view value) noexcept override
  {
    headers_.insert(std::pair<std::string, std::string>(static_cast<std::string>(name),
                                                        static_cast<std::string>(value)));
  }

  void ReplaceHeader(nostd::string_view name, nostd::string_view value) noexcept override
  {
    // erase matching headers
    auto range = headers_.equal_range(static_cast<std::string>(name));
    headers_.erase(range.first, range.second);
    AddHeader(name, value);
  }

  virtual void SetUri(nostd::string_view uri) noexcept override
  {
    uri_ = static_cast<std::string>(uri);
  }

  void SetTimeoutMs(std::chrono::milliseconds timeout_ms) noexcept override
  {
    timeout_ms_ = timeout_ms;
  }

public:
  opentelemetry::ext::http::client::Method method_;
  opentelemetry::ext::http::client::Body body_;
  opentelemetry::ext::http::client::Headers headers_;
  std::string uri_;
  std::chrono::milliseconds timeout_ms_{5000};  // ms
};

class Response : public opentelemetry::ext::http::client::Response
{
public:
  Response() : status_code_(Http_Ok) {}

  virtual const opentelemetry::ext::http::client::Body &GetBody() const noexcept override
  {
    return body_;
  }

  virtual bool ForEachHeader(
      nostd::function_ref<bool(nostd::string_view name, nostd::string_view value)> callable)
      const noexcept override
  {
    for (const auto &header : headers_)
    {
      if (!callable(header.first, header.second))
      {
        return false;
      }
    }
    return true;
  }

  virtual bool ForEachHeader(
      const nostd::string_view &name,
      nostd::function_ref<bool(nostd::string_view name, nostd::string_view value)> callable)
      const noexcept override
  {
    auto range = headers_.equal_range(static_cast<std::string>(name));
    for (auto it = range.first; it != range.second; ++it)
    {
      if (!callable(it->first, it->second))
      {
        return false;
      }
    }
    return true;
  }

  virtual opentelemetry::ext::http::client::StatusCode GetStatusCode() const noexcept override
  {
    return status_code_;
  }

public:
  Headers headers_;
  opentelemetry::ext::http::client::Body body_;
  opentelemetry::ext::http::client::StatusCode status_code_;
};

class HttpClient;

class Session : public opentelemetry::ext::http::client::Session
{
public:
  Session(HttpClient &http_client,
          std::string scheme      = "http",
          const std::string &host = "",
          uint16_t port           = 80)
      : http_client_(http_client), is_session_active_(false)
  {
    host_ = scheme + "://" + host + ":" + std::to_string(port) + "/";
  }

  std::shared_ptr<opentelemetry::ext::http::client::Request> CreateRequest() noexcept override
  {
    http_request_.reset(new Request());
    return http_request_;
  }

  virtual void SendRequest(
      opentelemetry::ext::http::client::EventHandler &callback) noexcept override
  {
    is_session_active_ = true;
    std::string url    = host_ + std::string(http_request_->uri_);
    auto callback_ptr  = &callback;
    curl_operation_.reset(new HttpOperation(
        http_request_->method_, url, callback_ptr, RequestMode::Async, http_request_->headers_,
        http_request_->body_, false, http_request_->timeout_ms_));
    curl_operation_->SendAsync([this, callback_ptr](HttpOperation &operation) {
      if (operation.WasAborted())
      {
        // Manually cancelled
        callback_ptr->OnEvent(opentelemetry::ext::http::client::SessionState::Cancelled, "");
      }

      if (operation.GetResponseCode() >= CURL_LAST)
      {
        // we have a http response
        auto response          = std::unique_ptr<Response>(new Response());
        response->headers_     = operation.GetResponseHeaders();
        response->body_        = operation.GetResponseBody();
        response->status_code_ = operation.GetResponseCode();
        callback_ptr->OnResponse(*response);
      }
      is_session_active_ = false;
    });
  }

  virtual bool CancelSession() noexcept override;

  virtual bool FinishSession() noexcept override;

  virtual bool IsSessionActive() noexcept override { return is_session_active_; }

  void SetId(uint64_t session_id) { session_id_ = session_id; }

  /**
   * Returns the base URI.
   * @return the base URI as a string consisting of scheme, host and port.
   */
  const std::string &GetBaseUri() const { return host_; }

#ifdef ENABLE_TEST
  std::shared_ptr<Request> GetRequest() { return http_request_; }
#endif
private:
  std::shared_ptr<Request> http_request_;
  std::string host_;
  std::unique_ptr<HttpOperation> curl_operation_;
  uint64_t session_id_;
  HttpClient &http_client_;
  bool is_session_active_;
};

class HttpClientSync : public opentelemetry::ext::http::client::HttpClientSync
{
public:
  HttpClientSync() { curl_global_init(CURL_GLOBAL_ALL); }

  opentelemetry::ext::http::client::Result Get(
      const nostd::string_view &url,
      const opentelemetry::ext::http::client::Headers &headers) noexcept override
  {
    opentelemetry::ext::http::client::Body body;
    HttpOperation curl_operation(opentelemetry::ext::http::client::Method::Get, url.data(), nullptr,
                                 RequestMode::Sync, headers, body);
    curl_operation.SendSync();
    auto session_state = curl_operation.GetSessionState();
    if (curl_operation.WasAborted())
    {
      session_state = opentelemetry::ext::http::client::SessionState::Cancelled;
    }
    auto response = std::unique_ptr<Response>(new Response());
    if (curl_operation.GetResponseCode() >= CURL_LAST)
    {
      // we have a http response

      response->headers_     = curl_operation.GetResponseHeaders();
      response->body_        = curl_operation.GetResponseBody();
      response->status_code_ = curl_operation.GetResponseCode();
    }
    return opentelemetry::ext::http::client::Result(std::move(response), session_state);
  }

  opentelemetry::ext::http::client::Result Post(
      const nostd::string_view &url,
      const Body &body,
      const opentelemetry::ext::http::client::Headers &headers) noexcept override
  {
    HttpOperation curl_operation(opentelemetry::ext::http::client::Method::Post, url.data(),
                                 nullptr, RequestMode::Sync, headers, body);
    curl_operation.SendSync();
    auto session_state = curl_operation.GetSessionState();
    if (curl_operation.WasAborted())
    {
      session_state = opentelemetry::ext::http::client::SessionState::Cancelled;
    }
    auto response = std::unique_ptr<Response>(new Response());
    if (curl_operation.GetResponseCode() >= CURL_LAST)
    {
      // we have a http response

      response->headers_     = curl_operation.GetResponseHeaders();
      response->body_        = curl_operation.GetResponseBody();
      response->status_code_ = curl_operation.GetResponseCode();
    }

    return opentelemetry::ext::http::client::Result(std::move(response), session_state);
  }

  ~HttpClientSync() { curl_global_cleanup(); }
};

class HttpClient : public opentelemetry::ext::http::client::HttpClient
{
public:
  // The call (curl_global_init) is not thread safe. Ensure this is called only once.
  HttpClient() : next_session_id_{0} { curl_global_init(CURL_GLOBAL_ALL); }

  std::shared_ptr<opentelemetry::ext::http::client::Session> CreateSession(
      nostd::string_view url) noexcept override
  {
    auto parsedUrl = common::UrlParser(std::string(url));
    if (!parsedUrl.success_)
    {
      return std::make_shared<Session>(*this);
    }
    auto session =
        std::make_shared<Session>(*this, parsedUrl.scheme_, parsedUrl.host_, parsedUrl.port_);
    auto session_id = ++next_session_id_;
    session->SetId(session_id);
    sessions_.insert({session_id, session});
    return session;
  }

  bool CancelAllSessions() noexcept override
  {
    for (auto &session : sessions_)
    {
      session.second->CancelSession();
    }
    return true;
  }

  bool FinishAllSessions() noexcept override
  {
    for (auto &session : sessions_)
    {
      session.second->FinishSession();
    }
    return true;
  }

  void CleanupSession(uint64_t session_id)
  {
    // TBD = Need to be thread safe
    sessions_.erase(session_id);
  }

  ~HttpClient() { curl_global_cleanup(); }

private:
  std::atomic<uint64_t> next_session_id_;
  std::map<uint64_t, std::shared_ptr<Session>> sessions_;
};

}  // namespace curl
}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
