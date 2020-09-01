// Copyright The OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include "opentelemetry/http/http_client.h"
#include "opentelemetry/version.h"

#include <algorithm>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace http
{
namespace http_api = opentelemetry::http;

static std::function<bool(const std::string &, const std::string &)> CaseInsensitiveComparator =
    [](const std::string &s1, const std::string &s2) -> bool {
  std::string str1(s1.length(), ' ');
  std::string str2(s2.length(), ' ');
  auto lowerCase = [](char c) -> char { return tolower(c); };
  std::transform(s1.begin(), s1.end(), str1.begin(), lowerCase);
  std::transform(s2.begin(), s2.end(), str2.begin(), lowerCase);
  return str1 < str2;
};

// HttpHeaders implementation
class HttpHeaders : http_api::HttpHeaders,
                    std::multimap<std::string, std::string, decltype(CaseInsensitiveComparator)>
{
public:
  virtual void set(nostd::string_view const &name, nostd::string_view const &value) override
  {
    auto range = equal_range(std::string(name.data()));
    auto hint  = erase(range.first, range.second);
    insert(hint, std::make_pair(name, value));
  }

  virtual void add(nostd::string_view const &name, nostd::string_view const &value) override
  {
    insert(std::make_pair(name, value));
  }

  virtual nostd::string_view const &get(nostd::string_view const &name) const override
  {
    auto it = find(std::string(name.data()));
    return (it != end()) ? it->second : m_empty;
  }

  virtual bool contains(nostd::string_view const &name) const override
  {
    auto it = find(std::string(name.data()));
    return (it != end());
  }

private:
  nostd::string_view m_empty{};
};

class SimpleHttpRequest : public http_api::HttpRequest
{

public:
  SimpleHttpRequest(nostd::string_view const &id) : id_(id), method_("GET") {}

  // Gets the HTTP request ID.
  virtual const nostd::string_view &GetId() const override { return id_; }

  // Sets the request method
  virtual void SetMethod(nostd::string_view const &method) override { method_ = method; }

  // Sets the HTTP request URI.
  virtual void SetUrl(nostd::string_view const &url) override { url_ = url; }

  // Gets the HTTP request headers.
  virtual http_api::HttpHeaders &GetHeaders() const override { return *headers_; }

  // Sets the request body.
  virtual void SetBody(const uint8_t *const body, const size_t len) override
  {
    for (int i = 0; i < len; i++)
    {
      body_.push_back(body[i]);
    }
  }

  virtual void GetBody(uint8_t *body, size_t &len) override
  {
    len = 0;
    for (auto &e : body_)
    {
      body[len++] = e;
    }
    len = (len > 0) ? len - 1 : len;
  }

private:
  std::unique_ptr<http_api::HttpHeaders> headers_;
  std::vector<uint8_t> body_;
  nostd::string_view method_;
  nostd::string_view id_;
  nostd::string_view url_;
};

class SimpleHttpResponse : public http_api::HttpResponse
{

public:
  SimpleHttpResponse(nostd::string_view const &id)
      : id_(id), result_(http_api::HttpResult::HttpResult_LocalFailure), statusCode_(0)
  {}

  virtual http_api::HttpResult GetResult() override { return result_; }

  virtual unsigned GetStatusCode() override { return statusCode_; }

  virtual const http_api::HttpHeaders &GetHeaders() override { return *headers_; }

  virtual void GetBody(uint8_t *body, size_t &len) override
  {
    int i = 0;
    for (auto &e : body_)
    {
      body_[i++] = e;
    }
  }

private:
  nostd::string_view id_;
  unsigned statusCode_;
  std::unique_ptr<http_api::HttpHeaders> headers_;
  http_api::HttpResult result_;
  std::vector<uint8_t> body_;
};
}  // namespace http
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
