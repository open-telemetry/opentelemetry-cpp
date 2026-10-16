// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>

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

// A writer distinct from the nlohmann default, just enough to show that injection reaches the
// constructor. Each method calls the matching on_ member, so a test replaces only the behavior it
// needs. By default the tokens are ignored, ok() returns true and ToString() returns "{}".
class StubJsonWriter : public JsonWriter
{
public:
  void BeginObject() noexcept override { on_begin_object(); }
  void EndObject() noexcept override { on_end_object(); }
  void BeginArray() noexcept override { on_begin_array(); }
  void EndArray() noexcept override { on_end_array(); }
  void Key(nostd::string_view key) noexcept override { on_key(key); }
  void WriteNull() noexcept override { on_write_null(); }
  void WriteString(nostd::string_view value) noexcept override { on_write_string(value); }
  void WriteInt32(std::int32_t value) noexcept override { on_write_int32(value); }
  void WriteInt64(std::int64_t value) noexcept override { on_write_int64(value); }
  void WriteUInt32(std::uint32_t value) noexcept override { on_write_uint32(value); }
  void WriteUInt64(std::uint64_t value) noexcept override { on_write_uint64(value); }
  void WriteDouble(double value) noexcept override { on_write_double(value); }
  void WriteBool(bool value) noexcept override { on_write_bool(value); }
  void WriteBytes(const std::uint8_t *data, std::size_t size) noexcept override
  {
    on_write_bytes(data, size);
  }
  bool ok() const noexcept override { return on_ok(); }
  std::string ToString() noexcept override { return on_to_string(); }

  std::function<void()> on_begin_object                                 = [] {};
  std::function<void()> on_end_object                                   = [] {};
  std::function<void()> on_begin_array                                  = [] {};
  std::function<void()> on_end_array                                    = [] {};
  std::function<void(nostd::string_view)> on_key                        = [](nostd::string_view) {};
  std::function<void()> on_write_null                                   = [] {};
  std::function<void(nostd::string_view)> on_write_string               = [](nostd::string_view) {};
  std::function<void(std::int32_t)> on_write_int32                      = [](std::int32_t) {};
  std::function<void(std::int64_t)> on_write_int64                      = [](std::int64_t) {};
  std::function<void(std::uint32_t)> on_write_uint32                    = [](std::uint32_t) {};
  std::function<void(std::uint64_t)> on_write_uint64                    = [](std::uint64_t) {};
  std::function<void(double)> on_write_double                           = [](double) {};
  std::function<void(bool)> on_write_bool                               = [](bool) {};
  std::function<void(const std::uint8_t *, std::size_t)> on_write_bytes = [](const std::uint8_t *,
                                                                             std::size_t) {};
  std::function<bool()> on_ok                                           = [] { return true; };
  std::function<std::string()> on_to_string = [] { return std::string("{}"); };
};

// Creates writers with the given function, so a test can inject any writer, or nullptr.
class StubJsonWriterFactory : public JsonWriterFactory
{
public:
  explicit StubJsonWriterFactory(std::function<std::unique_ptr<JsonWriter>()> create =
                                     [] { return std::make_unique<StubJsonWriter>(); })
      : create_(std::move(create))
  {}

  std::unique_ptr<JsonWriter> Create() override { return create_(); }

private:
  std::function<std::unique_ptr<JsonWriter>()> create_;
};

}  // namespace test

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
