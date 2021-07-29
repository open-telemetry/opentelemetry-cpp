#pragma once

#include "THttpTransport.h"
#include "transport.h"

#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>
#include <memory>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

using TProtocol = apache::thrift::protocol::TProtocol;

class HttpTransport : public Transport
{
public:
  HttpTransport(std::string endpoint, ext::http::client::Headers headers);

  int EmitBatch(const thrift::Batch &batch) override;

  uint32_t MaxPacketSize() const override
  {
    // Default to 4 MiB POST body size.
    return 1 << 22;
  }

private:
  std::shared_ptr<THttpTransport> endpoint_transport_;
  std::shared_ptr<TProtocol> protocol_;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
