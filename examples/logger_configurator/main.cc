// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// This example shows how to use LoggerProvider::UpdateLoggerConfigurator to dynamically control
// the minimum severity level of loggers at runtime. Updating the LoggerConfigurator
// affects all existing and future loggers provided by the LoggerProvider. It is thread-safe and
// can be called concurrently with logger and log record creation.
//
// Three instrumentation scopes are shown:
// 1. "my_application" (example instrumented user application code),
// 2. "my_library"     (example instrumented user library code),
// 3. "external_library" (example instrumented third-party dependency).
//
// The example simulates a typical debugging workflow:
//
// Stage 1: Production baseline. my_application and my_library loggers at Warn.
//          external_library logger is disabled.
// Stage 2: A user reports unexpected behavior. Lower my_application and my_library loggers to
// Debug.
//          external_library logger remains disabled.
// Stage 3: Debug logs implicate external_library. Re-enable its logger at Debug.
//          Raise my_application logger back to Warn.
// Stage 4: Root cause found and fixed. Restore the production baseline.

#include <chrono>
#include <initializer_list>
#include <iostream>
#include <thread>
#include <utility>

#include <string>
#include <vector>
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/processor.h"

#include "opentelemetry/exporters/ostream/log_record_exporter_factory.h"
#include "opentelemetry/logs/logger.h"
#include "opentelemetry/logs/logger_provider.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/logs/logger_config.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/semconv/service_attributes.h"

namespace logs_api       = opentelemetry::logs;
namespace logs_sdk       = opentelemetry::sdk::logs;
namespace logs_exporters = opentelemetry::exporter::logs;
namespace scope_sdk      = opentelemetry::sdk::instrumentationscope;
namespace nostd          = opentelemetry::nostd;

namespace
{

bool external_library_has_error = true;  // simulate an error condition in the external library

// ---------------------------------------------------------------------------
// EmitLogRecord Helper: For ABIv1 checks Enabled() based on severity of the record and emits.
// For ABI v2 EmitLogRecord applies filtering implicitly based on the LoggerConfig.
// ---------------------------------------------------------------------------
void Log(const nostd::shared_ptr<logs_api::Logger> &logger,
         logs_api::Severity severity,
         nostd::string_view message) noexcept
{
#if OPENTELEMETRY_ABI_VERSION_NO <= 1
  // EmitLogRecord does not apply the minimum-severity filter in ABI v1. Check Enabled() before
  // emitting.
  if (logger->Enabled(severity))
  {
    logger->EmitLogRecord(severity, message);
  }
#else
  // EmitLogRecord automatically applies the minimum-severity filter in ABI v2.
  logger->EmitLogRecord(severity, message);
#endif
}

// ---------------------------------------------------------------------------
// Example external library
// ---------------------------------------------------------------------------
namespace external_library
{
class ExternalModule
{
public:
  ExternalModule()
      : logger_(logs_api::Provider::GetLoggerProvider()->GetLogger("ExternalModule",
                                                                   "external_library"))
  {}

  bool Execute()
  {
    Log(logger_, logs_api::Severity::kDebug, "ExternalModule: Execute called...");
    std::this_thread::sleep_for(std::chrono::microseconds(1));

    Log(logger_, logs_api::Severity::kDebug, "ExternalModule: executing...");
    std::this_thread::sleep_for(std::chrono::microseconds(1));

    const bool has_error = external_library_has_error;  // simulate an error condition

    if (has_error)
    {
      Log(logger_, logs_api::Severity::kError, "ExternalModule: error detected!");
      Log(logger_, logs_api::Severity::kDebug, "ExternalModule: some error details.");
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1));
    Log(logger_, logs_api::Severity::kDebug, "ExternalModule: Execute complete");
    return !has_error;
  }

private:
  nostd::shared_ptr<logs_api::Logger> logger_;
};
}  // namespace external_library

// ---------------------------------------------------------------------------
// Example user library
// ---------------------------------------------------------------------------
namespace my_library
{
class MyModule
{
public:
  MyModule() : logger_(logs_api::Provider::GetLoggerProvider()->GetLogger("MyModule", "my_library"))
  {}

  bool Execute()
  {
    Log(logger_, logs_api::Severity::kDebug, "MyModule: Execute called...");

    const auto result = external_module_.Execute();

    if (!result)
    {
      Log(logger_, logs_api::Severity::kDebug, "MyModule: external_library returned an error");
    }
    Log(logger_, logs_api::Severity::kDebug, "MyModule: Execute complete");
    return result;
  }

private:
  nostd::shared_ptr<logs_api::Logger> logger_;
  external_library::ExternalModule external_module_;
};
}  // namespace my_library

// ---------------------------------------------------------------------------
// Example user application
// ---------------------------------------------------------------------------
class MyApplication
{
public:
  MyApplication()
      : logger_(
            logs_api::Provider::GetLoggerProvider()->GetLogger("MyApplication", "my_application"))
  {}

  void Execute()
  {
    Log(logger_, logs_api::Severity::kDebug, "MyApplication: Execute called...");
    std::this_thread::sleep_for(std::chrono::microseconds(1));

    const auto result = my_module_.Execute();

    if (!result)
    {
      Log(logger_, logs_api::Severity::kWarn,
          "MyApplication: my_library invoke failed with an error");
    }
    Log(logger_, logs_api::Severity::kDebug, "MyApplication: Execute complete");
  }

private:
  nostd::shared_ptr<logs_api::Logger> logger_;
  my_library::MyModule my_module_;
};

// ---------------------------------------------------------------------------
// ScopeConfigurator builder helper
// ---------------------------------------------------------------------------

// Builds a configurator that applies default_config to all scopes, with optional per-scope
// overrides.
std::unique_ptr<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>> MakeLoggerConfigurator(
    logs_sdk::LoggerConfig default_config,
    std::initializer_list<std::pair<nostd::string_view, logs_sdk::LoggerConfig>> overrides = {})
{
  scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>::Builder builder(default_config);
  for (const auto &kv : overrides)
  {
    const auto &name   = kv.first;
    const auto &config = kv.second;
    builder.AddConditionNameEquals(name, config);
  }
  return std::make_unique<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>>(builder.Build());
}

// Creates a LoggerProvider with an OStreamLogRecordExporter and initial
// ScopeConfigurator.
std::shared_ptr<logs_sdk::LoggerProvider> CreateLoggerProvider(
    std::unique_ptr<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>> configurator)
{
  auto exporter  = logs_exporters::OStreamLogRecordExporterFactory::Create();
  auto processor = logs_sdk::SimpleLogRecordProcessorFactory::Create(std::move(exporter));

  return std::make_shared<logs_sdk::LoggerProvider>(
      std::move(processor),
      opentelemetry::sdk::resource::Resource::Create(
          {{opentelemetry::semconv::service::kServiceName, "logger_configurator_example"}}),
      std::move(configurator));
}

}  // namespace

int main()
{
  // Create LoggerConfig objects for the various configurations we will use in this example.
  const bool is_logger_enabled = true;
  const bool is_trace_based    = false;

  // A LoggerConfig that enables the logger and sets the minimum severity to Warn.
  const logs_sdk::LoggerConfig warn_config =
      logs_sdk::LoggerConfig::Create(is_logger_enabled, logs_api::Severity::kWarn, is_trace_based);

  // A LoggerConfig that enables the logger and sets the minimum severity to Debug.
  const logs_sdk::LoggerConfig debug_config =
      logs_sdk::LoggerConfig::Create(is_logger_enabled, logs_api::Severity::kDebug, is_trace_based);

  // A LoggerConfig that disables the logger.
  const logs_sdk::LoggerConfig disabled_config = logs_sdk::LoggerConfig::Disabled();

  // -------------------------------------------------------------------------
  // Stage 1: Production baseline.
  //          my_application and my_library loggers at Warn. external_library logger is disabled.
  // -------------------------------------------------------------------------
  auto sdk_logger_provider = CreateLoggerProvider(
      MakeLoggerConfigurator(warn_config, {{"external_library", disabled_config}}));

  logs_api::Provider::SetLoggerProvider(
      nostd::shared_ptr<logs_api::LoggerProvider>(sdk_logger_provider));

  // Instantiate the application (this creates the loggers for all three scopes).
  MyApplication my_app;

  std::cout << "Stage 1: production baseline\n";
  std::cout << "  my_application and my_library at Warn. external_library disabled\n";
  my_app.Execute();

  // -------------------------------------------------------------------------
  // Stage 2: User reports unexpected behavior.
  //          Lower my_application and my_library to Debug. external_library still disabled.
  // -------------------------------------------------------------------------
  std::cout << "\nStage 2: investigating, loggers lowered to Debug\n";
  std::cout << "  my_application and my_library at Debug. external_library disabled\n";

  sdk_logger_provider->UpdateLoggerConfigurator(
      MakeLoggerConfigurator(debug_config, {{"external_library", disabled_config}}));

  my_app.Execute();

  // -------------------------------------------------------------------------
  // Stage 3: Debug logs implicate external_library.
  //          Enable its logger and my_library logger at Debug. Raise my_application logger back to
  //          Warn.
  // -------------------------------------------------------------------------
  std::cout << "\nStage 3: narrowing down, my_library and external_library at Debug\n";
  std::cout << "  my_library and external_library at Debug. my_application at Warn\n";

  sdk_logger_provider->UpdateLoggerConfigurator(MakeLoggerConfigurator(
      warn_config, {{"my_library", debug_config}, {"external_library", debug_config}}));

  my_app.Execute();

  // -------------------------------------------------------------------------
  // Stage 4: Root cause found and fixed. Restore the production baseline.
  // -------------------------------------------------------------------------
  std::cout << "\nStage 4: external_library error resolved, production baseline restored\n";
  std::cout << "  my_application and my_library at Warn. external_library disabled\n";

  external_library_has_error = false;  // simulate that the error condition has been fixed

  sdk_logger_provider->UpdateLoggerConfigurator(
      MakeLoggerConfigurator(warn_config, {{"external_library", disabled_config}}));

  my_app.Execute();

  sdk_logger_provider->ForceFlush();
  sdk_logger_provider->Shutdown();
  return 0;
}
