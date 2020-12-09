#pragma once

#include <chrono>
#include <string>
#include <vector>
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

/*
 Usage Example

 struct SimpleReponseHandler: public ResponseHandler {
      void OnResponse(Response& res) noexcept override
      {
           if (res.IsSuccess()) {
            res.GetNextHeader([](nostd::string_view name, std::string value) -> bool {
                std::cout << "Header Name:" << name << " Header Value:"<< value ;
                return true;
            });
            .. process response body
           }
      }

      void OnError(nostd::string_view err) noexcept override
      {
          std::cout << " Error:" << err;
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
namespace ext
{
namespace http
{
namespace client
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
  CreateFailed,        // session create failed
  Created,             // session created
  Destroyed,           // session destroyed
  Connecting,          // connecting to peer
  ConnectFailed,       // connection failed
  Connected,           // connected
  Sending,             // sending request
  SendFailed,          // request send failed
  Response,            // response received
  SSLHandshakeFailed,  // SSL handshake failed
  TimedOut,            // request time out
  NetworkError,        // network error
  ReadError,           // error reading response
  WriteError,          // error writing request
  Cancelled            // (manually) cancelled
};

using Byte           = uint8_t;
using StatusCode     = uint16_t;
using Body           = std::vector<Byte>;
using SSLCertificate = std::vector<Byte>;

class Request
{
public:
  virtual void SetMethod(Method method) noexcept = 0;

  virtual void SetUri(nostd::string_view uri) noexcept = 0;

  virtual void SetBody(Body &body) noexcept = 0;

  virtual void AddHeader(nostd::string_view name, nostd::string_view value) noexcept = 0;

  virtual void ReplaceHeader(nostd::string_view name, nostd::string_view value) noexcept = 0;

  virtual void SetTimeoutMs(std::chrono::milliseconds timeout_ms) noexcept = 0;

  virtual ~Request() = default;
};

class Response
{
public:
  virtual const Body &GetBody() const noexcept = 0;

  virtual bool ForEachHeader(
      nostd::function_ref<bool(nostd::string_view name, nostd::string_view value)> callable) const
      noexcept = 0;

  virtual bool ForEachHeader(
      const nostd::string_view &key,
      nostd::function_ref<bool(nostd::string_view name, nostd::string_view value)> callable) const
      noexcept = 0;

  virtual StatusCode GetStatusCode() const noexcept = 0;

  virtual ~Response() = default;
};

class EventHandler
{
public:
  virtual void OnResponse(Response &) noexcept = 0;

  virtual void OnEvent(SessionState, nostd::string_view) noexcept = 0;

  virtual void OnConnecting(const SSLCertificate &) noexcept {}

  virtual ~EventHandler() = default;
};

class Session
{
public:
  virtual std::shared_ptr<Request> CreateRequest() noexcept = 0;

  virtual void SendRequest(EventHandler &) noexcept = 0;

  virtual bool IsSessionActive() noexcept = 0;

  virtual bool CancelSession() noexcept = 0;

  virtual bool FinishSession() noexcept = 0;

  virtual ~Session() = default;
};

class SessionManager
{
public:
  virtual std::shared_ptr<Session> CreateSession(nostd::string_view host,
                                                 uint16_t port = 80) noexcept = 0;

  virtual bool CancelAllSessions() noexcept = 0;

  virtual bool FinishAllSessions() noexcept = 0;

  virtual ~SessionManager() = default;
};

}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
