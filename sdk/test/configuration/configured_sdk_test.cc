// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/context/propagation/global_propagator.h"
#include "opentelemetry/context/propagation/noop_propagator.h"
#include "opentelemetry/logs/noop.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/provider.h"

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configured_sdk.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/logger_provider_configuration.h"
#include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/resource_configuration.h"
#include "opentelemetry/sdk/configuration/severity_number.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"

#include "config_test_common.h"

namespace trace        = opentelemetry::trace;
namespace logs         = opentelemetry::logs;
namespace metrics      = opentelemetry::metrics;
namespace propagation  = opentelemetry::context::propagation;
namespace config_sdk   = opentelemetry::sdk::configuration;
namespace internal_log = opentelemetry::sdk::common::internal_log;

namespace
{

//------------------------------------------------------------------------------
// ConfiguredSdkTest fixture

class ConfiguredSdkTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    MakeRegistry();
    SetNullProviders();
  }

  void TearDown() override
  {
    if (sdk_)
    {
      sdk_->UnInstall();
    }
    SetNoopProviders();
  }

  void SetNullProviders()
  {
    propagation::GlobalTextMapPropagator::SetGlobalPropagator({});
    trace::Provider::SetTracerProvider({});
    logs::Provider::SetLoggerProvider({});
    metrics::Provider::SetMeterProvider({});

    ASSERT_EQ(propagation::GlobalTextMapPropagator::GetGlobalPropagator(), nullptr);
    ASSERT_EQ(trace::Provider::GetTracerProvider(), nullptr);
    ASSERT_EQ(logs::Provider::GetLoggerProvider(), nullptr);
    ASSERT_EQ(metrics::Provider::GetMeterProvider(), nullptr);
  }

  void SetNoopProviders()
  {
    propagation::GlobalTextMapPropagator::SetGlobalPropagator(
        {std::make_shared<propagation::NoOpPropagator>()});
    trace::Provider::SetTracerProvider({std::make_shared<trace::NoopTracerProvider>()});
    logs::Provider::SetLoggerProvider({std::make_shared<logs::NoopLoggerProvider>()});
    metrics::Provider::SetMeterProvider({std::make_shared<metrics::NoopMeterProvider>()});

    ASSERT_NE(propagation::GlobalTextMapPropagator::GetGlobalPropagator(), nullptr);
    ASSERT_NE(trace::Provider::GetTracerProvider(), nullptr);
    ASSERT_NE(logs::Provider::GetLoggerProvider(), nullptr);
    ASSERT_NE(metrics::Provider::GetMeterProvider(), nullptr);
  }

  void CreateSdk(const std::unique_ptr<config_sdk::Configuration> &model)
  {
    ASSERT_TRUE(sdk_ == nullptr);
    ASSERT_NO_THROW(sdk_ = config_sdk::ConfiguredSdk::Create(registry_, model));
    ASSERT_FALSE(sdk_ == nullptr);
  }

  void MakeRegistry()
  {
    registry_ = std::make_shared<config_sdk::Registry>();
    registry_->SetExtensionSpanExporterBuilder(
        "noop", std::make_unique<config_test::NoopSpanExporterBuilder>());
    registry_->SetExtensionLogRecordExporterBuilder(
        "noop", std::make_unique<config_test::NoopLogRecordExporterBuilder>());
    registry_->SetExtensionPushMetricExporterBuilder(
        "noop", std::make_unique<config_test::NoopPushMetricExporterBuilder>());
    registry_->SetPeriodicMetricReaderBuilder(
        std::make_unique<config_test::NoopPeriodicMetricReaderBuilder>());
  }

  static std::unique_ptr<config_sdk::TracerProviderConfiguration> MakeTracerProviderConfig()
  {
    auto exporter       = std::make_unique<config_sdk::ExtensionSpanExporterConfiguration>();
    exporter->name      = "noop";
    auto processor      = std::make_unique<config_sdk::SimpleSpanProcessorConfiguration>();
    processor->exporter = std::move(exporter);
    auto config         = std::make_unique<config_sdk::TracerProviderConfiguration>();
    config->processors.emplace_back(std::move(processor));
    return config;
  }

  static std::unique_ptr<config_sdk::LoggerProviderConfiguration> MakeLoggerProviderConfig()
  {
    auto exporter       = std::make_unique<config_sdk::ExtensionLogRecordExporterConfiguration>();
    exporter->name      = "noop";
    auto processor      = std::make_unique<config_sdk::SimpleLogRecordProcessorConfiguration>();
    processor->exporter = std::move(exporter);
    auto config         = std::make_unique<config_sdk::LoggerProviderConfiguration>();
    config->processors.emplace_back(std::move(processor));
    return config;
  }

  static std::unique_ptr<config_sdk::MeterProviderConfiguration> MakeMeterProviderConfig()
  {
    auto exporter    = std::make_unique<config_sdk::ExtensionPushMetricExporterConfiguration>();
    exporter->name   = "noop";
    auto reader      = std::make_unique<config_sdk::PeriodicMetricReaderConfiguration>();
    reader->exporter = std::move(exporter);
    reader->interval = 3'600'000;  // milliseconds. Set to a large value and rely on ForceFlush to
                                   // trigger collection.
    reader->timeout = 60'000;      // milliseconds
    auto config     = std::make_unique<config_sdk::MeterProviderConfiguration>();
    config->readers.emplace_back(std::move(reader));
    return config;
  }

  std::shared_ptr<config_sdk::Registry> registry_;
  std::unique_ptr<config_sdk::ConfiguredSdk> sdk_;
};

}  // namespace

//------------------------------------------------------------------------------
// ConfiguredSdk Tests. These are intended to cover just the API and implementation of the
// ConfiguredSdk class.
//                      For integration testing to determine if the SDK is configured correctly, see
//                      the programmatic_configuration_test.cc file

TEST_F(ConfiguredSdkTest, ConfiguredSdkDefaultLogLevel)
{
  // default log level is info
  auto model      = std::make_unique<config_sdk::Configuration>();
  model->resource = std::make_unique<config_sdk::ResourceConfiguration>();
  CreateSdk(model);
  sdk_->Install();
  EXPECT_EQ(sdk_->log_level, internal_log::LogLevel::Info);
  EXPECT_EQ(internal_log::GlobalLogHandler::GetLogLevel(), internal_log::LogLevel::Info);
}

TEST_F(ConfiguredSdkTest, ConfiguredSdkSetLogLevel)
{
  auto model       = std::make_unique<config_sdk::Configuration>();
  model->resource  = std::make_unique<config_sdk::ResourceConfiguration>();
  model->log_level = config_sdk::SeverityNumber::debug;
  CreateSdk(model);
  sdk_->Install();
  EXPECT_EQ(sdk_->log_level, internal_log::LogLevel::Debug);
  EXPECT_EQ(internal_log::GlobalLogHandler::GetLogLevel(), internal_log::LogLevel::Debug);
}

TEST_F(ConfiguredSdkTest, ConfiguredSdkInstallUninstall)
{
  auto model                        = std::make_unique<config_sdk::Configuration>();
  model->resource                   = std::make_unique<config_sdk::ResourceConfiguration>();
  model->tracer_provider            = MakeTracerProviderConfig();
  model->logger_provider            = MakeLoggerProviderConfig();
  model->meter_provider             = MakeMeterProviderConfig();
  model->propagator                 = std::make_unique<config_sdk::PropagatorConfiguration>();
  model->propagator->composite_list = "tracecontext,baggage,b3,b3multi,jaeger";

  CreateSdk(model);
  sdk_->Install();
  EXPECT_NE(trace::Provider::GetTracerProvider(), nullptr);
  EXPECT_NE(logs::Provider::GetLoggerProvider(), nullptr);
  EXPECT_NE(metrics::Provider::GetMeterProvider(), nullptr);
  EXPECT_NE(propagation::GlobalTextMapPropagator::GetGlobalPropagator(), nullptr);

  sdk_->UnInstall();
  EXPECT_EQ(trace::Provider::GetTracerProvider(), nullptr);
  EXPECT_EQ(logs::Provider::GetLoggerProvider(), nullptr);
  EXPECT_EQ(metrics::Provider::GetMeterProvider(), nullptr);
  EXPECT_EQ(propagation::GlobalTextMapPropagator::GetGlobalPropagator(), nullptr);
}
