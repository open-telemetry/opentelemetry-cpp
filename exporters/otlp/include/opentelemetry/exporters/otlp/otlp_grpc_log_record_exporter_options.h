// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_client_options.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Struct to hold OTLP GRPC log record exporter options.
 *
 * See
 * https://github.com/open-telemetry/opentelemetry-proto/blob/main/docs/specification.md#otlpgrpc
 *
 * See
 * https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/protocol/exporter.md
 */
struct OPENTELEMETRY_EXPORT OtlpGrpcLogRecordExporterOptions : public OtlpGrpcClientOptions
{
  OtlpGrpcLogRecordExporterOptions();
  OtlpGrpcLogRecordExporterOptions(const OtlpGrpcLogRecordExporterOptions &)            = default;
  OtlpGrpcLogRecordExporterOptions(OtlpGrpcLogRecordExporterOptions &&)                 = default;
  OtlpGrpcLogRecordExporterOptions &operator=(const OtlpGrpcLogRecordExporterOptions &) = default;
  OtlpGrpcLogRecordExporterOptions &operator=(OtlpGrpcLogRecordExporterOptions &&)      = default;
  ~OtlpGrpcLogRecordExporterOptions() override;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
