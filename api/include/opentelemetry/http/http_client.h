#pragma once
OPENTELEMETRY_BEGIN_NAMESPACE
namespace http
{

enum class HttpMethod: uint8_t {
    HTTP_GET,
    HTTP_POST,
    HTTP_DELETE,
    HTTP_PUT,
    HTTP_HEAD,
    HTTP_PATCH
};

class IHttpClient
{
public:
    virtual IHttpRequest* CreateRequest() = 0;
    virtual void SendRequest (IHttpRequest* request, IHttpResponse* response) = 0;
    virtual void SendRequestAsync(IHttpRequest* request, IHttpResponseCallback* callback) = 0;
    virtual void CancelRequestAsync(nostd::string_view const& id) = 0;
    virtual void CancelAllRequests();
};

class IHttpRequest
{
public:
    virtual nostd::string_view& GetId() = 0;
    virtual void SetMethod(nostd::string_view const& method) = 0;
    virtual void SetUrl(nostd::string_view const& url) = 0;
    virtual HttpHeaders& GetHeaders() = 0;
    virtual void SetBody(uint8_t* body, int len) = 0;
    virtual void GetBody(uint8_t* body, int* len) = 0;
};

class HttpHeaders
{
public:
    virtual void set(nostd::string_view const& name, nostd::string_view const& value) = 0;
    virtual void add(nostd::string_view const& name, nostd::string_view const& value) = 0;
    virtual nostd::string_view const& get(nostd::string_view const& name) = 0;
    virtual bool has(nostd::string_view const& name) = 0;
};

class IHttpResponse
{
public:
    virtual nostd::string_view& GetId() = 0;
    virtual HttpResult GetResult() = 0;
    virtual unsigned GetStatusCode() = 0;
    virtual HttpHeaders& GetHeaders() = 0;
    virtual void GetBody(uint8_t* body, int* len) = 0;
};

class IHttpResponseCallback
{
public:
    virtual void OnHttpResponse(IHttpResponse* response) = 0;
};
} // namespace nostd
OPENTELEMETRY_END_NAMESPACE
OPENTELEMETRY_END_NAMESPACE
