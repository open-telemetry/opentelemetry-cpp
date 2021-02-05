#pragma once

#include "http_operation_curl.h"
#include "opentelemetry/ext/http/client/http_client.h"
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

namespace http_client                 = opentelemetry::ext::http::client;
const http_client::StatusCode Http_Ok = 200;

class Request : public http_client::Request
{
public:
  Request() : method_(http_client::Method::Get), uri_("/") {}

  void SetMethod(http_client::Method method) noexcept override { method_ = method; }

  void SetBody(http_client::Body &body) noexcept override { body_ = std::move(body); }

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
  http_client::Method method_;
  http_client::Body body_;
  http_client::Headers headers_;
  std::string uri_;
  std::chrono::milliseconds timeout_ms_{5000};  // ms
};

class Response : public http_client::Response
{
public:
  Response() : status_code_(Http_Ok) {}

  virtual const http_client::Body &GetBody() const noexcept override { return body_; }

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

  virtual http_client::StatusCode GetStatusCode() const noexcept override { return status_code_; }

public:
  Headers headers_;
  http_client::Body body_;
  http_client::StatusCode status_code_;
};

class HttpClient;

class Session : public http_client::Session
{
public:
  Session(HttpClient &http_client, const std::string &host, uint16_t port = 80)
      : http_client_(http_client), is_session_active_(false)
  {
    if (host.rfind("http://", 0) != 0 && host.rfind("https://", 0) != 0)
    {
      host_ = "http://" + host;  // TODO - https support
    }
    else
    {
      host_ = host;
    }
    host_ += ":" + std::to_string(port) + "/";
  }

  std::shared_ptr<http_client::Request> CreateRequest() noexcept override
  {
    http_request_.reset(new Request());
    return http_request_;
  }

  virtual void SendRequest(http_client::EventHandler &callback) noexcept override
  {
    is_session_active_ = true;
    std::string url    = host_ + std::string(http_request_->uri_);
    auto callback_ptr  = &callback;
    curl_operation_.reset(new HttpOperation(
        http_request_->method_, url, callback_ptr, RequestMode::Sync, http_request_->headers_,
        http_request_->body_, false, http_request_->timeout_ms_));
    curl_operation_->SendAsync([this, callback_ptr](HttpOperation &operation) {
      if (operation.WasAborted())
      {
        // Manually cancelled
        callback_ptr->OnEvent(http_client::SessionState::Cancelled, "");
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

private:
  std::shared_ptr<Request> http_request_;
  std::string host_;
  std::unique_ptr<HttpOperation> curl_operation_;
  uint64_t session_id_;
  HttpClient &http_client_;
  bool is_session_active_;
};

class HttpClientSync : public http_client::HttpClientSync
{
public:
  HttpClientSync() { curl_global_init(CURL_GLOBAL_ALL); }

  http_client::Result Get(const nostd::string_view &url,
                          const http_client::Headers &headers) noexcept override
  {
    http_client::Body body;
    HttpOperation curl_operation(http_client::Method::Get, url.data(), nullptr, RequestMode::Sync,
                                 headers, body);
    curl_operation.SendSync();
    auto session_state = curl_operation.GetSessionState();
    if (curl_operation.WasAborted())
    {
      session_state = http_client::SessionState::Cancelled;
    }
    auto response = std::unique_ptr<Response>(new Response());
    if (curl_operation.GetResponseCode() >= CURL_LAST)
    {
      // we have a http response

      response->headers_     = curl_operation.GetResponseHeaders();
      response->body_        = curl_operation.GetResponseBody();
      response->status_code_ = curl_operation.GetResponseCode();
    }
    return http_client::Result(std::move(response), session_state);
  }

  http_client::Result Post(const nostd::string_view &url,
                           const Data &data,
                           const http_client::Headers &headers) noexcept override
  {
    HttpOperation curl_operation(http_client::Method::Post, url.data(), nullptr, RequestMode::Sync,
                                 headers);
    curl_operation.SendSync();
    auto session_state = curl_operation.GetSessionState();
    if (curl_operation.WasAborted())
    {
      session_state = http_client::SessionState::Cancelled;
    }
    auto response = std::unique_ptr<Response>(new Response());
    if (curl_operation.GetResponseCode() >= CURL_LAST)
    {
      // we have a http response

      response->headers_     = curl_operation.GetResponseHeaders();
      response->body_        = curl_operation.GetResponseBody();
      response->status_code_ = curl_operation.GetResponseCode();
    }

    return http_client::Result(std::move(response), session_state);
  }

  ~HttpClientSync() { curl_global_cleanup(); }
};

class HttpClient : public http_client::HttpClient
{
public:
  // The call (curl_global_init) is not thread safe. Ensure this is called only once.
  HttpClient() : next_session_id_{0} { curl_global_init(CURL_GLOBAL_ALL); }

  std::shared_ptr<http_client::Session> CreateSession(nostd::string_view host,
                                                      uint16_t port = 80) noexcept override
  {
    auto session    = std::make_shared<Session>(*this, std::string(host), port);
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
