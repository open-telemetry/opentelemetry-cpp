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

#include <opentelemetry/exporters/jaeger/jaeger_exporter.h>
#include <opentelemetry/exporters/jaeger/recordable.h>
#include <thrift-gen/cpp/agent_types.h>

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

std::unique_ptr<std::trace::Recordable> JaegerExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdk::trace::Recordable>(new (nothrow) Recordable);
}

std::trace::ExportResult JaegerExporter::Export(
    const nostd::span<std::unique_ptr<std::trace::Recordable>> &spans) noexcept
{
  if (is_shutdown_)
  {
    return sdk::trace::ExportResult::kFailuer;
  }
  std::vector<thrift::Span> spans{spans};

  thrift::Batch batch;
  // batch.__set_process(_process);
  batch.__set_spans(_spanBuffer);

  sender_.EmitBatch(batch);

  return sdk::trace::ExportResult::kSuccess;
}

void JaegerExporter::InitializeEndpoint()
{
  if (options.transport_type == THRIFT_UDP)
  {
    sender_ =
        std::unique_ptr<ThriftSender>(new ThriftSender(options.server_addr, options.server_port));
  }
  else
  {
    static_assert(false, "Unsupported transport type");
  }
}

}  // namespace jaeger
}  // namespace exporter
