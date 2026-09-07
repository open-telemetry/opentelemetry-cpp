// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/exporters/otlp/otlp_json_writer_factory.h"
#include "opentelemetry/sdk/common/thread_instrumentation.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Struct to hold OTLP FILE client runtime options.
 */
struct OtlpFileClientRuntimeOptions
{
  OtlpFileClientRuntimeOptions()                                                = default;
  virtual ~OtlpFileClientRuntimeOptions()                                       = default;
  OtlpFileClientRuntimeOptions(const OtlpFileClientRuntimeOptions &)            = default;
  OtlpFileClientRuntimeOptions &operator=(const OtlpFileClientRuntimeOptions &) = default;
  OtlpFileClientRuntimeOptions(OtlpFileClientRuntimeOptions &&)                 = default;
  OtlpFileClientRuntimeOptions &operator=(OtlpFileClientRuntimeOptions &&)      = default;

  std::shared_ptr<sdk::common::ThreadInstrumentation> thread_instrumentation =
      std::shared_ptr<sdk::common::ThreadInstrumentation>(nullptr);

  std::shared_ptr<JsonWriterFactory> json_writer_factory;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
