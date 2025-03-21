// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class OtlpHttpLogRecordExporterBuilder
{
public:
  OtlpHttpLogRecordExporterBuilder()          = default;
  virtual ~OtlpHttpLogRecordExporterBuilder() = default;

  virtual std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::OtlpHttpLogRecordExporterConfiguration *model)
      const = 0;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
