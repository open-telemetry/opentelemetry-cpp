#pragma once
#include "opentelemetry/http/http_client.h"

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace http
{
namespace http_api = opentelemetry::http;

class HttpHeaders: http_api::HttpHeaders, std::multimap<std::string, std::string>
{

public:
    virtual void set(nostd::string_view const& name, nostd::string_view const& value) override
    {
       auto range = equal_range(name);
       auto hint = erase(range.first, range.second);
       insert(hint, std::make_pair(name, value));
    }

    virtual void add(nostd::string_view const& name, nostd::string_view const& value) override
    {
        insert(std::make_pair(name, value));
    }

    virtual nostd::string_view const& get(nostd::string_view const& name) const override
    {
        auto it = find(name);
        return (it != end()) ? it->second : m_empty;
    }

    virtual bool has(nostd::string_view const& name) const override
    {
        auto it = find(name);
        return (it != end());
    }
private:
    nostd::string_view m_empty;
};

class SimpleHttpRequest:: public http_api::IHttpRequest
{
public:
    SimpleHttpRequest(std::string const& id): _id(id), method_("GET")
    {
    }
    
    virtual nostd::string_view& GetId() const override 
    {
        return id_;
    }

    virtual void SetMethod(nostd::string_view const& method) override
    {
        method_ = method;
    }

    virtual void SetUrl(std::string const& url) override
    {
		url_ = url;
	}

    virtual HttpHeaders& GetHeaders() override
    {
        return headers_;
    }

    virtual void SetBody(const uint8_t* const body, int len) override
    {
		for (int i = 0; i < len; i++)
        {
            body_.push_back(body[i]);
        }
    }

    virtual void GetBody(uint8_t* body, int* len) override
    {
        int i = 0;
        for (auto &e: body_)
        {
            body_[i++] = e;
        }
        return body_;
    }

private:

	std::vector<uint8_t> body_;
	std::string          method_;
	std::string          id_;
	std::string          url_;

};

class SimpleHttpResponse: public IHttpResponse
{
public:
    SimpleHttpResponse(nostd::string_view const& id):
        id_(id), m
        result_(HTTP_LOCALFAULURE),
        statusCode_(0)
    {
    }

    virtual HttpResult GetResult() override
    {
        return result_;
    }

    virtual unsigned GetStatusCode() override
    {
        return statusCode_;
    }

    virtual HttpHeaders& GetHeaders() override
    {
        return headers_;
    }

    virtual void GetBody(uint8_t* body, int& len) override
    {
        int i = 0;
        for (auto &e: body_)
        {
            body_[i++] = e;
        }
        return body_;
    }
private:

    nostd::string_view id_;
    unsigned statusCode_;
    HttpHeaders headers_;
    std::vector<uint8_t> body_;
};
} //http
} //sdk 
OPENTELEMETRY_END_NAMESPACE






