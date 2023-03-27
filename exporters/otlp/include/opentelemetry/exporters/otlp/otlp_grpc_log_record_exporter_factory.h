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

struct OtlpGrpcExporterOptions;

/**
 * Factory class for OtlpGrpcLogRecordExporter.
 */
class OPENTELEMETRY_EXPORT OtlpGrpcLogRecordExporterFactory
{
public:
  /**
   * Create a OtlpGrpcLogRecordExporter.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create();

  /**
   * Create a OtlpGrpcLogRecordExporter.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create(
      const OtlpGrpcExporterOptions &options);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif /* ENABLE_LOGS_PREVIEW */
