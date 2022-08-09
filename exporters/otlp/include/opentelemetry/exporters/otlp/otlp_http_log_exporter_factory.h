// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/exporters/otlp/otlp_http_log_exporter_options.h"
#  include "opentelemetry/sdk/logs/exporter.h"

#  include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Factory class for OtlpHttpLogExporter.
 */
class OtlpHttpLogExporterFactory
{
public:
  /**
   * Create a OtlpHttpLogExporter.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogExporter> Create();

  /**
   * Create a OtlpHttpLogExporter.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogExporter> Create(
      const OtlpHttpLogExporterOptions &options);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
