// Copyright 2020, OpenTelemetry Authors
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

#include <stdio.h>
#include <iterator>
#include <string>
#include <vector>
#include "curl/curl.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace HTTP_SERVER_NS
{

// Because libcurl global init is not thread safe, at most one HttpClients object
// should be standing at the same time only.
class HttpClients
{
public:
  // Intialize the environment for all clients
  HttpClients();

  // Stop libcurl
  ~HttpClients();

  class HttpClient
  {
  public:
    HttpClient();

    // clean up the curl
    ~HttpClient();

    // Sending the request stored and return true if communication successful,
    // and false otherwise.
    bool SendRequest(std::string url);

    // Set the headers of the post request to be transmitted.
    void SetHeaders(std::vector<std::string> headers);

    // Set the POST fields of the post request to be transmitted, need to percent-encode '=' sign
    // in trace state to "%3D"
    void AddPostField(std::string post_field_name, std::string post_field_value);

  private:
    CURL *curl;
    CURLcode res;
    struct curl_slist *list = NULL;
    std::string fields;
  };

  HttpClient StartNewClient();
};
}  // namespace HTTP_SERVER_NS
OPENTELEMETRY_END_NAMESPACE
