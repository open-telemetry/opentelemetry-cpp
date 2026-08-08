// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/logs_builders.h"

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/logger_config_configuration.h"
#include "opentelemetry/sdk/configuration/logger_configurator_builder.h"
#include "opentelemetry/sdk/configuration/logger_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/logger_matcher_and_config_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/severity_number.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_builder.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/logs/batch_log_record_processor_factory.h"
#include "opentelemetry/sdk/logs/batch_log_record_processor_options.h"
#include "opentelemetry/sdk/logs/logger_config.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
#include "opentelemetry/version.h"
#include "src/common/wildcard_match.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

namespace
{

static opentelemetry::logs::Severity ToLogSeverity(
    opentelemetry::sdk::configuration::SeverityNumber severity_number)
{
  switch (severity_number)
  {
    case SeverityNumber::trace:
      return opentelemetry::logs::Severity::kTrace;
    case SeverityNumber::trace2:
      return opentelemetry::logs::Severity::kTrace2;
    case SeverityNumber::trace3:
      return opentelemetry::logs::Severity::kTrace3;
    case SeverityNumber::trace4:
      return opentelemetry::logs::Severity::kTrace4;
    case SeverityNumber::debug:
      return opentelemetry::logs::Severity::kDebug;
    case SeverityNumber::debug2:
      return opentelemetry::logs::Severity::kDebug2;
    case SeverityNumber::debug3:
      return opentelemetry::logs::Severity::kDebug3;
    case SeverityNumber::debug4:
      return opentelemetry::logs::Severity::kDebug4;
    case SeverityNumber::info:
      return opentelemetry::logs::Severity::kInfo;
    case SeverityNumber::info2:
      return opentelemetry::logs::Severity::kInfo2;
    case SeverityNumber::info3:
      return opentelemetry::logs::Severity::kInfo3;
    case SeverityNumber::info4:
      return opentelemetry::logs::Severity::kInfo4;
    case SeverityNumber::warn:
      return opentelemetry::logs::Severity::kWarn;
    case SeverityNumber::warn2:
      return opentelemetry::logs::Severity::kWarn2;
    case SeverityNumber::warn3:
      return opentelemetry::logs::Severity::kWarn3;
    case SeverityNumber::warn4:
      return opentelemetry::logs::Severity::kWarn4;
    case SeverityNumber::error:
      return opentelemetry::logs::Severity::kError;
    case SeverityNumber::error2:
      return opentelemetry::logs::Severity::kError2;
    case SeverityNumber::error3:
      return opentelemetry::logs::Severity::kError3;
    case SeverityNumber::error4:
      return opentelemetry::logs::Severity::kError4;
    case SeverityNumber::fatal:
      return opentelemetry::logs::Severity::kFatal;
    case SeverityNumber::fatal2:
      return opentelemetry::logs::Severity::kFatal2;
    case SeverityNumber::fatal3:
      return opentelemetry::logs::Severity::kFatal3;
    case SeverityNumber::fatal4:
      return opentelemetry::logs::Severity::kFatal4;
    default:
      break;
  }
  return opentelemetry::logs::Severity::kInvalid;
}

class DefaultBatchLogRecordProcessorBuilder : public BatchLogRecordProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> Build(
      const BatchLogRecordProcessorConfiguration *model,
      std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> &&exporter) const override
  {
    opentelemetry::sdk::logs::BatchLogRecordProcessorOptions options;
    options.schedule_delay_millis = std::chrono::milliseconds(model->schedule_delay);
    options.export_timeout_millis = std::chrono::milliseconds(model->export_timeout);
    options.max_queue_size        = model->max_queue_size;
    options.max_export_batch_size = model->max_export_batch_size;
    return opentelemetry::sdk::logs::BatchLogRecordProcessorFactory::Create(std::move(exporter),
                                                                            options);
  }
};

class DefaultSimpleLogRecordProcessorBuilder : public SimpleLogRecordProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> Build(
      const SimpleLogRecordProcessorConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> &&exporter) const override
  {
    return opentelemetry::sdk::logs::SimpleLogRecordProcessorFactory::Create(std::move(exporter));
  }
};

class DefaultLoggerConfiguratorBuilder : public LoggerConfiguratorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
      opentelemetry::sdk::logs::LoggerConfig>>
  Build(const LoggerConfiguratorConfiguration *model) const override
  {
    using common::WildcardMatch;
    using opentelemetry::sdk::instrumentationscope::InstrumentationScope;
    using opentelemetry::sdk::instrumentationscope::ScopeConfigurator;
    using opentelemetry::sdk::logs::LoggerConfig;

    LoggerConfig default_config = LoggerConfig::Create(
        model->default_config.enabled, ToLogSeverity(model->default_config.minimum_severity),
        model->default_config.trace_based);
    auto builder = ScopeConfigurator<LoggerConfig>::Builder(default_config);

    for (const auto &entry : model->loggers)
    {
      LoggerConfig entry_config =
          LoggerConfig::Create(entry.config.enabled, ToLogSeverity(entry.config.minimum_severity),
                               entry.config.trace_based);
      std::string pattern = entry.name;
      builder.AddCondition(
          [pattern](const InstrumentationScope &scope) {
            return WildcardMatch(pattern, scope.GetName());
          },
          entry_config);
    }

    return std::make_unique<ScopeConfigurator<LoggerConfig>>(builder.Build());
  }
};

}  // namespace

void RegisterDefaultLogsBuilders(Registry *registry)
{
  registry->SetBatchLogRecordProcessorBuilder(
      std::make_unique<DefaultBatchLogRecordProcessorBuilder>());
  registry->SetSimpleLogRecordProcessorBuilder(
      std::make_unique<DefaultSimpleLogRecordProcessorBuilder>());
  registry->SetLoggerConfiguratorBuilder(std::make_unique<DefaultLoggerConfiguratorBuilder>());
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
