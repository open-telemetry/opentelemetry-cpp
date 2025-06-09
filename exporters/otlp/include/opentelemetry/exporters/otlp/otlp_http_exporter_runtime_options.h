// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/common/thread_instrumentation.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Struct to hold OTLP HTTP traces exporter runtime options.
 */
struct OPENTELEMETRY_EXPORT OtlpHttpExporterRuntimeOptions
{
  OtlpHttpExporterRuntimeOptions()  = default;
  ~OtlpHttpExporterRuntimeOptions() = default;

  std::shared_ptr<sdk::common::ThreadInstrumentation> thread_instrumentation =
      std::shared_ptr<sdk::common::ThreadInstrumentation>(nullptr);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
