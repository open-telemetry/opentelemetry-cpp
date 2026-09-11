// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

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

// A writer distinct from the nlohmann default, just enough to show that
// injection reaches the constructor.
class StubJsonWriter : public JsonWriter
{
public:
  void BeginObject() noexcept override {}
  void EndObject() noexcept override {}
  void BeginArray() noexcept override {}
  void EndArray() noexcept override {}
  void Key(nostd::string_view) noexcept override {}
  void WriteNull() noexcept override {}
  void WriteString(nostd::string_view) noexcept override {}
  void WriteInt32(std::int32_t) noexcept override {}
  void WriteInt64(std::int64_t) noexcept override {}
  void WriteUInt32(std::uint32_t) noexcept override {}
  void WriteUInt64(std::uint64_t) noexcept override {}
  void WriteDouble(double) noexcept override {}
  void WriteBool(bool) noexcept override {}
  void WriteBytes(const std::uint8_t *, std::size_t) noexcept override {}
  bool ok() const noexcept override { return true; }
  std::string ToString() noexcept override { return "{}"; }
};

class StubJsonWriterFactory : public JsonWriterFactory
{
public:
  std::unique_ptr<JsonWriter> Create() override { return std::make_unique<StubJsonWriter>(); }
};

}  // namespace test

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
