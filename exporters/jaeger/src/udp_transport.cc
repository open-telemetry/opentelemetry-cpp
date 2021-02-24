// Copyright 2020, OpenTelemetry Authors
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

#include <opentelemetry/exporters/jaeger/udp_transport.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

UDPTransport::UDPTransport(const std::string &addr, uint16_t port)
{
  socket_    = std::shared_ptr<TTransport>(new TSocket(addr, port));
  socket_->open();
  transport_ = std::shared_ptr<TTransport>(new TBufferedTransport(socket_));
  protocol_  = std::shared_ptr<TProtocol>(new TBinaryProtocol(transport_));
  agent_ = std::unique_ptr<AgentClient>(new AgentClient(protocol_));
}

UDPTransport::~UDPTransport() {}

void UDPTransport::EmitBatch(const thrift::Batch &batch)
{
  agent_->emitBatch(batch);
}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
