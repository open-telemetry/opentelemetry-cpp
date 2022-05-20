// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include <array>
#  include "opentelemetry/logs/provider.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/logs/log_record.h"
#  include "opentelemetry/sdk/logs/logger.h"
#  include "opentelemetry/sdk/logs/logger_provider.h"
#  include "opentelemetry/sdk/logs/simple_log_processor.h"

#  include <gtest/gtest.h>

using namespace opentelemetry::sdk::logs;
namespace logs_api = opentelemetry::logs;
namespace nostd    = opentelemetry::nostd;

TEST(LoggerProviderSDK, PushToAPI)
{
  auto lp =
      nostd::shared_ptr<logs_api::LoggerProvider>(new opentelemetry::sdk::logs::LoggerProvider());
  logs_api::Provider::SetLoggerProvider(lp);

  // Check that the loggerprovider was correctly pushed into the API
  ASSERT_EQ(lp, logs_api::Provider::GetLoggerProvider());
}

TEST(LoggerProviderSDK, LoggerProviderGetLoggerSimple)
{
  auto lp = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());

  nostd::string_view schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger1 = lp->GetLogger("logger1", "", "opentelelemtry_library", "", schema_url);
  auto logger2 = lp->GetLogger("logger2", "", "", "", schema_url);

  // Check that the logger is not nullptr
  ASSERT_NE(logger1, nullptr);
  ASSERT_NE(logger2, nullptr);

  auto sdk_logger1 = static_cast<opentelemetry::sdk::logs::Logger *>(logger1.get());
  auto sdk_logger2 = static_cast<opentelemetry::sdk::logs::Logger *>(logger2.get());
  ASSERT_EQ(sdk_logger1->GetInstrumentationLibrary().GetName(), "opentelelemtry_library");
  ASSERT_EQ(sdk_logger1->GetInstrumentationLibrary().GetVersion(), "");
  ASSERT_EQ(sdk_logger1->GetInstrumentationLibrary().GetSchemaURL(), schema_url);

  ASSERT_EQ(sdk_logger2->GetInstrumentationLibrary().GetName(), "logger2");
  ASSERT_EQ(sdk_logger2->GetInstrumentationLibrary().GetVersion(), "");
  ASSERT_EQ(sdk_logger2->GetInstrumentationLibrary().GetSchemaURL(), schema_url);

  // Check that two loggers with different names aren't the same instance
  ASSERT_NE(logger1, logger2);

  // Check that two loggers with the same name are the same instance
  auto logger3 = lp->GetLogger("logger1", "", "opentelelemtry_library", "", schema_url);
  ASSERT_EQ(logger1, logger3);
  auto sdk_logger3 = static_cast<opentelemetry::sdk::logs::Logger *>(logger3.get());
  ASSERT_EQ(sdk_logger3->GetInstrumentationLibrary(), sdk_logger1->GetInstrumentationLibrary());
}

TEST(LoggerProviderSDK, LoggerProviderLoggerArguments)
{
  // Currently, arguments are not supported by the loggers.
  // TODO: Once the logging spec defines what arguments are allowed, add more
  // detail to this test
  auto lp = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());

  nostd::string_view schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger1 = lp->GetLogger("logger1", "", "opentelelemtry_library", "", schema_url);

  // Check GetLogger(logger_name, args)
  std::array<nostd::string_view, 1> sv{"string"};
  nostd::span<nostd::string_view> args{sv};
  auto logger2     = lp->GetLogger("logger2", args, "opentelelemtry_library", "", schema_url);
  auto sdk_logger1 = static_cast<opentelemetry::sdk::logs::Logger *>(logger1.get());
  auto sdk_logger2 = static_cast<opentelemetry::sdk::logs::Logger *>(logger2.get());
  ASSERT_EQ(sdk_logger2->GetInstrumentationLibrary(), sdk_logger1->GetInstrumentationLibrary());
}

class DummyProcessor : public LogProcessor
{
  std::unique_ptr<Recordable> MakeRecordable() noexcept
  {
    return std::unique_ptr<Recordable>(new LogRecord);
  }

  void OnReceive(std::unique_ptr<Recordable> &&record) noexcept {}
  bool ForceFlush(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept
  {
    return true;
  }
  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept
  {
    return true;
  }
};

TEST(LoggerProviderSDK, GetResource)
{
  // Create a LoggerProvider without a processor
  auto resource = opentelemetry::sdk::resource::Resource::Create({{"key", "value"}});
  LoggerProvider lp{nullptr, resource};
  ASSERT_EQ(nostd::get<std::string>(lp.GetResource().GetAttributes().at("key")), "value");
}

TEST(LoggerProviderSDK, Shutdown)
{
  std::unique_ptr<SimpleLogProcessor> processor(new SimpleLogProcessor(nullptr));
  std::vector<std::unique_ptr<LogProcessor>> processors;
  processors.push_back(std::move(processor));

  LoggerProvider lp(std::make_shared<LoggerContext>(std::move(processors)));

  EXPECT_TRUE(lp.Shutdown());

  // It's safe to shutdown again
  EXPECT_TRUE(lp.Shutdown());
}

TEST(LoggerProviderSDK, ForceFlush)
{
  std::unique_ptr<SimpleLogProcessor> processor(new SimpleLogProcessor(nullptr));
  std::vector<std::unique_ptr<LogProcessor>> processors;
  processors.push_back(std::move(processor));

  LoggerProvider lp(std::make_shared<LoggerContext>(std::move(processors)));

  EXPECT_TRUE(lp.ForceFlush());
}

#endif
