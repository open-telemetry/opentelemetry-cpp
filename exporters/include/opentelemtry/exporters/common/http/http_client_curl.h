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

  virtual bool ForEachHeader(
    nostd::function_ref<bool(nostd::string_view name, nostd::string_view value)> callable) const
      noexcept override
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

  virtual bool ForEachHeader(const nostd::string_view &name,
                             nostd::function_ref<bool(nostd::string_view name, nostd::string_view value)> callable) const
      noexcept override
  {
    auto range = headers_.equal_range(name);
    for (auto it = range.first; it != range.second; ++it)
    {
      if (!callable(it->first, it->second))
      {
        return false;
      }
    }
    return true;
  }

  virtual http_sdk::StatusCode GetStatusCode() const noexcept override { return status_code_; }

public:
  Headers headers_;
  http_sdk::Body body_;
  http_sdk::StatusCode status_code_;
};

class SessionManager;

class Session : public http_sdk::Session
{

public:
  Session(SessionManager &session_manager, std::string host, uint16_t port = 80)
      : session_manager_(session_manager), is_session_active_(false)
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
    is_session_active_ = true;
    std::string url = static_cast<std::string>(host_) + "/" + static_cast<std::string>(http_request_->uri_);

    curl_operation_.reset(new HttpOperation(http_request_->method_, url, &callback, http_request_->headers_, http_request_->body_));
    auto callback_ptr = &callback;
    curl_operation_->SendAsync([this, callback_ptr](HttpOperation& operation){
        if (operation.WasAborted()) {
            //Manually cancelled
            callback_ptr->OnEvent(http_sdk::SessionState::Cancelled, "");     
        }

        if (operation.GetResponseCode() >= CURL_LAST){ 
          // we have a http response
          auto response = std::unique_ptr<Response>(new Response());
          //auto responseHeaders = operation.GetResponseHeaders();
          response->headers_ = operation.GetResponseHeaders();  //.insert(responseHeaders.begin(), responseHeaders.end());
          response->body_ = operation.GetResponseBody();
          callback_ptr->OnResponse(*response);
        }

    });
    is_session_active_ = false;
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
  
  virtual bool IsSessionActive() noexcept override
  {
    return is_session_active_;
  }

  void SetId(uint64_t session_id) { session_id_ = session_id; }

private:
  std::shared_ptr<Request> http_request_;
  nostd::string_view host_;
  std::unique_ptr<HttpOperation> curl_operation_;
  uint64_t session_id_;
  SessionManager &session_manager_;
  bool is_session_active_;
  //std::unique_ptr<httplib::Client> httplib_client_;
};

class SessionManager : public http_sdk::SessionManager
{

public:

  SessionManager()
  {
    curl_global_init(CURL_GLOBAL_ALL);
  }

  std::shared_ptr<http_sdk::Session> CreateSession(nostd::string_view host,
                                                           uint16_t port = 80) noexcept override
  {
    auto session    = std::make_shared<Session>(*this, static_cast<std::string>(host), port);
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
