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

#include <agent.h>
#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include <opentelemetry/exporters/jaeger/recordable.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TBufferTransports.h>

#include "sender.h"
#include "transport.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

using namespace jaegertracing;

class ThriftSender : public Sender
{
public:
  static constexpr uint32_t kEmitBatchOverhead = 30;

  ThriftSender(std::unique_ptr<Transport> &&transport);
  ~ThriftSender() override { Close(); }

  int Append(std::unique_ptr<Recordable> &&span) noexcept override;
  int Flush() override;
  void Close() override;

private:
  void ResetBuffers()
  {
    span_buffer_.clear();
    byte_buffer_size_ = process_bytes_size_;
  }

  template <typename ThriftType>
  uint32_t CalcSizeOfSerializedThrift(const ThriftType &base)
  {
    uint8_t *data = nullptr;
    uint32_t size = 0;

    thrift_buffer_->resetBuffer();
    auto protocol = protocol_factory_->getProtocol(thrift_buffer_);
    base.write(protocol.get());
    thrift_buffer_->getBuffer(&data, &size);
    return size;
  }

private:
  std::vector<std::unique_ptr<Recordable>> spans_;
  std::vector<thrift::Span> span_buffer_;
  std::unique_ptr<Transport> transport_;
  std::unique_ptr<apache::thrift::protocol::TProtocolFactory> protocol_factory_;
  std::shared_ptr<apache::thrift::transport::TMemoryBuffer> thrift_buffer_;
  thrift::Process process_;

  // Size in bytes of the serialization buffer.
  uint32_t byte_buffer_size_   = 0;
  uint32_t process_bytes_size_ = 0;
  uint32_t max_span_bytes_     = 0;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
