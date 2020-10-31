#pragma once

#include "curl_http_operation.h"

#include <string>
#include <vector>
#include <map>

//#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporters
{
namespace common
{
namespace http
{
namespace curl
{

namespace http_sdk = opentelemetry::sdk::common::http;
const http_sdk::StatusCode Http_Ok = 200;

class Request : public http_sdk::Request
{

public:
  Request() : method_(http_sdk::Method::Get), uri_("/") {}

  void SetMethod(http_sdk::Method method) noexcept override { method_ = method; }

  void SetBody(http_sdk::Body &body) noexcept override { body_ = std::move(body); }

  void AddHeader(nostd::string_view name, nostd::string_view value) noexcept override
  {
    headers_.insert(std::pair<nostd::string_view, nostd::string_view>(name, value));
  }

  void ReplaceHeader(nostd::string_view name, nostd::string_view value) noexcept override
  {
    // erase matching headers
    auto range = headers_.equal_range(name);
    headers_.erase(range.first, range.second);
    AddHeader(name, value);
  }

  virtual void SetUri(nostd::string_view uri) noexcept override { uri_ = uri; }

  void SetTimeoutMs(std::chrono::milliseconds timeout_ms) noexcept override
  {
    _timeout_ms = timeout_ms;
  }

public:
  http_sdk::Method method_;
  http_sdk::Body body_;
  Headers headers_;
  nostd::string_view uri_;
  std::chrono::milliseconds _timeout_ms;
};

class Response : public http_sdk::Response
{

public:
  Response() : status_code_(Http_Ok) {}

  virtual const http_sdk::Body &GetBody() const noexcept override { return body_; }

  virtual bool ForEachHeader(nostd::function_ref<bool(nostd::string_view name, nostd::string_view value)> callable) const
      noexcept override
  {
    for (const auto &header : headers_)
    {
      if (!callable(name, value))
      {
        return false;
      }
    }
    return true;
  }

  virtual bool ForEachHeader(const std::string &key,
                             nostd::function_ref<bool(nostd::string_view name, nostd::string_view value)> callable) const
      noexcept override
  {
    auto range = headers_.equal_range(key);
    for (auto it = range.first; it != range.second; ++it)
    {
      if (!callable(std::make_pair(name, value))
      {
        return false;
      }
    }
    return true;
  }

  virtual http_sdk::StatusCode GetStatusCode() const noexcept override { return status_code_; }

  virtual const std::string &GetErrorMessage() const noexcept override { return error_message_; }

  virtual bool IsSuccess() const noexcept override { return (error_message_.length() == 0); }

public:
  Headers headers_;
  http_sdk::Body body_;
  http_sdk::StatusCode status_code_;
  std::string error_message_;
};

class SessionManager;

class Session : public http_sdk::Session
{

public:
  Session(SessionManager &session_manager, std::string host, uint16_t port = 80)
      : session_manager_(session_manager), session_state_(http_sdk::SessionState::CREATED)
  {
    if (host.rfind("http://", 0) != 0 && host.rfind("https://", 0) != 0)
    {
      host_ = "http://" + host;  // TBD - https support
    }
    host_ = host + ":" + std::to_string(port);
    //httplib_client_.reset(new httplib::Client(host.c_str()));
  }

  std::shared_ptr<http_sdk::Request> CreateRequest() noexcept override
  {
    http_request_.reset(new Request());
    return http_request_;
  }

  virtual void SendRequest(http_sdk::EventHandler &callback) noexcept override
  {
    std::string url = host_ + "/" + http_request_->uri_;

    curl_operation_->reset(new HttpOperation(http_request_->method_, url, http_request_->headers_, callback, http_request_->body_)));

    curl_operation_->SendAsync([this, callback](HttpOperation& operation){
        if (operation.WasAborted()) {
            //Manually cancelled
            callback->OnEvent(http_sdk::SessionState::Cancelled, "");     
        }

        if (operation.GetStatusCode() == CURLE_OK){
          auto response = std::unique_ptr<Response>(new Response());
          //auto responseHeaders = operation.GetResponseHeaders();
          response->headers_ = operation.GetResponseHeaders();  //.insert(responseHeaders.begin(), responseHeaders.end());
          response->m_body = operation.GetResponseBody();
          callback->OnResponse(response.release());
        }
    });
  }

  virtual http_sdk::SessionState GetSessionState() const noexcept override
  {
    return session_state_;
  }

  virtual bool CancelSession() noexcept override
  {
    curl_operation_->Abort();
    return true;
  }

  virtual bool FinishSession() noexcept override
  {
    curl_operation_->Finish();
    return true;
  }

  void SetId(uint64_t session_id) { session_id_ = session_id; }

private:
  std::shared_ptr<Request> http_request_;
  http_sdk::SessionState session_state_;
  nostd::string_view host_;
  std::unique_ptr<HttpOperation> curl_operation_;
  uint64_t session_id_;
  SessionManager &session_manager_;
  //std::unique_ptr<httplib::Client> httplib_client_;
};

class SessionManager : public http_sdk::SessionManager
{

public:

  SessionManager()
  {
    curl_global_init(CURL_GLOBAL_ALL);
  }

  std::shared_ptr<http_sdk::Session> createSession(std::string host,
                                                           uint16_t port = 80) noexcept override
  {
    auto session    = std::make_shared<Session>(*this, host, port);
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

  ~SessionManager() {
    curl_global_cleanup();
  }

private:
  std::atomic<uint64_t> next_session_id_;
  std::map<uint64_t, std::shared_ptr<Session>> sessions_;
};

} // namespace curl
}  // namespace http
}  // namespace common
}  // namespace exporters
OPENTELEMETRY_END_NAMESPACE
