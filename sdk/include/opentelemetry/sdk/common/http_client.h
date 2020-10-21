#pragma once

#include <chrono>
#include <string>
#include <vector>
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/version.h"

/*
 Usage Example

 struct SimpleReponseHandler: public ResponseHandler {
      void OnResponse(Response& res) noexcept override
      {
           if (res.IsSuccess()) {
            res.GetNextHeader([](std::string name, std::string value) -> bool {
                std::cout << "Header Name:" << name << " Header Value:"<< value ;
                return true;
            });
            .. process response body
           }
      }
  };

  SessionManager sessionManager; // implementer can provide singleton implementation for it
  auto session = sessionManager.createSession("localhost", 8000);
  auto request = session->CreateRequest();
  request->AddHeader(..);
  SimpleResponseHandler res_handler;
  session->SendRequest(res_handler);
  session->FinishSession() // optionally in the end
  ...shutdown
  sessionManager.FinishAllSessions()
*/

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
namespace http
{

enum class Method
{
  Get,
  Post,
  Put,
  Options,
  Head,
  Patch,
  Delete
};

enum class SessionState
{
  Created,  // session object is created
  Ongoing,  // http(s) request is ongoing
  Finished  // https(s) request is finished
};

using Body       = std::vector<uint8_t>;
using StatusCode = uint16_t;

class Request
{

public:
  virtual void SetMethod(Method method) noexcept = 0;

  virtual void SetUri(const std::string &uri) noexcept = 0;

  virtual void SetBody(Body &body) noexcept = 0;

  virtual void AddHeader(const std::string &name, const std::string &value) noexcept = 0;

  virtual void ReplaceHeader(const std::string &name, const std::string &value) noexcept = 0;

  virtual void SetTimeoutMs(std::chrono::milliseconds timeout_ms) noexcept = 0;
};

class Response
{
public:
  virtual const Body &GetBody() const noexcept = 0;

  virtual bool GetNextHeader(
      nostd::function_ref<bool(std::string name, std::string value)> callable) const noexcept = 0;

  virtual bool GetNextHeader(
      const std::string &key,
      nostd::function_ref<bool(std::string name, std::string value)> callable) const noexcept = 0;

  virtual StatusCode GetStatusCode() const noexcept = 0;
};

class ResponseHandler
{
public:
  virtual void OnResponse(Response &) noexcept = 0;

  virtual void OnError(std::string &) noexcept = 0;
};

class Session
{
public:
  virtual std::shared_ptr<Request> CreateRequest() noexcept = 0;

  virtual void SendRequest(ResponseHandler &) noexcept = 0;

  virtual SessionState GetSessionState() const noexcept = 0;

  virtual bool CancelSession() noexcept = 0;

  virtual bool FinishSession() noexcept = 0;
};

class SessionManager
{
public:
  virtual std::shared_ptr<Session> CreateSession(std::string host, uint16_t port = 80) noexcept = 0;

  virtual bool CancelAllSessions() noexcept = 0;

  virtual bool FinishAllSessions() noexcept = 0;
};
}  // namespace http
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
