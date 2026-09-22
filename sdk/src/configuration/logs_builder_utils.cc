// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/logs_builder_utils.h"

#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/logger_configurator_builder.h"
#include "opentelemetry/sdk/configuration/logger_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/logger_config.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

namespace
{

static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> CreateOtlpHttpLogRecordExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::OtlpHttpLogRecordExporterConfiguration *model)
{
  const OtlpHttpLogRecordExporterBuilder *builder = registry->GetOtlpHttpLogRecordBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpHttpLogRecordExporter() using registered http builder");
    return builder->Build(model);
  }
  static const std::string message("No http builder for OtlpLogRecordExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> CreateOtlpGrpcLogRecordExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::OtlpGrpcLogRecordExporterConfiguration *model)
{
  const OtlpGrpcLogRecordExporterBuilder *builder = registry->GetOtlpGrpcLogRecordBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpGrpcLogRecordExporter() using registered grpc builder");
    return builder->Build(model);
  }
  static const std::string message("No grpc builder for OtlpLogRecordExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> CreateOtlpFileLogRecordExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::OtlpFileLogRecordExporterConfiguration *model)
{
  const OtlpFileLogRecordExporterBuilder *builder = registry->GetOtlpFileLogRecordBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateOtlpFileLogRecordExporter() using registered file builder");
    return builder->Build(model);
  }
  static const std::string message("No file builder for OtlpLogRecordExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> CreateConsoleLogRecordExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ConsoleLogRecordExporterConfiguration *model)
{
  const ConsoleLogRecordExporterBuilder *builder = registry->GetConsoleLogRecordBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateConsoleLogRecordExporter() using registered builder");
    return builder->Build(model);
  }
  static const std::string message("No builder for ConsoleLogRecordExporter");
  throw UnsupportedException(message);
}

static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
CreateExtensionLogRecordExporter(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ExtensionLogRecordExporterConfiguration *model)
{
  std::string name = model->name;
  const ExtensionLogRecordExporterBuilder *builder =
      registry->GetExtensionLogRecordExporterBuilder(name);
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionLogRecordExporter() using registered builder "
                            << opentelemetry::nostd::string_view(name));
    return builder->Build(model);
  }
  std::string message("CreateExtensionLogRecordExporter() no builder for ");
  message.append(name);
  throw UnsupportedException(message);
}

class LogRecordExporterDispatcher
    : public opentelemetry::sdk::configuration::LogRecordExporterConfigurationVisitor
{
public:
  explicit LogRecordExporterDispatcher(const Registry *registry) : registry_(registry) {}
  LogRecordExporterDispatcher(LogRecordExporterDispatcher &&)                      = delete;
  LogRecordExporterDispatcher(const LogRecordExporterDispatcher &)                 = delete;
  LogRecordExporterDispatcher &operator=(LogRecordExporterDispatcher &&)           = delete;
  LogRecordExporterDispatcher &operator=(const LogRecordExporterDispatcher &other) = delete;
  ~LogRecordExporterDispatcher() override                                          = default;

  void VisitOtlpHttp(const opentelemetry::sdk::configuration::OtlpHttpLogRecordExporterConfiguration
                         *model) override
  {
    exporter = CreateOtlpHttpLogRecordExporter(registry_, model);
  }

  void VisitOtlpGrpc(const opentelemetry::sdk::configuration::OtlpGrpcLogRecordExporterConfiguration
                         *model) override
  {
    exporter = CreateOtlpGrpcLogRecordExporter(registry_, model);
  }

  void VisitOtlpFile(const opentelemetry::sdk::configuration::OtlpFileLogRecordExporterConfiguration
                         *model) override
  {
    exporter = CreateOtlpFileLogRecordExporter(registry_, model);
  }

  void VisitConsole(const opentelemetry::sdk::configuration::ConsoleLogRecordExporterConfiguration
                        *model) override
  {
    exporter = CreateConsoleLogRecordExporter(registry_, model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionLogRecordExporterConfiguration *model)
      override
  {
    exporter = CreateExtensionLogRecordExporter(registry_, model);
  }

  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> exporter;

private:
  const Registry *registry_;
};

static std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>
CreateExtensionLogRecordProcessor(
    const Registry *registry,
    const opentelemetry::sdk::configuration::ExtensionLogRecordProcessorConfiguration *model)
{
  std::string name = model->name;
  const ExtensionLogRecordProcessorBuilder *builder =
      registry->GetExtensionLogRecordProcessorBuilder(name);
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateExtensionLogRecordProcessor() using registered builder "
                            << opentelemetry::nostd::string_view(name));
    return builder->Build(model);
  }
  std::string message("CreateExtensionLogRecordProcessor() no builder for ");
  message.append(name);
  throw UnsupportedException(message);
}

class LogRecordProcessorDispatcher
    : public opentelemetry::sdk::configuration::LogRecordProcessorConfigurationVisitor
{
public:
  explicit LogRecordProcessorDispatcher(const Registry *registry) : registry_(registry) {}
  LogRecordProcessorDispatcher(LogRecordProcessorDispatcher &&)                      = delete;
  LogRecordProcessorDispatcher(const LogRecordProcessorDispatcher &)                 = delete;
  LogRecordProcessorDispatcher &operator=(LogRecordProcessorDispatcher &&)           = delete;
  LogRecordProcessorDispatcher &operator=(const LogRecordProcessorDispatcher &other) = delete;
  ~LogRecordProcessorDispatcher() override                                           = default;

  void VisitBatch(
      const opentelemetry::sdk::configuration::BatchLogRecordProcessorConfiguration *model) override
  {
    auto exporter_sdk = LogsBuilderUtils::CreateLogRecordExporter(registry_, model->exporter);
    const BatchLogRecordProcessorBuilder *builder = registry_->GetBatchLogRecordProcessorBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("CreateBatchLogRecordProcessor() using registered builder");
      processor = builder->Build(model, std::move(exporter_sdk));
      return;
    }
    static const std::string message("No builder for BatchLogRecordProcessor");
    throw UnsupportedException(message);
  }

  void VisitSimple(const opentelemetry::sdk::configuration::SimpleLogRecordProcessorConfiguration
                       *model) override
  {
    auto exporter_sdk = LogsBuilderUtils::CreateLogRecordExporter(registry_, model->exporter);
    const SimpleLogRecordProcessorBuilder *builder =
        registry_->GetSimpleLogRecordProcessorBuilder();
    if (builder != nullptr)
    {
      OTEL_INTERNAL_LOG_DEBUG("CreateSimpleLogRecordProcessor() using registered builder");
      processor = builder->Build(model, std::move(exporter_sdk));
      return;
    }
    static const std::string message("No builder for SimpleLogRecordProcessor");
    throw UnsupportedException(message);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionLogRecordProcessorConfiguration *model)
      override
  {
    processor = CreateExtensionLogRecordProcessor(registry_, model);
  }

  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> processor;

private:
  const Registry *registry_;
};

}  // namespace

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
LogsBuilderUtils::CreateLogRecordExporter(
    const Registry *registry,
    const std::unique_ptr<LogRecordExporterConfiguration> &model)
{
  LogRecordExporterDispatcher dispatcher(registry);
  model->Accept(&dispatcher);
  return std::move(dispatcher.exporter);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>
LogsBuilderUtils::CreateLogRecordProcessor(
    const Registry *registry,
    const std::unique_ptr<LogRecordProcessorConfiguration> &model)
{
  LogRecordProcessorDispatcher dispatcher(registry);
  model->Accept(&dispatcher);
  return std::move(dispatcher.processor);
}

std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
    opentelemetry::sdk::logs::LoggerConfig>>
LogsBuilderUtils::CreateLoggerConfigurator(
    const Registry *registry,
    const std::unique_ptr<LoggerConfiguratorConfiguration> &model)
{
  const LoggerConfiguratorBuilder *builder = registry->GetLoggerConfiguratorBuilder();
  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("CreateLoggerConfigurator() using registered builder");
    return builder->Build(model.get());
  }
  static const std::string message("No builder for LoggerConfigurator");
  throw UnsupportedException(message);
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
