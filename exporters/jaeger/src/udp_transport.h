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

#include "TUDPTransport.h"
#include "transport.h"

#include <agent.h>
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
using TSocket            = apache::thrift::transport::TSocket;
using TTransport         = apache::thrift::transport::TTransport;

class UDPTransport : public Transport
{
public:
  static constexpr auto kUDPPacketMaxLength = 65000;

  UDPTransport(const std::string &addr, uint16_t port);
  virtual ~UDPTransport();

  void EmitBatch(const thrift::Batch &batch) override;

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
