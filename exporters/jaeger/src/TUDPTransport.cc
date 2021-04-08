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

#include "TUDPTransport.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

TUDPTransport::TUDPTransport(const std::string &host, int port)
    : host_(host), port_(port), socket_(THRIFT_INVALID_SOCKET)
{}

TUDPTransport::~TUDPTransport()
{
  if (server_addr_info_)
  {
    ::freeaddrinfo(server_addr_info_);
    server_addr_info_ = nullptr;
    sockaddr_len      = 0;
  }
  close();
}

bool TUDPTransport::isOpen() const
{
  return (socket_ != THRIFT_INVALID_SOCKET);
}

void TUDPTransport::open()
{
  if (isOpen())
  {
    return;
  }

  struct addrinfo hints;
  int error;
  char port[sizeof("65535") + 1];

  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags    = AI_PASSIVE | AI_ADDRCONFIG;

  sprintf(port, "%d", port_);

  error = getaddrinfo(host_.c_str(), port, &hints, &server_addr_info_);

  if (error)
  {
    // TODO: log error
    return;
  }

  socket_      = socket(server_addr_info_->ai_family, server_addr_info_->ai_socktype,
                        server_addr_info_->ai_protocol);
  sockaddr_len = server_addr_info_->ai_addr->sa_family == AF_INET ? sizeof(struct in_addr)
                                                                  : sizeof(struct in6_addr);
}

void TUDPTransport::close()
{
  if (socket_ != THRIFT_INVALID_SOCKET)
  {
    ::THRIFT_CLOSESOCKET(socket_);
  }
  socket_ = THRIFT_INVALID_SOCKET;
}

uint32_t TUDPTransport::read(uint8_t *buf, uint32_t len)
{
  uint32_t num_read = recvfrom(socket_,
#if defined(_WIN32)
                               reinterpret_cast<char *>(buf), len, 0, server_addr_info_->ai_addr,
                               reinterpret_cast<int *>(&sockaddr_len)
#else
                               buf, len, 0, server_addr_info_->ai_addr, &sockaddr_len
#endif
  );

  return num_read;
}

void TUDPTransport::write(const uint8_t *buf, uint32_t len)
{
  sendto(socket_,
#if defined(_WIN32)
         reinterpret_cast<const char *>(buf),
#else
         buf,
#endif
         len, 0, server_addr_info_->ai_addr, sockaddr_len);
}

void TUDPTransport::flush() {}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
