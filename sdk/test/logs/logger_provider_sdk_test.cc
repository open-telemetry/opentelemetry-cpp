// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stdint.h>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <future>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/logs/logger.h"
#include "opentelemetry/logs/logger_provider.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/logs/event_logger_provider.h"          // IWYU pragma: keep
#include "opentelemetry/sdk/logs/event_logger_provider_factory.h"  // IWYU pragma: keep
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/logger.h"
#include "opentelemetry/sdk/logs/logger_config.h"
#include "opentelemetry/sdk/logs/logger_context.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/provider.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor.h"
#include "opentelemetry/sdk/resource/resource.h"

using namespace opentelemetry::sdk::logs;
namespace logs_api  = opentelemetry::logs;
namespace logs_sdk  = opentelemetry::sdk::logs;
namespace nostd     = opentelemetry::nostd;
namespace scope_sdk = opentelemetry::sdk::instrumentationscope;

TEST(LoggerProviderSDK, PushToAPI)
{
  auto lp =
      nostd::shared_ptr<logs_api::LoggerProvider>(new opentelemetry::sdk::logs::LoggerProvider());
  logs_sdk::Provider::SetLoggerProvider(lp);

  // Check that the loggerprovider was correctly pushed into the API
  ASSERT_EQ(lp, logs_api::Provider::GetLoggerProvider());
}

TEST(LoggerProviderSDK, LoggerProviderGetLoggerSimple)
{
  auto lp = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());

  nostd::string_view schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger1 = lp->GetLogger("logger1", "opentelelemtry_library", "", schema_url);
  auto logger2 = lp->GetLogger("logger2", "", "", schema_url);

  // Check that the logger is not nullptr
  ASSERT_NE(logger1, nullptr);
  ASSERT_NE(logger2, nullptr);

  auto sdk_logger1 = static_cast<opentelemetry::sdk::logs::Logger *>(logger1.get());
  auto sdk_logger2 = static_cast<opentelemetry::sdk::logs::Logger *>(logger2.get());
  ASSERT_EQ(sdk_logger1->GetInstrumentationScope().GetName(), "opentelelemtry_library");
  ASSERT_EQ(sdk_logger1->GetInstrumentationScope().GetVersion(), "");
  ASSERT_EQ(sdk_logger1->GetInstrumentationScope().GetSchemaURL(), schema_url);

  ASSERT_EQ(sdk_logger2->GetInstrumentationScope().GetName(), "logger2");
  ASSERT_EQ(sdk_logger2->GetInstrumentationScope().GetVersion(), "");
  ASSERT_EQ(sdk_logger2->GetInstrumentationScope().GetSchemaURL(), schema_url);

  // Check that two loggers with different names aren't the same instance
  ASSERT_NE(logger1, logger2);

  // Check that two loggers with the same name are the same instance
  auto logger3 = lp->GetLogger("logger1", "opentelelemtry_library", "", schema_url);
  ASSERT_EQ(logger1, logger3);
  auto sdk_logger3 = static_cast<opentelemetry::sdk::logs::Logger *>(logger3.get());
  ASSERT_EQ(sdk_logger3->GetInstrumentationScope(), sdk_logger1->GetInstrumentationScope());
}

TEST(LoggerProviderSDK, LoggerProviderLoggerArguments)
{
  auto lp = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());

  nostd::string_view schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger1 = lp->GetLogger("logger1", "opentelelemtry_library", "", schema_url);

  auto logger2     = lp->GetLogger("logger2", "opentelelemtry_library", "", schema_url);
  auto sdk_logger1 = static_cast<opentelemetry::sdk::logs::Logger *>(logger1.get());
  auto sdk_logger2 = static_cast<opentelemetry::sdk::logs::Logger *>(logger2.get());
  ASSERT_EQ(sdk_logger2->GetInstrumentationScope(), sdk_logger1->GetInstrumentationScope());

  auto logger3 = lp->GetLogger("logger3", "opentelelemtry_library", "", schema_url,
                               {{"scope_key1", "scope_value"}, {"scope_key2", 2}});

  auto sdk_logger3 = static_cast<opentelemetry::sdk::logs::Logger *>(logger3.get());
  EXPECT_EQ(sdk_logger3->GetInstrumentationScope().GetAttributes().size(), 2);
  {
    auto attibute = sdk_logger3->GetInstrumentationScope().GetAttributes().find("scope_key1");
    ASSERT_FALSE(attibute == sdk_logger3->GetInstrumentationScope().GetAttributes().end());
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::string>(attibute->second));
    EXPECT_EQ(opentelemetry::nostd::get<std::string>(attibute->second), "scope_value");
  }

  std::unordered_map<std::string, std::string> scope_attributes = {{"scope_key", "scope_value"}};
  auto logger4 =
      lp->GetLogger("logger4", "opentelelemtry_library", "", schema_url, scope_attributes);
  auto sdk_logger4 = static_cast<opentelemetry::sdk::logs::Logger *>(logger4.get());

  EXPECT_EQ(sdk_logger4->GetInstrumentationScope().GetAttributes().size(), 1);
  {
    auto attibute = sdk_logger4->GetInstrumentationScope().GetAttributes().find("scope_key");
    ASSERT_FALSE(attibute == sdk_logger4->GetInstrumentationScope().GetAttributes().end());
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::string>(attibute->second));
    EXPECT_EQ(opentelemetry::nostd::get<std::string>(attibute->second), "scope_value");
  }
}

#if OPENTELEMETRY_ABI_VERSION_NO < 2

/*
 * opentelemetry::sdk::logs::EventLoggerProviderFactory is deprecated.
 * Suppress warnings in tests, to have a clean build and coverage.
 */

#  if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 4996)
#  elif defined(__GNUC__) && !defined(__clang__) && !defined(__apple_build_version__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#  elif defined(__clang__) || defined(__apple_build_version__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  endif

TEST(LoggerProviderSDK, EventLoggerProviderFactory)
{
  auto elp = opentelemetry::sdk::logs::EventLoggerProviderFactory::Create();
  auto lp  = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());

  nostd::string_view schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger1 = lp->GetLogger("logger1", "opentelelemtry_library", "", schema_url);

  auto event_logger = elp->CreateEventLogger(logger1, "otel-cpp.test");
}

#  if defined(_MSC_VER)
#    pragma warning(pop)
#  elif defined(__GNUC__) && !defined(__clang__) && !defined(__apple_build_version__)
#    pragma GCC diagnostic pop
#  elif defined(__clang__) || defined(__apple_build_version__)
#    pragma clang diagnostic pop
#  endif

#endif

TEST(LoggerProviderSDK, LoggerEqualityCheck)
{
  auto lp = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());
  nostd::string_view schema_url{"https://opentelemetry.io/schemas/1.11.0"};

  auto logger1 = lp->GetLogger("logger1", "opentelelemtry_library", "", schema_url);
  auto logger2 = lp->GetLogger("logger1", "opentelelemtry_library", "", schema_url);
  EXPECT_EQ(logger1, logger2);

  auto logger3         = lp->GetLogger("logger3");
  auto another_logger3 = lp->GetLogger("logger3");
  EXPECT_EQ(logger3, another_logger3);

  auto logger4         = lp->GetLogger("logger4", "opentelelemtry_library", "1.0.0", schema_url);
  auto another_logger4 = lp->GetLogger("logger4", "opentelelemtry_library", "1.0.0", schema_url);
  EXPECT_EQ(logger4, another_logger4);

  auto logger5 =
      lp->GetLogger("logger5", "opentelelemtry_library", "1.0.0", schema_url, {{"key", "value"}});
  auto another_logger5 =
      lp->GetLogger("logger5", "opentelelemtry_library", "1.0.0", schema_url, {{"key", "value"}});
  EXPECT_EQ(logger5, another_logger5);
}

TEST(LoggerProviderSDK, GetLoggerInequalityCheck)
{
  auto lp               = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());
  auto logger_library_1 = lp->GetLogger("logger1", "library_1");
  auto logger_library_2 = lp->GetLogger("logger1", "library_2");
  auto logger_version_1 = lp->GetLogger("logger1", "library_1", "1.0.0");
  auto logger_version_2 = lp->GetLogger("logger1", "library_1", "2.0.0");
  auto logger_url_1     = lp->GetLogger("logger1", "library_1", "1.0.0", "url_1");
  auto logger_url_2     = lp->GetLogger("logger1", "library_1", "1.0.0", "url_2");
  auto logger_attribute_1 =
      lp->GetLogger("logger1", "library_1", "1.0.0", "url_1", {{"key", "one"}});
  auto logger_attribute_2 =
      lp->GetLogger("logger1", "library_1", "1.0.0", "url_1", {{"key", "two"}});

  // different scope names should return distinct loggers
  EXPECT_NE(logger_library_1, logger_library_2);

  // different scope versions should return distinct loggers
  EXPECT_NE(logger_version_1, logger_library_1);
  EXPECT_NE(logger_version_1, logger_version_2);

  // different scope schema urls should return distinct loggers
  EXPECT_NE(logger_url_1, logger_library_1);
  EXPECT_NE(logger_url_1, logger_version_1);
  EXPECT_NE(logger_url_1, logger_url_2);

  // different scope attributes should return distinct loggers
  EXPECT_NE(logger_attribute_1, logger_library_1);
  EXPECT_NE(logger_attribute_1, logger_url_1);
  EXPECT_NE(logger_attribute_1, logger_attribute_2);
}

namespace
{
class DummyLogRecordable final : public opentelemetry::sdk::logs::Recordable
{
public:
  void SetTimestamp(opentelemetry::common::SystemTimestamp) noexcept override {}

  void SetObservedTimestamp(opentelemetry::common::SystemTimestamp) noexcept override {}

  void SetSeverity(opentelemetry::logs::Severity) noexcept override {}

  void SetBody(const opentelemetry::common::AttributeValue &) noexcept override {}

  void SetEventId(int64_t, nostd::string_view) noexcept override {}

  void SetTraceId(const opentelemetry::trace::TraceId &) noexcept override {}

  void SetSpanId(const opentelemetry::trace::SpanId &) noexcept override {}

  void SetTraceFlags(const opentelemetry::trace::TraceFlags &) noexcept override {}

  void SetAttribute(nostd::string_view,
                    const opentelemetry::common::AttributeValue &) noexcept override
  {}

  void SetResource(const opentelemetry::sdk::resource::Resource &) noexcept override {}

  void SetInstrumentationScope(
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope &) noexcept override
  {}
};

class DummyProcessor : public LogRecordProcessor
{
public:
  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new DummyLogRecordable());
  }

  void OnEmit(std::unique_ptr<Recordable> &&record) noexcept override
  {
    auto record_ptr = std::move(record);
  }
  bool ForceFlush(std::chrono::microseconds /* timeout */) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds /* timeout */) noexcept override { return true; }
};
}  // namespace

TEST(LoggerProviderSDK, GetResource)
{
  // Create a LoggerProvider without a processor
  auto resource = opentelemetry::sdk::resource::Resource::Create({{"key", "value"}});
  LoggerProvider lp{nullptr, resource};
  ASSERT_EQ(nostd::get<std::string>(lp.GetResource().GetAttributes().at("key")), "value");
}

TEST(LoggerProviderSDK, Shutdown)
{
  std::unique_ptr<SimpleLogRecordProcessor> processor(new SimpleLogRecordProcessor(nullptr));
  SimpleLogRecordProcessor *processor_ptr = processor.get();
  std::vector<std::unique_ptr<LogRecordProcessor>> processors;
  processors.push_back(std::move(processor));

  std::unique_ptr<LoggerContext> context(new LoggerContext(std::move(processors)));
  LoggerProvider lp(std::move(context));

  EXPECT_TRUE(lp.Shutdown());
  EXPECT_TRUE(processor_ptr->IsShutdown());

  // It's safe to shutdown again
  EXPECT_TRUE(lp.Shutdown());
}

TEST(LoggerProviderSDK, ForceFlush)
{
  std::unique_ptr<SimpleLogRecordProcessor> processor(new SimpleLogRecordProcessor(nullptr));
  std::vector<std::unique_ptr<LogRecordProcessor>> processors;
  processors.push_back(std::move(processor));

  std::unique_ptr<LoggerContext> context(new LoggerContext(std::move(processors)));
  LoggerProvider lp(std::move(context));

  EXPECT_TRUE(lp.ForceFlush());
}

// ---------------------------------------------------------------------------
// UpdateLoggerConfigurator tests
// ---------------------------------------------------------------------------
namespace
{

// Minimal recording exporter used across the UpdateLoggerConfigurator tests.
class RecordingExporter final : public logs_sdk::LogRecordExporter
{
public:
  std::unique_ptr<logs_sdk::Recordable> MakeRecordable() noexcept override
  {
    return std::make_unique<logs_sdk::ReadWriteLogRecord>();
  }

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<logs_sdk::Recordable>> &records) noexcept
      override
  {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &r : records)
    {
      records_.push_back(std::move(r));
    }
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  bool ForceFlush(std::chrono::microseconds /*timeout*/) noexcept override { return true; }

  bool Shutdown(std::chrono::microseconds /*timeout*/) noexcept override { return true; }

  std::size_t RecordCount() const
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return records_.size();
  }

  void Reset()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    records_.clear();
  }

private:
  mutable std::mutex mutex_;
  std::vector<std::unique_ptr<logs_sdk::Recordable>> records_;
};

// Helper: make a ScopeConfigurator where all loggers default to enabled with the given min
// severity.
std::unique_ptr<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>> AllAtSeverity(
    logs_api::Severity severity)
{
  return std::make_unique<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>>(
      scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>::Builder(
          logs_sdk::LoggerConfig::Create(true, severity, false))
          .Build());
}

// Helper: make a ScopeConfigurator where the named scope is disabled; default is enabled.
std::unique_ptr<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>> DisableByName(
    opentelemetry::nostd::string_view name)
{
  return std::make_unique<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>>(
      scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>::Builder(
          logs_sdk::LoggerConfig::Default())
          .AddConditionNameEquals(name, logs_sdk::LoggerConfig::Disabled())
          .Build());
}

// Helper: make a ScopeConfigurator that disables all loggers.
std::unique_ptr<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>> DisableAll()
{
  return std::make_unique<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>>(
      scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>::Builder(
          logs_sdk::LoggerConfig::Disabled())
          .Build());
}

// Helper: make a ScopeConfigurator that enables all loggers (default config).
std::unique_ptr<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>> EnableAll()
{
  return std::make_unique<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>>(
      scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>::Builder(
          logs_sdk::LoggerConfig::Default())
          .Build());
}

// Helper: Info default for all scopes with the named scope set to a specific minimum severity
// override.
std::unique_ptr<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>> InfoWithOverrideSeverity(
    opentelemetry::nostd::string_view scope_name,
    logs_api::Severity override_severity)
{
  return std::make_unique<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>>(
      scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>::Builder(
          logs_sdk::LoggerConfig::Create(true, logs_api::Severity::kInfo, false))
          .AddConditionNameEquals(scope_name,
                                  logs_sdk::LoggerConfig::Create(true, override_severity, false))
          .Build());
}

// Builds a LoggerProvider whose RecordingExporter pointer is returned via the out parameter.
std::unique_ptr<logs_sdk::LoggerProvider> MakeProvider(
    RecordingExporter *&exporter_out,
    std::unique_ptr<scope_sdk::ScopeConfigurator<logs_sdk::LoggerConfig>> configurator =
        EnableAll())
{
  auto exporter  = std::make_unique<RecordingExporter>();
  exporter_out   = exporter.get();
  auto processor = std::make_unique<logs_sdk::SimpleLogRecordProcessor>(std::move(exporter));
  return std::make_unique<logs_sdk::LoggerProvider>(
      std::move(processor), opentelemetry::sdk::resource::Resource::Create({}),
      std::move(configurator));
}

}  // namespace

TEST(LoggerProviderSDK, UpdateLoggerConfiguratorDisableByName)
{
  RecordingExporter *exporter{};
  auto provider = MakeProvider(exporter);
  ASSERT_TRUE(exporter != nullptr);

  auto logger_disabled_by_update = provider->GetLogger("logger.disabled", "scope.disabled");
  auto logger_unaffected         = provider->GetLogger("logger.unaffected", "scope.unaffected");

  // Both loggers are initially enabled.
  ASSERT_TRUE(logger_disabled_by_update->Enabled(logs_api::Severity::kInfo));
  ASSERT_TRUE(logger_unaffected->Enabled(logs_api::Severity::kInfo));

  // Disable scope.disabled by name; scope.unaffected must remain enabled.
  provider->UpdateLoggerConfigurator(DisableByName("scope.disabled"));

  EXPECT_FALSE(logger_disabled_by_update->Enabled(logs_api::Severity::kInfo));
  EXPECT_TRUE(logger_unaffected->Enabled(logs_api::Severity::kInfo));

  // Verify that emission is suppressed for the disabled logger.
  exporter->Reset();
  logger_disabled_by_update->Info("should not appear");
  logger_unaffected->Info("should appear");
  EXPECT_EQ(exporter->RecordCount(), 1u);
}

TEST(LoggerProviderSDK, UpdateLoggerConfiguratorReEnable)
{
  RecordingExporter *exporter{};
  auto provider = MakeProvider(exporter, DisableAll());
  ASSERT_TRUE(exporter != nullptr);

  auto existing_logger = provider->GetLogger("existing", "scope.existing");

  ASSERT_FALSE(existing_logger->Enabled(logs_api::Severity::kInfo));

  // enable all loggers.
  provider->UpdateLoggerConfigurator(EnableAll());

  // The existing logger handle must immediately reflect the updated config.
  EXPECT_TRUE(existing_logger->Enabled(logs_api::Severity::kInfo));

  // Loggers obtained after the update also reflect the new configurator.
  auto logger_after = provider->GetLogger("after", "scope.after");
  EXPECT_TRUE(logger_after->Enabled(logs_api::Severity::kInfo));
}

TEST(LoggerProviderSDK, UpdateLoggerConfiguratorMinSeverityChange)
{
  RecordingExporter *exporter{};
  // Start with min severity = Info.
  auto provider = MakeProvider(exporter, AllAtSeverity(logs_api::Severity::kInfo));

  ASSERT_TRUE(exporter != nullptr);

  auto logger = provider->GetLogger("mylogger", "scope.test");

  // At Info threshold: Debug is suppressed, Info passes.
  EXPECT_FALSE(logger->Enabled(logs_api::Severity::kDebug));
  EXPECT_TRUE(logger->Enabled(logs_api::Severity::kInfo));
  EXPECT_TRUE(logger->Enabled(logs_api::Severity::kError));

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  // In ABIv2, emit filtering is implicit
  exporter->Reset();
  ASSERT_EQ(exporter->RecordCount(), 0u);
  logger->Debug("debug msg");  // filtered
  logger->Info("info msg");    // passes
  logger->Error("error msg");  // passes
  EXPECT_EQ(exporter->RecordCount(), 2u);
#endif

  // Raise min severity to Warn.
  provider->UpdateLoggerConfigurator(AllAtSeverity(logs_api::Severity::kWarn));

  EXPECT_FALSE(logger->Enabled(logs_api::Severity::kInfo));
  EXPECT_TRUE(logger->Enabled(logs_api::Severity::kWarn));
  EXPECT_TRUE(logger->Enabled(logs_api::Severity::kError));

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  exporter->Reset();
  ASSERT_EQ(exporter->RecordCount(), 0u);
  logger->Info("info msg");    // filtered
  logger->Warn("warn msg");    // passes
  logger->Error("error msg");  // passes
  EXPECT_EQ(exporter->RecordCount(), 2u);
#endif

  // Drop min severity to Debug.
  provider->UpdateLoggerConfigurator(AllAtSeverity(logs_api::Severity::kDebug));

  EXPECT_TRUE(logger->Enabled(logs_api::Severity::kDebug));

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  exporter->Reset();
  ASSERT_EQ(exporter->RecordCount(), 0u);
  logger->Debug("debug msg");  // passes
  logger->Info("info msg");    // passes
  EXPECT_EQ(exporter->RecordCount(), 2u);
#endif
}

TEST(LoggerProviderSDK, UpdateLoggerConfiguratorPerScopeMinSeverity)
{
  RecordingExporter *exporter{};
  // Default Info; "scope.verbose" gets Debug.
  auto provider =
      MakeProvider(exporter, InfoWithOverrideSeverity("scope.verbose", logs_api::Severity::kDebug));

  ASSERT_TRUE(exporter != nullptr);

  auto logger_default = provider->GetLogger("default", "scope.default");
  auto logger_verbose = provider->GetLogger("verbose", "scope.verbose");

  // scope.default filtered at Debug, passes at Info.
  EXPECT_FALSE(logger_default->Enabled(logs_api::Severity::kDebug));
  EXPECT_TRUE(logger_default->Enabled(logs_api::Severity::kInfo));

  // scope.verbose passes at Debug.
  EXPECT_TRUE(logger_verbose->Enabled(logs_api::Severity::kDebug));

  // Now raise scope.verbose to Warn; everything else stays at Info.
  provider->UpdateLoggerConfigurator(
      InfoWithOverrideSeverity("scope.verbose", logs_api::Severity::kWarn));

  EXPECT_FALSE(logger_verbose->Enabled(logs_api::Severity::kInfo));
  EXPECT_TRUE(logger_verbose->Enabled(logs_api::Severity::kWarn));
  EXPECT_TRUE(logger_default->Enabled(logs_api::Severity::kInfo));
}

TEST(LoggerProviderSDK, UpdateLoggerConfiguratorNewLoggerUsesUpdatedConfig)
{
  RecordingExporter *exporter{};
  auto provider = MakeProvider(exporter);
  ASSERT_TRUE(exporter != nullptr);

  // Install a configurator that disables "scope.disabled" before any logger for that scope exists.
  provider->UpdateLoggerConfigurator(DisableByName("scope.disabled"));

  // A logger obtained after the update must reflect the new config.
  auto logger_for_disabled_scope = provider->GetLogger("disabled", "scope.disabled");
  EXPECT_FALSE(logger_for_disabled_scope->Enabled(logs_api::Severity::kInfo));

  // Scopes not named in the configurator remain enabled.
  auto logger_for_unaffected_scope = provider->GetLogger("unaffected", "scope.unaffected");
  EXPECT_TRUE(logger_for_unaffected_scope->Enabled(logs_api::Severity::kInfo));
}

TEST(LoggerProviderSDK, UpdateLoggerConfiguratorNullIgnored)
{
  RecordingExporter *exporter{};
  auto provider = MakeProvider(exporter, AllAtSeverity(logs_api::Severity::kInfo));
  ASSERT_TRUE(exporter != nullptr);

  auto logger = provider->GetLogger("mylogger", "scope.test");
  ASSERT_TRUE(logger->Enabled(logs_api::Severity::kInfo));

  // Passing nullptr must be a no-op (logged as an error internally, config unchanged).
  provider->UpdateLoggerConfigurator(nullptr);

  EXPECT_TRUE(logger->Enabled(logs_api::Severity::kInfo));
}

TEST(LoggerProviderSDK, UpdateLoggerConfiguratorConcurrentEmit)
{
  RecordingExporter *exporter{};
  auto provider = MakeProvider(exporter, AllAtSeverity(logs_api::Severity::kInfo));
  ASSERT_TRUE(exporter != nullptr);

  auto logger = provider->GetLogger("mylogger", "scope.concurrent");

  std::atomic<bool> stop{false};
  std::atomic<bool> worker_saw_suppressed{false};
  std::atomic<bool> worker_saw_emitted{false};

  std::promise<void> worker_ready;
  std::future<void> worker_ready_future = worker_ready.get_future();

  // Worker: call Enabled() and emit in a tight loop; flag each observed state.
  std::thread worker([&] {
    worker_ready.set_value();
    while (!stop.load(std::memory_order_relaxed))
    {
      if (logger->Enabled(logs_api::Severity::kInfo))
      {
        logger->Info("emit");
        worker_saw_emitted.store(true, std::memory_order_relaxed);
      }
      else
      {
        worker_saw_suppressed.store(true, std::memory_order_relaxed);
      }
    }
  });

  worker_ready_future.wait();

  // Raise min severity so Info is suppressed; wait for worker to observe it.
  provider->UpdateLoggerConfigurator(AllAtSeverity(logs_api::Severity::kError));
  while (!worker_saw_suppressed.load(std::memory_order_relaxed))
  {
    std::this_thread::yield();
  }

  // Lower min severity back to Info; wait for worker to observe it.
  provider->UpdateLoggerConfigurator(AllAtSeverity(logs_api::Severity::kInfo));
  while (!worker_saw_emitted.load(std::memory_order_relaxed))
  {
    std::this_thread::yield();
  }

  stop.store(true, std::memory_order_relaxed);
  worker.join();
}
