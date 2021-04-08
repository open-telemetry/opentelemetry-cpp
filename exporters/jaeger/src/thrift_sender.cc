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

#include <opentelemetry/exporters/jaeger/thrift_sender.h>

#include "udp_transport.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

using namespace jaegertracing;

ThriftSender::ThriftSender(std::unique_ptr<Transport> &&transport)
    : transport_(std::move(transport))
{}

bool ThriftSender::Append(std::unique_ptr<Recordable> &&span) noexcept
{
  if (span == nullptr)
  {
    return false;
  }

  if (process_.serviceName.empty())
  {
    // TODO: populate Span.Process from OpenTelemetry resources.
  }

  thrift::Batch batch;
  std::vector<thrift::Span> span_vec;
  span_vec.push_back(span.release()->span());
  transport_->EmitBatch(batch);

  return true;
}

int ThriftSender::Flush()
{
  if (span_buffer_.empty())
  {
    return 0;
  }

  thrift::Batch batch;
  batch.__set_process(process_);
  batch.__set_spans(span_buffer_);

  try
  {
    transport_->EmitBatch(batch);
  }
  catch (const std::system_error &)
  {}
  catch (const std::exception &)
  {}
  catch (...)
  {}

  return static_cast<int>(batch.spans.size());
}

void ThriftSender::Close()
{
  // transport_->close();
}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
