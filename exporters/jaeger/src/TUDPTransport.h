// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

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
