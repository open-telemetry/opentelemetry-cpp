// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/logs/logger.h"
#include "opentelemetry/logs/logger_provider.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/logs/severity.h"

#include "opentelemetry/sdk/configuration/batch_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configured_sdk.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/logger_config_configuration.h"
#include "opentelemetry/sdk/configuration/logger_configurator_builder.h"
#include "opentelemetry/sdk/configuration/logger_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/logger_matcher_and_config_configuration.h"
#include "opentelemetry/sdk/configuration/logger_provider_configuration.h"
#include "opentelemetry/sdk/configuration/logs_builders.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/sdk_builder.h"
#include "opentelemetry/sdk/configuration/severity_number.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/logger_config.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/processor.h"

#include "config_test_logs.h"

namespace logs       = opentelemetry::logs;
namespace logs_sdk   = opentelemetry::sdk::logs;
namespace scope_sdk  = opentelemetry::sdk::instrumentationscope;
namespace config_sdk = opentelemetry::sdk::configuration;

namespace
{
class LogsBuildersTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    registry_ = std::make_shared<config_sdk::Registry>();
    config_sdk::RegisterDefaultLogsBuilders(registry_.get());
    registry_->SetConsoleLogRecordBuilder(
        std::make_unique<config_test::NoopConsoleLogRecordExporterBuilder>());
    registry_->SetOtlpHttpLogRecordBuilder(
        std::make_unique<config_test::RecordingOtlpHttpLogRecordExporterBuilder>(nullptr));
    registry_->SetOtlpGrpcLogRecordBuilder(
        std::make_unique<config_test::RecordingOtlpGrpcLogRecordExporterBuilder>(nullptr));
    registry_->SetOtlpFileLogRecordBuilder(
        std::make_unique<config_test::RecordingOtlpFileLogRecordExporterBuilder>(nullptr));
  }

  std::unique_ptr<logs_sdk::LogRecordProcessor> MakeLogRecordProcessor(
      std::unique_ptr<config_sdk::LogRecordProcessorConfiguration> model)
  {
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateLogRecordProcessor(model);
  }

  std::unique_ptr<logs_sdk::LogRecordExporter> MakeLogRecordExporter(
      std::unique_ptr<config_sdk::LogRecordExporterConfiguration> model)
  {
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateLogRecordExporter(model);
  }

  auto MakeLoggerConfigurator(std::unique_ptr<config_sdk::LoggerConfiguratorConfiguration> model)
  {
    config_sdk::SdkBuilder builder(registry_);
    return builder.CreateLoggerConfigurator(model);
  }

  std::shared_ptr<config_sdk::Registry> registry_;
};
}  // namespace

TEST_F(LogsBuildersTest, DefaultRegistry)
{
  auto registry = std::make_shared<config_sdk::Registry>();

  EXPECT_EQ(registry->GetBatchLogRecordProcessorBuilder(), nullptr);
  EXPECT_EQ(registry->GetSimpleLogRecordProcessorBuilder(), nullptr);
  EXPECT_EQ(registry->GetLoggerConfiguratorBuilder(), nullptr);
}

TEST_F(LogsBuildersTest, RegisterDefaultLogsBuilders)
{
  auto registry = std::make_shared<config_sdk::Registry>();
  config_sdk::RegisterDefaultLogsBuilders(registry.get());

  EXPECT_NE(registry->GetBatchLogRecordProcessorBuilder(), nullptr);
  EXPECT_NE(registry->GetSimpleLogRecordProcessorBuilder(), nullptr);
  EXPECT_NE(registry->GetLoggerConfiguratorBuilder(), nullptr);
}

TEST_F(LogsBuildersTest, LoggerConfiguratorBuilder)
{
  config_sdk::LoggerConfigConfiguration default_config;
  default_config.enabled          = true;
  default_config.minimum_severity = config_sdk::SeverityNumber::warn;
  default_config.trace_based      = false;

  config_sdk::LoggerMatcherAndConfigConfiguration matcher1;
  matcher1.name                    = "enabled_minsev_error_not_trace_based";
  matcher1.config.enabled          = true;
  matcher1.config.minimum_severity = config_sdk::SeverityNumber::error3;
  matcher1.config.trace_based      = false;

  config_sdk::LoggerMatcherAndConfigConfiguration matcher2;
  matcher2.name                    = "disabled_minsev_info_trace_based";
  matcher2.config.enabled          = false;
  matcher2.config.minimum_severity = config_sdk::SeverityNumber::debug;
  matcher2.config.trace_based      = true;

  auto model            = std::make_unique<config_sdk::LoggerConfiguratorConfiguration>();
  model->default_config = default_config;
  model->loggers.push_back(matcher1);
  model->loggers.push_back(matcher2);

  auto logger_configurator = MakeLoggerConfigurator(std::move(model));
  ASSERT_NE(logger_configurator, nullptr);

  auto default_scope = scope_sdk::InstrumentationScope::Create("default_scope");
  logs_sdk::LoggerConfig sdk_logger_config_default =
      logger_configurator->ComputeConfig(*default_scope);

  auto scope_1 = scope_sdk::InstrumentationScope::Create(matcher1.name);
  logs_sdk::LoggerConfig sdk_logger_config_1 = logger_configurator->ComputeConfig(*scope_1);

  auto scope_2 = scope_sdk::InstrumentationScope::Create(matcher2.name);
  logs_sdk::LoggerConfig sdk_logger_config_2 = logger_configurator->ComputeConfig(*scope_2);

  EXPECT_TRUE(sdk_logger_config_default.IsEnabled());
  EXPECT_EQ(sdk_logger_config_default.GetMinimumSeverity(), logs::Severity::kWarn);
  EXPECT_FALSE(sdk_logger_config_default.IsTraceBased());

  EXPECT_TRUE(sdk_logger_config_1.IsEnabled());
  EXPECT_EQ(sdk_logger_config_1.GetMinimumSeverity(), logs::Severity::kError3);
  EXPECT_FALSE(sdk_logger_config_1.IsTraceBased());

  EXPECT_FALSE(sdk_logger_config_2.IsEnabled());
  EXPECT_EQ(sdk_logger_config_2.GetMinimumSeverity(), logs::Severity::kDebug);
  EXPECT_TRUE(sdk_logger_config_2.IsTraceBased());
}

TEST_F(LogsBuildersTest, LoggerConfiguratorMinSeverity)
{
  struct TestCases
  {
    config_sdk::SeverityNumber input;
    logs::Severity expected;
  };

  TestCases kCases[] = {
      {config_sdk::SeverityNumber::trace, logs::Severity::kTrace},
      {config_sdk::SeverityNumber::trace2, logs::Severity::kTrace2},
      {config_sdk::SeverityNumber::trace3, logs::Severity::kTrace3},
      {config_sdk::SeverityNumber::trace4, logs::Severity::kTrace4},
      {config_sdk::SeverityNumber::debug, logs::Severity::kDebug},
      {config_sdk::SeverityNumber::debug2, logs::Severity::kDebug2},
      {config_sdk::SeverityNumber::debug3, logs::Severity::kDebug3},
      {config_sdk::SeverityNumber::debug4, logs::Severity::kDebug4},
      {config_sdk::SeverityNumber::info, logs::Severity::kInfo},
      {config_sdk::SeverityNumber::info2, logs::Severity::kInfo2},
      {config_sdk::SeverityNumber::info3, logs::Severity::kInfo3},
      {config_sdk::SeverityNumber::info4, logs::Severity::kInfo4},
      {config_sdk::SeverityNumber::warn, logs::Severity::kWarn},
      {config_sdk::SeverityNumber::warn2, logs::Severity::kWarn2},
      {config_sdk::SeverityNumber::warn3, logs::Severity::kWarn3},
      {config_sdk::SeverityNumber::warn4, logs::Severity::kWarn4},
      {config_sdk::SeverityNumber::error, logs::Severity::kError},
      {config_sdk::SeverityNumber::error2, logs::Severity::kError2},
      {config_sdk::SeverityNumber::error3, logs::Severity::kError3},
      {config_sdk::SeverityNumber::error4, logs::Severity::kError4},
      {config_sdk::SeverityNumber::fatal, logs::Severity::kFatal},
      {config_sdk::SeverityNumber::fatal2, logs::Severity::kFatal2},
      {config_sdk::SeverityNumber::fatal3, logs::Severity::kFatal3},
      {config_sdk::SeverityNumber::fatal4, logs::Severity::kFatal4},
  };

  for (const auto &test_case : kCases)
  {
    auto model = std::make_unique<config_sdk::LoggerConfiguratorConfiguration>();
    model->default_config.minimum_severity = test_case.input;
    auto configurator                      = MakeLoggerConfigurator(std::move(model));
    ASSERT_NE(configurator, nullptr);
    auto scope  = scope_sdk::InstrumentationScope::Create("s");
    auto config = configurator->ComputeConfig(*scope);
    EXPECT_EQ(config.GetMinimumSeverity(), test_case.expected);
  }
}

TEST_F(LogsBuildersTest, CreateBatchLogRecordProcessor)
{
  auto exporter       = std::make_unique<config_sdk::ConsoleLogRecordExporterConfiguration>();
  auto processor      = std::make_unique<config_sdk::BatchLogRecordProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::LogRecordProcessorConfiguration> model = std::move(processor);

  auto result = MakeLogRecordProcessor(std::move(model));
  EXPECT_NE(result, nullptr);
}

TEST_F(LogsBuildersTest, CreateSimpleLogRecordProcessor)
{
  auto exporter       = std::make_unique<config_sdk::ConsoleLogRecordExporterConfiguration>();
  auto processor      = std::make_unique<config_sdk::SimpleLogRecordProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::LogRecordProcessorConfiguration> model = std::move(processor);

  auto result = MakeLogRecordProcessor(std::move(model));
  EXPECT_NE(result, nullptr);
}

TEST_F(LogsBuildersTest, UnregisteredLoggerConfigurator)
{
  registry_->SetLoggerConfiguratorBuilder(nullptr);
  auto model = std::make_unique<config_sdk::LoggerConfiguratorConfiguration>();
  EXPECT_THROW(MakeLoggerConfigurator(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(LogsBuildersTest, UnregisteredExtensionLogRecordExporter)
{
  auto ext  = std::make_unique<config_sdk::ExtensionLogRecordExporterConfiguration>();
  ext->name = "unregistered";
  std::unique_ptr<config_sdk::LogRecordExporterConfiguration> model = std::move(ext);
  EXPECT_THROW(MakeLogRecordExporter(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(LogsBuildersTest, RegisteredExtensionLogRecordExporterBuilder)
{
  auto buffer  = std::make_shared<config_test::LogRecordBuffer>();
  auto builder = std::make_unique<config_test::RecordingLogRecordExporterBuilder>(buffer);
  registry_->SetExtensionLogRecordExporterBuilder("my_exporter", std::move(builder));

  auto ext  = std::make_unique<config_sdk::ExtensionLogRecordExporterConfiguration>();
  ext->name = "my_exporter";
  std::unique_ptr<config_sdk::LogRecordExporterConfiguration> model = std::move(ext);
  auto exporter = MakeLogRecordExporter(std::move(model));

  EXPECT_NE(exporter, nullptr);
}

TEST_F(LogsBuildersTest, UnregisteredExtensionLogRecordProcessorBuilder)
{
  auto ext  = std::make_unique<config_sdk::ExtensionLogRecordProcessorConfiguration>();
  ext->name = "unregistered";
  std::unique_ptr<config_sdk::LogRecordProcessorConfiguration> model = std::move(ext);
  EXPECT_THROW(MakeLogRecordProcessor(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(LogsBuildersTest, RegisteredExtensionLogRecordProcessorBuilder)
{
  auto builder      = std::make_unique<config_test::RecordingLogRecordProcessorBuilder>();
  auto *builder_ptr = builder.get();
  registry_->SetExtensionLogRecordProcessorBuilder("my_processor", std::move(builder));

  auto ext  = std::make_unique<config_sdk::ExtensionLogRecordProcessorConfiguration>();
  ext->name = "my_processor";
  std::unique_ptr<config_sdk::LogRecordProcessorConfiguration> model = std::move(ext);
  auto processor = MakeLogRecordProcessor(std::move(model));

  EXPECT_NE(processor, nullptr);
  EXPECT_TRUE(builder_ptr->called);
}

TEST_F(LogsBuildersTest, UnregisteredBatchLogRecordProcessorBuilder)
{
  registry_->SetBatchLogRecordProcessorBuilder(nullptr);

  auto exporter       = std::make_unique<config_sdk::ConsoleLogRecordExporterConfiguration>();
  auto processor      = std::make_unique<config_sdk::BatchLogRecordProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::LogRecordProcessorConfiguration> model = std::move(processor);

  EXPECT_THROW(MakeLogRecordProcessor(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(LogsBuildersTest, UnregisteredSimpleLogRecordProcessorBuilder)
{
  registry_->SetSimpleLogRecordProcessorBuilder(nullptr);

  auto exporter       = std::make_unique<config_sdk::ConsoleLogRecordExporterConfiguration>();
  auto processor      = std::make_unique<config_sdk::SimpleLogRecordProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  std::unique_ptr<config_sdk::LogRecordProcessorConfiguration> model = std::move(processor);

  EXPECT_THROW(MakeLogRecordProcessor(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(LogsBuildersTest, UnregisteredConsoleLogRecordExporterBuilder)
{
  registry_->SetConsoleLogRecordBuilder(nullptr);
  auto model = std::make_unique<config_sdk::ConsoleLogRecordExporterConfiguration>();
  EXPECT_THROW(MakeLogRecordExporter(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(LogsBuildersTest, UnregisteredOtlpHttpLogRecordExporterBuilder)
{
  registry_->SetOtlpHttpLogRecordBuilder(nullptr);
  auto model = std::make_unique<config_sdk::OtlpHttpLogRecordExporterConfiguration>();
  EXPECT_THROW(MakeLogRecordExporter(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(LogsBuildersTest, UnregisteredOtlpGrpcLogRecordExporterBuilder)
{
  registry_->SetOtlpGrpcLogRecordBuilder(nullptr);
  auto model = std::make_unique<config_sdk::OtlpGrpcLogRecordExporterConfiguration>();
  EXPECT_THROW(MakeLogRecordExporter(std::move(model)), config_sdk::UnsupportedException);
}

TEST_F(LogsBuildersTest, UnregisteredOtlpFileLogRecordExporterBuilder)
{
  registry_->SetOtlpFileLogRecordBuilder(nullptr);
  auto model = std::make_unique<config_sdk::OtlpFileLogRecordExporterConfiguration>();
  EXPECT_THROW(MakeLogRecordExporter(std::move(model)), config_sdk::UnsupportedException);
}

// ---------------------------------------------------------------------------
// LoggerProviderConfigTest: Use ConfiguredSdk::Create to verify logger provider configuration and
// installation.

namespace
{

class LoggerProviderConfigTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    registry_   = std::make_shared<config_sdk::Registry>();
    log_buffer_ = std::make_shared<config_test::LogRecordBuffer>();
    config_sdk::RegisterDefaultLogsBuilders(registry_.get());
  }

  void TearDown() override
  {
    if (sdk_)
      sdk_->UnInstall();
  }

  static std::unique_ptr<config_sdk::Configuration> MakeLoggerProviderConfig(
      std::unique_ptr<config_sdk::LogRecordExporterConfiguration> exporter)
  {
    auto processor      = std::make_unique<config_sdk::SimpleLogRecordProcessorConfiguration>();
    processor->exporter = std::move(exporter);
    return MakeLoggerProviderConfig(std::move(processor));
  }

  static std::unique_ptr<config_sdk::Configuration> MakeLoggerProviderConfig(
      std::unique_ptr<config_sdk::LogRecordProcessorConfiguration> processor)
  {
    auto lp = std::make_unique<config_sdk::LoggerProviderConfiguration>();
    lp->processors.emplace_back(std::move(processor));
    auto model             = std::make_unique<config_sdk::Configuration>();
    model->logger_provider = std::move(lp);
    return model;
  }

  void BuildAndInstall(const std::unique_ptr<config_sdk::Configuration> &model)
  {
    sdk_ = config_sdk::ConfiguredSdk::Create(registry_, model);
    ASSERT_NE(sdk_, nullptr);
    sdk_->Install();
  }

  void EmitAndVerify()
  {
    ASSERT_NE(sdk_->logger_provider, nullptr);
    ASSERT_EQ(sdk_->tracer_provider, nullptr);
    ASSERT_EQ(sdk_->meter_provider, nullptr);

    auto provider = logs::Provider::GetLoggerProvider();
    ASSERT_NE(provider, nullptr);

    provider->GetLogger("logger")->Info("logs");
    ASSERT_TRUE(sdk_->logger_provider->ForceFlush(std::chrono::milliseconds(5000)));
    ASSERT_TRUE(sdk_->logger_provider->Shutdown(std::chrono::milliseconds(5000)));
    EXPECT_EQ(log_buffer_->size(), 1u);
  }

  std::shared_ptr<config_sdk::Registry> registry_;
  std::shared_ptr<config_test::LogRecordBuffer> log_buffer_;
  std::unique_ptr<config_sdk::ConfiguredSdk> sdk_;
};

}  // namespace

TEST_F(LoggerProviderConfigTest, LoggerProviderWithExtensionLogRecordExporter)
{
  registry_->SetExtensionLogRecordExporterBuilder(
      "recording", std::make_unique<config_test::RecordingLogRecordExporterBuilder>(log_buffer_));

  auto exporter  = std::make_unique<config_sdk::ExtensionLogRecordExporterConfiguration>();
  exporter->name = "recording";
  auto model     = MakeLoggerProviderConfig(std::move(exporter));
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(LoggerProviderConfigTest, LoggerProviderWithConsoleLogRecordExporter)
{
  registry_->SetConsoleLogRecordBuilder(
      std::make_unique<config_test::RecordingConsoleLogRecordExporterBuilder>(log_buffer_));
  auto model = MakeLoggerProviderConfig(
      std::make_unique<config_sdk::ConsoleLogRecordExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(LoggerProviderConfigTest, LoggerProviderWithOtlpHttpLogRecordExporter)
{
  registry_->SetOtlpHttpLogRecordBuilder(
      std::make_unique<config_test::RecordingOtlpHttpLogRecordExporterBuilder>(log_buffer_));
  auto model = MakeLoggerProviderConfig(
      std::make_unique<config_sdk::OtlpHttpLogRecordExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(LoggerProviderConfigTest, LoggerProviderWithOtlpGrpcLogRecordExporter)
{
  registry_->SetOtlpGrpcLogRecordBuilder(
      std::make_unique<config_test::RecordingOtlpGrpcLogRecordExporterBuilder>(log_buffer_));
  auto model = MakeLoggerProviderConfig(
      std::make_unique<config_sdk::OtlpGrpcLogRecordExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(LoggerProviderConfigTest, LoggerProviderWithOtlpFileLogRecordExporter)
{
  registry_->SetOtlpFileLogRecordBuilder(
      std::make_unique<config_test::RecordingOtlpFileLogRecordExporterBuilder>(log_buffer_));
  auto model = MakeLoggerProviderConfig(
      std::make_unique<config_sdk::OtlpFileLogRecordExporterConfiguration>());
  BuildAndInstall(model);
  EmitAndVerify();
}

TEST_F(LoggerProviderConfigTest, LoggerProviderWithBatchLogRecordProcessor)
{
  registry_->SetBatchLogRecordProcessorBuilder(
      std::make_unique<config_test::MockBatchLogRecordProcessorBuilder>());
  registry_->SetExtensionLogRecordExporterBuilder(
      "recording", std::make_unique<config_test::RecordingLogRecordExporterBuilder>(log_buffer_));

  auto exporter       = std::make_unique<config_sdk::ExtensionLogRecordExporterConfiguration>();
  exporter->name      = "recording";
  auto processor      = std::make_unique<config_sdk::BatchLogRecordProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  auto model          = MakeLoggerProviderConfig(std::move(processor));
  BuildAndInstall(model);
  EmitAndVerify();
}
