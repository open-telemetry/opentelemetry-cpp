#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/nostd/function_ref.h"
#include <vector>
#include <string>
#include <chrono>

// Usage Example
//
// struct ReponseHandler: public HttpResponseHandler {
//      void OnHttpResponse(HttpResponse& res) noexcept override
//      { 
//           if (res.IsSuccess()) 
//              .. process response
//      }
//  };
//  
//   HttpSessionManager sessiionManager; //  = getStaticInstance();
//   auto session = sessionManager.createHttpSession("localhost", 8000);
//   auto request = session->CreateRequest();
//   request->AddHeader(..);
//   ResponseHandler res_handler;
//   session->SendRequest(res_handler);
//   session->FinishSession() // optionally in the end.
//
//


OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
namespace http
{

enum class HttpMethod {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_OPTIONS,
    HTTP_HEAD,
    HTTP_PATCH,
    HTTP_DELETE
};

enum class HttpSessionState {
    CREATED,  //session object is created
    ONGOING,  // http(s) request is ongoing
    FINISHED  // https(s) reuest is finished
};

using HttpHeader = std::pair<std::string, std::string>;
using HttpBody = std::vector<uint8_t>;  
using HttpStatusCode = uint16_t;

class HttpRequest
{

    public:
    
        virtual void setMethod(HttpMethod method) noexcept = 0;

        virtual void setURI(const std::string& uri) noexcept = 0;

        virtual void SetBody( HttpBody& body ) noexcept = 0;

        virtual void AddHeader(const HttpHeader& header) noexcept = 0;

        virtual void ReplaceHeader(const HttpHeader& header) noexcept = 0;

        virtual void SetTimeoutMs(std::chrono::milliseconds timeout_ms) noexcept = 0;

};

class HttpResponse 
{
    public:
        virtual const HttpBody& GetBody() const noexcept = 0;

        virtual bool GetNextHeader(nostd::function_ref<bool(HttpHeader)> callable ) const noexcept  = 0;

        virtual bool GetNextHeader( const std::string& key, nostd::function_ref<bool(HttpHeader)> callable ) const noexcept  = 0;

        virtual HttpStatusCode GetStatusCode() const noexcept = 0;

        virtual bool IsSuccess() const noexcept = 0;

        virtual const std::string& GetErrorMessage() const noexcept = 0;

};

class HttpResponseHandler {

    public:
        virtual void OnHttpResponse(HttpResponse&) noexcept = 0;
};

class HttpSession {

    public:
        virtual std::shared_ptr<HttpRequest> CreateRequest() noexcept = 0;

        virtual void SendRequest(HttpResponseHandler&) noexcept = 0;

        virtual HttpSessionState GetSessionState() const noexcept = 0;

        virtual bool CancelSession() noexcept = 0;

        virtual bool FinishSession() noexcept = 0;

};

class HttpSessionManager {

    public:
        virtual std::shared_ptr<HttpSession> createHttpSession(std::string host, uint16_t port = 80) noexcept = 0 ;

        virtual bool CancelAllSessions() noexcept  = 0;

        virtual bool FinishAllSessions() noexcept = 0;

};
} //http
} //common
} //sdk
OPENTELEMETRY_END_NAMESPACE
