// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TUDPTransport.h"
#include "transport.h"

#include <Agent.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransport.h>
#include <memory>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

using AgentClient        = jaegertracing::agent::thrift::AgentClient;
using TBinaryProtocol    = apache::thrift::protocol::TBinaryProtocol;
using TCompactProtocol   = apache::thrift::protocol::TCompactProtocol;
using TBufferedTransport = apache::thrift::transport::TBufferedTransport;
using TProtocol          = apache::thrift::protocol::TProtocol;
using TTransport         = apache::thrift::transport::TTransport;

class UDPTransport : public Transport
{
public:
  static constexpr auto kUDPPacketMaxLength = 65000;

  UDPTransport(const std::string &addr, uint16_t port);
  virtual ~UDPTransport();

  int EmitBatch(const thrift::Batch &batch) override;

  uint32_t MaxPacketSize() const override { return max_packet_size_; }

  void InitSocket();
  void CleanSocket();

private:
  std::unique_ptr<AgentClient> agent_;
  std::shared_ptr<TTransport> endpoint_transport_;
  std::shared_ptr<TTransport> transport_;
  std::shared_ptr<TProtocol> protocol_;
  uint32_t max_packet_size_;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
