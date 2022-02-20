// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_TEST
#  include "opentelemetry/ext/http/client/http_client.h"
#  include "opentelemetry/ext/http/common/url_parser.h"
#  include "opentelemetry/version.h"

#  include <map>
#  include <string>
#  include <vector>

#  include <gtest/gtest.h>
#  include "gmock/gmock.h"

using namespace testing;
OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace http
{
namespace client
{
namespace nosend
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

  void ReplaceHeader(nostd::string_view name, nostd::string_view value) noexcept override;

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
      const noexcept override;

  virtual bool ForEachHeader(
      const nostd::string_view &name,
      nostd::function_ref<bool(nostd::string_view name, nostd::string_view value)> callable)
      const noexcept override;

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

  MOCK_METHOD(void,
              SendRequest,
              (opentelemetry::ext::http::client::EventHandler &),
              (noexcept, override));

  virtual bool CancelSession() noexcept override;

  virtual bool FinishSession() noexcept override;

  virtual bool IsSessionActive() noexcept override { return is_session_active_; }

  void SetId(uint64_t session_id) { session_id_ = session_id; }

  /**
   * Returns the base URI.
   * @return the base URI as a string consisting of scheme, host and port.
   */
  const std::string &GetBaseUri() const { return host_; }

  std::shared_ptr<Request> GetRequest() { return http_request_; }

private:
  std::shared_ptr<Request> http_request_;
  std::string host_;
  uint64_t session_id_;
  HttpClient &http_client_;
  bool is_session_active_;
};

class HttpClient : public opentelemetry::ext::http::client::HttpClient
{
public:
  HttpClient() { session_ = std::shared_ptr<Session>{new Session(*this)}; }

  std::shared_ptr<opentelemetry::ext::http::client::Session> CreateSession(
      nostd::string_view) noexcept override
  {
    return session_;
  }

  bool CancelAllSessions() noexcept override
  {
    session_->CancelSession();
    return true;
  }

  bool FinishAllSessions() noexcept override
  {
    session_->FinishSession();
    return true;
  }

  void CleanupSession(uint64_t session_id) {}

  std::shared_ptr<Session> session_;
};

}  // namespace nosend
}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
#endif
