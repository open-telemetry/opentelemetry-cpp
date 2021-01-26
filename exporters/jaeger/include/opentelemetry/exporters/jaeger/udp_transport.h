
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

#pragma once

#include "transport.h"

#include <thrift-gen/cpp/agent.h>
#include <thrift/transport/TSocket.h>
#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

namespace AgentClient = jagertracing::agent::thrift::AgentClient;

class UDPTransport : public Transport
{
  pubilc : UDPTransport(const std::string &addr, uint16_t port);
  ~UDPTransport();

  void EmitBatch(const thrift::Batch &batch) override;

private:
  std::unique_ptr<AgentClient> agent_;
  std::shared_ptr<TTransport> socket_;
  std::shared_ptr<TTransport> transport_;
  std::shared_ptr<TTransport> protocol_;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
