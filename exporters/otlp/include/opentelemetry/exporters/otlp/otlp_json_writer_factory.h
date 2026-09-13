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

/**
 * Creates a JsonWriter for each OTLP/JSON export. Set it as
 * json_writer_factory in the runtime options of an OTLP HTTP or file exporter
 * to replace the default nlohmann-json backend.
 *
 * Create() is called once per export. One factory may be shared by several
 * exporters, and an exporter may export from several threads, so Create() can
 * be called concurrently and must be thread-safe.
 *
 * Create() must not throw: it is called from noexcept export paths, where an
 * escaping exception terminates the program. To signal a failure it returns
 * nullptr, and the export then fails.
 */
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
