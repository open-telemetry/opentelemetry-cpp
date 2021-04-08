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

#include <agent_types.h>
#include <opentelemetry/exporters/jaeger/jaeger_exporter.h>
#include <opentelemetry/exporters/jaeger/recordable.h>

#include "thrift_sender.h"
#include "udp_transport.h"

#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

JaegerExporter::JaegerExporter(const JaegerExporterOptions &options) : options_(options)
{
  InitializeEndpoint();
}

JaegerExporter::JaegerExporter() : JaegerExporter(JaegerExporterOptions()) {}

std::unique_ptr<trace_sdk::Recordable> JaegerExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdk::trace::Recordable>(new Recordable);
}

sdk::trace::ExportResult JaegerExporter::Export(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept
{
  if (is_shutdown_)
  {
    return sdk::trace::ExportResult::kFailure;
  }

  for (auto &recordable : spans)
  {
    auto rec = std::unique_ptr<Recordable>(static_cast<Recordable *>(recordable.release()));
    if (rec != nullptr)
    {
      sender_->Append(std::move(rec));
    }
  }

  // thrift::Batch batch;

  // TODO: support process.
  // batch.__set_process(process);
  // batch.__set_spans(_span_buffer);

  // sender_.EmitBatch(batch);

  return sdk::trace::ExportResult::kSuccess;
}

void JaegerExporter::InitializeEndpoint()
{
  if (options_.transport_format == TransportFormat::kThriftUdp)
  {
    auto transport = std::unique_ptr<Transport>(
        static_cast<Transport *>(new UDPTransport(options_.server_addr, options_.server_port)));
    sender_ = std::unique_ptr<ThriftSender>(new ThriftSender(std::move(transport)));
  }
  else
  {
    // The transport format is not implemented.
    assert(false);
  }
}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
