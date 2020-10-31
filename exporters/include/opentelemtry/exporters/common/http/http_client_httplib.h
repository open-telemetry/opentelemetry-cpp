#pragma once

#include "httplib.h"
#include "opentelemetry/sdk/common/http_client.h"

#include <future>
#include <map>
#include <string>

#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporters
{
namespace common
{
namespace http
{

namespace http_sdk = opentelemetry::sdk::common::http;
using HttpHeaders  = std::multimap<std::string, std::string, httplib::detail::ci>;

const http_sdk::HttpStatusCode HTTP_OK = 200;

class HttpRequest : public http_sdk::HttpRequest
{

public:
  HttpRequest() : method_(http_sdk::HttpMethod::HTTP_GET), uri_("/") {}

  void setMethod(http_sdk::HttpMethod method) noexcept override { method_ = method; }

  void SetBody(http_sdk::HttpBody &body) noexcept override { body_ = std::move(body); }

  void AddHeader(const http_sdk::HttpHeader &header) noexcept override
  {
    headers_.insert({header.first, header.second});
  }

  void ReplaceHeader(const http_sdk::HttpHeader &header) noexcept override
  {
    // erase matching headers
    auto range = headers_.equal_range(header.first);
    headers_.erase(range.first, range.second);
    AddHeader(header);
  }

  virtual void setURI(const std::string &uri) noexcept override { uri_ = uri; }

  void SetTimeoutMs(std::chrono::milliseconds timeout_ms) noexcept override
  {
    _timeout_ms = timeout_ms;
  }

public:
  http_sdk::HttpMethod method_;
  http_sdk::HttpBody body_;
  HttpHeaders headers_;
  std::string uri_;
  std::chrono::milliseconds _timeout_ms;
};

class HttpResponse : public http_sdk::HttpResponse
{

public:
  HttpResponse() : status_code_(HTTP_OK) {}

  virtual const http_sdk::HttpBody &GetBody() const noexcept override { return body_; }

  virtual bool GetNextHeader(nostd::function_ref<bool(http_sdk::HttpHeader)> callable) const
      noexcept override
  {
    for (const auto &header : headers_)
    {
      if (!callable(std::make_pair(header.first, header.second)))
      {
        return false;
      }
    }
    return true;
  }

  virtual bool GetNextHeader(const std::string &key,
                             nostd::function_ref<bool(http_sdk::HttpHeader)> callable) const
      noexcept override
  {
    auto range = headers_.equal_range(key);
    for (auto it = range.first; it != range.second; ++it)
    {
      if (!callable(std::make_pair(it->first, it->second)))
      {
        return false;
      }
    }
    return true;
  }

  virtual http_sdk::HttpStatusCode GetStatusCode() const noexcept override { return status_code_; }

  virtual const std::string &GetErrorMessage() const noexcept override { return error_message_; }

  virtual bool IsSuccess() const noexcept override { return (error_message_.length() == 0); }

public:
  HttpHeaders headers_;
  http_sdk::HttpBody body_;
  http_sdk::HttpStatusCode status_code_;
  std::string error_message_;
};

class HttpSessionManager;

class HttpSession : public http_sdk::HttpSession
{

public:
  HttpSession(HttpSessionManager &session_manager, std::string host, uint16_t port = 80)
      : session_manager_(session_manager), session_state_(http_sdk::HttpSessionState::CREATED)
  {
    if (host.rfind("http://", 0) != 0 && host.rfind("https://", 0) != 0)
    {
      host = "http://" + host;  // TBD - https support
    }
    host = host + ":" + std::to_string(port);
    httplib_client_.reset(new httplib::Client(host.c_str()));
  }

  std::shared_ptr<http_sdk::HttpRequest> CreateRequest() noexcept override
  {
    http_request_.reset(new HttpRequest());
    return http_request_;
  }

  virtual void SendRequest(http_sdk::HttpResponseHandler &callback) noexcept override
  {
    HttpResponse res;
    if (http_sdk::HttpMethod::HTTP_GET == http_request_->method_)
    {
      auto result_future = std::async(std::launch::async, [this, &callback, &res] {
        return this->SendGetRequestAsync(callback, res);
      });
    }
    else
    {
      auto result_future = std::async(std::launch::async, [this, &callback, &res] {
        return this->SendPostRequestAsync(callback, res);
      });
    }
    // &HttpSession::SendRequestAsync, this, callback);
  }

  virtual http_sdk::HttpSessionState GetSessionState() const noexcept override
  {
    return session_state_;
  }

  virtual bool CancelSession() noexcept override
  {
    // TBD - Not Implemented
    return true;
  }

  virtual bool FinishSession() noexcept override
  {
    // TBD - Not Implemented
    return true;
  }

  void SetId(uint64_t session_id) { session_id_ = session_id; }

private:
  void SendGetRequestAsync(http_sdk::HttpResponseHandler &callback, HttpResponse &res)
  {
    session_state_ = http_sdk::HttpSessionState::ONGOING;
    httplib::Result httplib_res =
        httplib_client_->Get(http_request_->uri_.c_str(), http_request_->headers_);
    httplib::Error err = httplib_res.error();
    if (err == httplib::Error::Success)
    {
      res.status_code_ = httplib_res->status;
      res.headers_     = std::move(httplib_res->headers);
      std::move(httplib_res->body.begin(), httplib_res->body.end(), std::back_inserter(res.body_));
      session_state_ = http_sdk::HttpSessionState::FINISHED;
      callback.OnHttpResponse(res);
    }
    else
    {
      res.error_message_ = get_error_message(err);
      session_state_     = http_sdk::HttpSessionState::FINISHED;
      callback.OnHttpResponse(res);
    }
  }

  void SendPostRequestAsync(http_sdk::HttpResponseHandler &callback, HttpResponse &res)
  {
    session_state_ = http_sdk::HttpSessionState::ONGOING;
    std::string body(http_request_->body_.begin(), http_request_->body_.end());
    // find content type from headers;
    auto search = http_request_->headers_.find("Content-Type");
    std::string content_type;
    if (search != http_request_->headers_.end())
    {
      content_type = search->second;
    }
    else
    {
      content_type = "application/json";  // defaults
    }
    httplib::Result httplib_res =
        httplib_client_->Post(http_request_->uri_.c_str(), std::move(body), content_type.c_str());
    httplib::Error err = httplib_res.error();
    if (err == httplib::Error::Success)
    {

      res.status_code_ = httplib_res->status;
      res.headers_     = std::move(httplib_res->headers);
      std::move(httplib_res->body.begin(), httplib_res->body.end(), std::back_inserter(res.body_));
      session_state_ = http_sdk::HttpSessionState::FINISHED;
      callback.OnHttpResponse(res);
    }
    else
    {
      res.error_message_ = get_error_message(err);
      session_state_     = http_sdk::HttpSessionState::FINISHED;
      callback.OnHttpResponse(res);
    }
  }

  std::string get_error_message(httplib::Error err)
  {
    switch (err)
    {
      case httplib::Error::Unknown:
        return "Unknown error";
      case httplib::Error::Connection:
        return "Connection error";
      case httplib::Error::BindIPAddress:
        return "Bind IP address error";
      case httplib::Error::Read:
        return "Read error";
      case httplib::Error::Write:
        return "Write error";
      case httplib::ExceedRedirectCount:
        return "Redirect Count Exceeded error";
      case httplib::Canceled:
        return "Cancelled Error";
      case httplib::SSLConnection:
        return "SSL Connection error";
      case httplib::SSLLoadingCerts:
        return "SSL Certificate loading error";
      case httplib::SSLServerVerification:
        return "SSL Server Verification error";
      case httplib::UnsupportedMultipartBoundaryChars:
        return "Unsupported Multiple Boundary Characters";
      default:
        return "Unknown error";
    }
  }

private:
  std::shared_ptr<HttpRequest> http_request_;
  http_sdk::HttpSessionState session_state_;
  uint64_t session_id_;
  HttpSessionManager &session_manager_;
  std::unique_ptr<httplib::Client> httplib_client_;
};

class HttpSessionManager : public http_sdk::HttpSessionManager
{

public:
  std::shared_ptr<http_sdk::HttpSession> createHttpSession(std::string host,
                                                           uint16_t port = 80) noexcept override
  {
    auto session    = std::make_shared<HttpSession>(*this, host, port);
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

private:
  std::atomic<uint64_t> next_session_id_;
  std::map<uint64_t, std::shared_ptr<HttpSession>> sessions_;
};

}  // namespace http
}  // namespace common
}  // namespace exporters
OPENTELEMETRY_END_NAMESPACE
