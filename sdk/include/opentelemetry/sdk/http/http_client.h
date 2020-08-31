#pragma once
#include "opentelemetry/http/http_client.h"
#include "opentelemetry/version.h"

#include <map>
#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace http
{
namespace http_api = opentelemetry::http;

// HttpHeaders implementation
class HttpHeaders: http_api::HttpHeaders, std::multimap<std::string, std::string>
{

public:
    virtual void set(nostd::string_view const& name, nostd::string_view const& value) override
    {
       auto range = equal_range(std::string(name.data()));
       auto hint = erase(range.first, range.second);
       insert(hint, std::make_pair(name, value));
    }

    virtual void add(nostd::string_view const& name, nostd::string_view const& value) override
    {
        insert(std::make_pair(name, value));
    }

    virtual nostd::string_view const& get(nostd::string_view const& name) const override
    {
        auto it = find(std::string(name.data()));
        return (it != end()) ? it->second : m_empty;
    }

    virtual bool has(nostd::string_view const& name) const override
    {
        auto it = find(std::string(name.data()));
        return (it != end());
    }

private:

    nostd::string_view m_empty{};
};

class SimpleHttpRequest: public http_api::HttpRequest
{
public:

    SimpleHttpRequest(nostd::string_view const& id): id_(id), method_("GET")
    {
    }
   
    // Gets the HTTP request ID.
    virtual const nostd::string_view& GetId() const override 
    {
        return id_;
    }

    // Sets the request method   
    virtual void SetMethod(nostd::string_view const& method) override
    {
        method_ = method;
    }

    // Sets the HTTP request URI.        
    virtual void SetUrl(nostd::string_view const& url) override
    {
		url_ = url;
	}

    // Gets the HTTP request headers.
    virtual http_api::HttpHeaders* GetHeaders() const override
    {
        return headers_;
    }

    // Sets the request body.
    virtual void SetBody(const uint8_t* const body, const size_t len) override
    {
		for (int i = 0; i < len; i++)
        {
            body_.push_back(body[i]);
        }
    }

    virtual void GetBody(uint8_t* body, size_t& len) override
    {
        len = 0;
        for (auto &e: body_)
        {
            body[len++] = e;
        }
        len = (len > 0 ) ? len - 1 :  len ;
    }

private:
    http_api::HttpHeaders* headers_;
	std::vector<uint8_t> body_;
	nostd::string_view          method_;
	nostd::string_view          id_;
	nostd::string_view          url_;


};

class SimpleHttpResponse: public http_api::HttpResponse
{
public:
    SimpleHttpResponse(nostd::string_view const& id):
        id_(id),
        result_(http_api::HttpResult::HttpResult_LocalFailure),
        statusCode_(0)
    {
    }

    virtual http_api::HttpResult GetResult() override
    {
        return result_;
    }

    virtual unsigned GetStatusCode() override
    {
        return statusCode_;
    }

    virtual http_api::HttpHeaders* GetHeaders() override
    {
        return headers_;
    }

    virtual void GetBody(uint8_t* body, size_t& len) override
    {
        int i = 0;
        for (auto &e: body_)
        {
            body_[i++] = e;
        }
    }
private:

    nostd::string_view id_;
    unsigned statusCode_;
    http_api::HttpHeaders* headers_;
    http_api::HttpResult result_;
    std::vector<uint8_t> body_;
};
} //http
} //sdk 
OPENTELEMETRY_END_NAMESPACE
