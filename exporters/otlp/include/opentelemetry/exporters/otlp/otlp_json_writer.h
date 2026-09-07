// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

class JsonWriter
{
public:
  JsonWriter()                              = default;
  JsonWriter(const JsonWriter &)            = delete;
  JsonWriter(JsonWriter &&)                 = delete;
  JsonWriter &operator=(const JsonWriter &) = delete;
  JsonWriter &operator=(JsonWriter &&)      = delete;
  virtual ~JsonWriter()                     = default;

  virtual void BeginObject() noexcept = 0;
  virtual void EndObject() noexcept   = 0;
  virtual void BeginArray() noexcept  = 0;
  virtual void EndArray() noexcept    = 0;

  virtual void Key(nostd::string_view key) noexcept = 0;

  virtual void WriteNull() noexcept                                            = 0;
  virtual void WriteString(nostd::string_view value) noexcept                  = 0;
  virtual void WriteInt32(std::int32_t value) noexcept                         = 0;
  virtual void WriteInt64(std::int64_t value) noexcept                         = 0;
  virtual void WriteUInt32(std::uint32_t value) noexcept                       = 0;
  virtual void WriteUInt64(std::uint64_t value) noexcept                       = 0;
  virtual void WriteDouble(double value) noexcept                              = 0;
  virtual void WriteBool(bool value) noexcept                                  = 0;
  virtual void WriteBytes(const std::uint8_t *data, std::size_t size) noexcept = 0;

  virtual bool ok() const noexcept = 0;

  virtual std::string ToString() noexcept = 0;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
