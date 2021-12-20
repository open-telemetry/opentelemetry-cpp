// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <agent_types.h>
#include <opentelemetry/exporters/jaeger/jaeger_exporter.h>
#include <opentelemetry/exporters/jaeger/recordable.h>

#include "http_transport.h"
#include "thrift_sender.h"
#include "udp_transport.h"

#include <vector>

namespace sdk_common = opentelemetry::sdk::common;
namespace trace_sdk  = opentelemetry::sdk::trace;

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
  return std::unique_ptr<sdk::trace::Recordable>(new JaegerRecordable);
}

sdk_common::ExportResult JaegerExporter::Export(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept
{
  if (is_shutdown_)
  {
    return sdk_common::ExportResult::kFailure;
  }

  std::size_t exported_size = 0;

  for (auto &recordable : spans)
  {
    auto rec =
        std::unique_ptr<JaegerRecordable>(static_cast<JaegerRecordable *>(recordable.release()));
    if (rec != nullptr)
    {
      exported_size += sender_->Append(std::move(rec));
    }
  }

  exported_size += sender_->Flush();

  if (exported_size == 0)
  {
    return sdk_common::ExportResult::kFailure;
  }

  return sdk_common::ExportResult::kSuccess;
}

void JaegerExporter::InitializeEndpoint()
{
  if (options_.transport_format == TransportFormat::kThriftUdpCompact)
  {
    // TODO: do we need support any authentication mechanism?
    auto transport = std::unique_ptr<Transport>(
        static_cast<Transport *>(new UDPTransport(options_.endpoint, options_.server_port)));
    sender_ = std::unique_ptr<ThriftSender>(new ThriftSender(std::move(transport)));
    return;
  }

  if (options_.transport_format == TransportFormat::kThriftHttp)
  {
    auto transport =
        std::unique_ptr<HttpTransport>(new HttpTransport(options_.endpoint, options_.headers));
    sender_ = std::unique_ptr<ThriftSender>(new ThriftSender(std::move(transport)));
    return;
  }

  // The transport format is not implemented.
  assert(false);
}

bool JaegerExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  is_shutdown_ = true;
  return true;
}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
