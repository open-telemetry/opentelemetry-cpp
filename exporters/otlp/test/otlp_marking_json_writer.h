// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "opentelemetry/exporters/otlp/detail/default_json_writer_factory.h"
#include "opentelemetry/exporters/otlp/otlp_json_writer.h"
#include "opentelemetry/exporters/otlp/otlp_json_writer_factory.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace test
{

// Wraps the default writer and marks its output, so a test can tell the
// injected writer ran by looking at the emitted bytes alone.
class MarkingJsonWriter : public JsonWriter
{
public:
  MarkingJsonWriter() : inner_(detail::GetDefaultJsonWriterFactory()->Create()) {}

  void BeginObject() noexcept override { inner_->BeginObject(); }
  void EndObject() noexcept override { inner_->EndObject(); }
  void BeginArray() noexcept override { inner_->BeginArray(); }
  void EndArray() noexcept override { inner_->EndArray(); }
  void Key(nostd::string_view key) noexcept override { inner_->Key(key); }
  void WriteNull() noexcept override { inner_->WriteNull(); }
  void WriteString(nostd::string_view value) noexcept override { inner_->WriteString(value); }
  void WriteInt32(std::int32_t value) noexcept override { inner_->WriteInt32(value); }
  void WriteInt64(std::int64_t value) noexcept override { inner_->WriteInt64(value); }
  void WriteUInt32(std::uint32_t value) noexcept override { inner_->WriteUInt32(value); }
  void WriteUInt64(std::uint64_t value) noexcept override { inner_->WriteUInt64(value); }
  void WriteDouble(double value) noexcept override { inner_->WriteDouble(value); }
  void WriteBool(bool value) noexcept override { inner_->WriteBool(value); }
  void WriteBytes(const std::uint8_t *data, std::size_t size) noexcept override
  {
    inner_->WriteBytes(data, size);
  }
  bool ok() const noexcept override { return inner_->ok(); }
  std::string ToString() noexcept override { return "/*custom-writer*/" + inner_->ToString(); }

private:
  std::unique_ptr<JsonWriter> inner_;
};

class MarkingJsonWriterFactory : public JsonWriterFactory
{
public:
  std::unique_ptr<JsonWriter> Create() override { return std::make_unique<MarkingJsonWriter>(); }
};

}  // namespace test

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
