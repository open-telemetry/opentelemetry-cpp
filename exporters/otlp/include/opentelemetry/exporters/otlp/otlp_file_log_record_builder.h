// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/init/otlp_file_log_record_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

class OPENTELEMETRY_EXPORT OtlpFileLogRecordBuilder
    : public opentelemetry::sdk::init::OtlpFileLogRecordExporterBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  OtlpFileLogRecordBuilder()           = default;
  ~OtlpFileLogRecordBuilder() override = default;

  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::OtlpFileLogRecordExporterConfiguration *model)
      const override;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
