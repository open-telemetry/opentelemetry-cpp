// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "http_transport.h"

#include <thrift/protocol/TBinaryProtocol.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

using TBinaryProtocol = apache::thrift::protocol::TBinaryProtocol;
using TTransport      = apache::thrift::transport::TTransport;

HttpTransport::HttpTransport(std::string endpoint, ext::http::client::Headers headers)
{
  endpoint_transport_ = std::make_shared<THttpTransport>(std::move(endpoint), std::move(headers));
  protocol_           = std::shared_ptr<TProtocol>(new TBinaryProtocol(endpoint_transport_));
}

int HttpTransport::EmitBatch(const thrift::Batch &batch)
{
  batch.write(protocol_.get());

  if (!endpoint_transport_->sendSpans())
  {
    return 0;
  }

  return static_cast<int>(batch.spans.size());
}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
