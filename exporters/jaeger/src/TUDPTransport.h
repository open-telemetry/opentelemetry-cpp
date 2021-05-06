// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef _WIN32
#  include <winsock2.h>
#else
#  include <netdb.h>
#  include <string.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#endif

#include <opentelemetry/version.h>
#include <thrift/transport/PlatformSocket.h>
#include <thrift/transport/TVirtualTransport.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

class TUDPTransport : public apache::thrift::transport::TVirtualTransport<TUDPTransport>
{
public:
  TUDPTransport(const std::string &host, int port);
  ~TUDPTransport() override;

  bool isOpen() const override;

  void open() override;

  void close() override;

  uint32_t read(uint8_t *buf, uint32_t len);

  void write(const uint8_t *buf, uint32_t len);

  void flush() override;

private:
  std::string host_;
  int port_;
  THRIFT_SOCKET socket_;
  struct addrinfo *server_addr_info_ = nullptr;
  uint32_t sockaddr_len              = 0;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
