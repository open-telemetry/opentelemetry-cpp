// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include <memory>

#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
class LogRecordExporter;
}  // namespace logs
}  // namespace sdk

namespace exporter
{
namespace otlp
{

struct OtlpHttpLogRecordExporterOptions;

/**
 * Factory class for OtlpHttpLogRecordExporter.
 */
class OPENTELEMETRY_EXPORT OtlpHttpLogRecordExporterFactory
{
public:
  /**
   * Create a OtlpHttpLogRecordExporter.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create();

  /**
   * Create a OtlpHttpLogRecordExporter.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create(
      const OtlpHttpLogRecordExporterOptions &options);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
