// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/exporters/otlp/otlp_json_writer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

class OPENTELEMETRY_EXPORT JsonWriterFactory
{
public:
  JsonWriterFactory()                                     = default;
  JsonWriterFactory(const JsonWriterFactory &)            = delete;
  JsonWriterFactory(JsonWriterFactory &&)                 = delete;
  JsonWriterFactory &operator=(const JsonWriterFactory &) = delete;
  JsonWriterFactory &operator=(JsonWriterFactory &&)      = delete;
  virtual ~JsonWriterFactory()                            = default;

  virtual std::unique_ptr<JsonWriter> Create() = 0;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
