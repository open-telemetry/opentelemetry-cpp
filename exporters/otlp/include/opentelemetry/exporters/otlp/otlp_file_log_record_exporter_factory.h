// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/version.h"
#include "opentelemetry/exporters/otlp/otlp_file_log_record_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_file_log_record_exporter_runtime_options.h"
#include "opentelemetry/sdk/logs/exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Factory class for OtlpFileExporter.
 */
class OPENTELEMETRY_EXPORT_TYPE OtlpFileLogRecordExporterFactory
{
public:
  /**
   * Create an OtlpFileExporter using all default options.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create();

  /**
   * Create an OtlpFileExporter using the given options.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create(
      const OtlpFileLogRecordExporterOptions &options);

  /**
   * Create an OtlpFileExporter using the given options.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create(
      const OtlpFileLogRecordExporterOptions &options,
      const OtlpFileLogRecordExporterRuntimeOptions &runtime_options);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
