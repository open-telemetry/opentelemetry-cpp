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

#include "thrift_sender.h"
#include "udp_transport.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

using namespace jaegertracing;

ThriftSender::ThriftSender(std::unique_ptr<Transport> &&transport)
    : transport_(std::move(transport)),
      protocol_factory_(new apache::thrift::protocol::TCompactProtocolFactory()),
      thrift_buffer_(new apache::thrift::transport::TMemoryBuffer())
{}

int ThriftSender::Append(std::unique_ptr<Recordable> &&span) noexcept
{
  if (span == nullptr)
  {
    return 0;
  }

  uint32_t max_span_bytes = transport_->MaxPacketSize() - kEmitBatchOverhead;
  if (process_.serviceName.empty())
  {
    // TODO: populate Span.Process from OpenTelemetry resources.
    // process_byte_size_ = CalcSizeOfSerializedThrift(process_);
    // max_span_bytes -= process_byte_size_;
  }

  auto jaeger_span = span->Span();

  const uint32_t span_size = CalcSizeOfSerializedThrift(*jaeger_span.get());
  if (span_size > max_span_bytes)
  {
    // TODO, handle too large span.
    return 0;
  }

  byte_buffer_size_ += span_size;
  if (byte_buffer_size_ <= max_span_bytes)
  {
    span_buffer_.push_back(*jaeger_span.release());
    if (byte_buffer_size_ < max_span_bytes)
    {
      return 0;
    }
    else
    {
      // byte buffer is full so flush it before appending new span.
      return Flush();
    }
  }

  const auto flushed = Flush();
  span_buffer_.push_back(*jaeger_span.release());
  byte_buffer_size_ = span_size + process_bytes_size_;

  return flushed;
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

  transport_->EmitBatch(batch);

  ResetBuffers();

  return static_cast<int>(batch.spans.size());
}

void ThriftSender::Close() {}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
