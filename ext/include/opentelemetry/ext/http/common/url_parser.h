// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace http
{
namespace common
{
// http://user:password@host:port/path1/path2?key1=val2&key2=val2
// http://host:port/path1/path2?key1=val1&key2=val2
// host:port/path1
// host:port ( path defaults to "/")
// host:port?

class UrlParser
{
public:
  std::string url_;
  std::string host_;
  std::string scheme_;
  std::string path_;
  uint16_t port_;
  std::string query_;
  bool success_;

  UrlParser(std::string url) : url_(url), success_(true)
  {
    if (url_.length() == 0)
    {
      return;
    }
    size_t cpos = 0;
    // scheme
    size_t pos = url_.find("://", cpos);
    if (pos == std::string::npos)
    {
      // scheme missing, use default as http
      scheme_ = "http";
    }
    else
    {
      scheme_ = std::string(url_.begin() + cpos, url_.begin() + pos);
      cpos    = pos + 3;
    }

    // credentials
    pos = url_.find_first_of("@", cpos);
    if (pos != std::string::npos)
    {
      // TODO - handle credentials
      cpos = pos + 1;
    }
    pos          = url_.find_first_of(":", cpos);
    bool is_port = false;
    if (pos == std::string::npos)
    {
      // port missing. Used default 80 / 443
      if (scheme_ == "http")
        port_ = 80;
      if (scheme_ == "https")
        port_ = 443;
    }
    else
    {
      // port present
      is_port = true;
      host_   = std::string(url_.begin() + cpos, url_.begin() + pos);
      cpos    = pos + 1;
    }
    pos = url_.find_first_of("/?", cpos);
    if (pos == std::string::npos)
    {
      path_ = "/";  // use default path
      if (is_port)
      {
        port_ = static_cast<uint16_t>(
            std::stoi(std::string(url_.begin() + cpos, url_.begin() + url_.length())));
      }
      else
      {
        host_ = std::string(url_.begin() + cpos, url_.begin() + url_.length());
      }
      return;
    }
    if (is_port)
    {
      port_ =
          static_cast<uint16_t>(std::stoi(std::string(url_.begin() + cpos, url_.begin() + pos)));
    }
    else
    {
      host_ = std::string(url_.begin() + cpos, url_.begin() + pos);
    }
    cpos = pos;

    if (url_[cpos] == '/')
    {
      pos = url_.find('?', cpos);
      if (pos == std::string::npos)
      {
        path_  = std::string(url_.begin() + cpos, url_.begin() + url_.length());
        query_ = "";
      }
      else
      {
        path_  = std::string(url_.begin() + cpos, url_.begin() + pos);
        cpos   = pos + 1;
        query_ = std::string(url_.begin() + cpos, url_.begin() + url_.length());
      }
      return;
    }
    path_ = "/";
    if (url_[cpos] == '?')
    {
      query_ = std::string(url_.begin() + cpos, url_.begin() + url_.length());
    }
  }
};

}  // namespace common

}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE