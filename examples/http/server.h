// Copyright 2021, OpenTelemetry Authors
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
#include <atomic>
#include <string>
#include "opentelemetry/ext/http/server/http_server.h"

namespace
{

class HttpServer : public HTTP_SERVER_NS::HttpRequestCallback
{

protected:
  HTTP_SERVER_NS::HttpServer server_;
  std::string server_url_;
  uint16_t port_;
  std::atomic<bool> is_running_{false};

public:
  HttpServer(std::string server_name = "test_server", uint16_t port = 8800) : port_(port)
  {
    server_.setServerName(server_name);
    server_.setKeepalive(false);
  }

  void AddHandler(std::string path, HTTP_SERVER_NS::HttpRequestCallback *request_handler)
  {
    server_.addHandler(path, *request_handler);
  }

  void Start()
  {
    if (!is_running_.exchange(true))
    {
      server_.addListeningPort(port_);
      server_.start();
    }
  }

  void Stop()
  {
    if (is_running_.exchange(false))
    {
      server_.stop();
    }
  }

  ~HttpServer() { Stop(); }
};

}  // namespace