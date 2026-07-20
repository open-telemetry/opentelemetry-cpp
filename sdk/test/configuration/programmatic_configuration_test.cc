// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/baggage/baggage.h"
#include "opentelemetry/baggage/baggage_context.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/context/propagation/global_propagator.h"
#include "opentelemetry/context/propagation/noop_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/logs/logger.h"
#include "opentelemetry/logs/logger_provider.h"
#include "opentelemetry/logs/noop.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/context.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/trace/tracer_provider.h"

#include "opentelemetry/sdk/configuration/aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/base2_exponential_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configured_sdk.h"
#include "opentelemetry/sdk/configuration/explicit_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/instrument_type.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_limits_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/logger_config_configuration.h"
#include "opentelemetry/sdk/configuration/logger_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/logger_matcher_and_config_configuration.h"
#include "opentelemetry/sdk/configuration/logger_provider_configuration.h"
#include "opentelemetry/sdk/configuration/meter_config_configuration.h"
#include "opentelemetry/sdk/configuration/meter_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/meter_matcher_and_config_configuration.h"
#include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/resource_configuration.h"
#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/sdk/configuration/severity_number.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_config_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_matcher_and_config_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/sdk/configuration/view_configuration.h"
#include "opentelemetry/sdk/configuration/view_selector_configuration.h"
#include "opentelemetry/sdk/configuration/view_stream_configuration.h"

#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"

namespace nostd       = opentelemetry::nostd;
namespace trace       = opentelemetry::trace;
namespace logs        = opentelemetry::logs;
namespace metrics     = opentelemetry::metrics;
namespace common      = opentelemetry::common;
namespace baggage     = opentelemetry::baggage;
namespace propagation = opentelemetry::context::propagation;
namespace context     = opentelemetry::context;
namespace common_sdk  = opentelemetry::sdk::common;
namespace logs_sdk    = opentelemetry::sdk::logs;
namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace trace_sdk   = opentelemetry::sdk::trace;
namespace config_sdk  = opentelemetry::sdk::configuration;

namespace
{
// ---------------------------------------------------------------------------
// Shared export buffers

using LogRecordBuffer = std::vector<std::unique_ptr<logs_sdk::ReadWriteLogRecord>>;
using SpanBuffer      = std::vector<std::unique_ptr<trace_sdk::SpanData>>;
using MetricBuffer    = std::vector<metrics_sdk::MetricData>;

// ---------------------------------------------------------------------------
// Recording exporters to support integration testing of the configured SDK.
// These exporters record the data they receive into a buffer for later inspection.

class RecordingSpanExporter : public trace_sdk::SpanExporter
{
public:
  explicit RecordingSpanExporter(std::shared_ptr<SpanBuffer> buffer) : buffer_(std::move(buffer)) {}

  std::unique_ptr<trace_sdk::Recordable> MakeRecordable() noexcept override
  {
    return std::make_unique<trace_sdk::SpanData>();
  }

  common_sdk::ExportResult Export(
      const nostd::span<std::unique_ptr<trace_sdk::Recordable>> &spans) noexcept override
  {
    for (auto &span : spans)
    {
      buffer_->emplace_back(static_cast<trace_sdk::SpanData *>(span.release()));
    }
    return common_sdk::ExportResult::kSuccess;
  }

  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }

private:
  std::shared_ptr<SpanBuffer> buffer_;
};

class RecordingLogRecordExporter : public logs_sdk::LogRecordExporter
{
public:
  explicit RecordingLogRecordExporter(std::shared_ptr<LogRecordBuffer> buffer)
      : buffer_(std::move(buffer))
  {}

  std::unique_ptr<logs_sdk::Recordable> MakeRecordable() noexcept override
  {
    return std::make_unique<logs_sdk::ReadWriteLogRecord>();
  }

  common_sdk::ExportResult Export(
      const nostd::span<std::unique_ptr<logs_sdk::Recordable>> &records) noexcept override
  {
    for (auto &rec : records)
    {
      buffer_->emplace_back(static_cast<logs_sdk::ReadWriteLogRecord *>(rec.release()));
    }
    return common_sdk::ExportResult::kSuccess;
  }

  bool RecordableEnforcesLogRecordLimits() const noexcept override { return true; }

  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }

private:
  std::shared_ptr<LogRecordBuffer> buffer_;
};

class RecordingPushMetricExporter : public metrics_sdk::PushMetricExporter
{
public:
  explicit RecordingPushMetricExporter(std::shared_ptr<MetricBuffer> buffer)
      : buffer_(std::move(buffer))
  {}

  common_sdk::ExportResult Export(
      const metrics_sdk::ResourceMetrics &resource_metrics) noexcept override
  {
    for (const auto &scope : resource_metrics.scope_metric_data_)
    {
      for (const auto &metric : scope.metric_data_)
      {
        buffer_->emplace_back(metric);
      }
    }
    return common_sdk::ExportResult::kSuccess;
  }

  metrics_sdk::AggregationTemporality GetAggregationTemporality(
      metrics_sdk::InstrumentType) const noexcept override
  {
    return metrics_sdk::AggregationTemporality::kCumulative;
  }

  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }

private:
  std::shared_ptr<MetricBuffer> buffer_;
};

// ---------------------------------------------------------------------------
// Configuration Builders for recording exporters

class RecordingSpanExporterBuilder : public config_sdk::ExtensionSpanExporterBuilder
{
public:
  explicit RecordingSpanExporterBuilder(std::shared_ptr<SpanBuffer> buffer)
      : buffer_(std::move(buffer))
  {}
  std::unique_ptr<trace_sdk::SpanExporter> Build(
      const config_sdk::ExtensionSpanExporterConfiguration *) const override
  {
    return std::make_unique<RecordingSpanExporter>(buffer_);
  }

private:
  std::shared_ptr<SpanBuffer> buffer_;
};

class RecordingLogRecordExporterBuilder : public config_sdk::ExtensionLogRecordExporterBuilder
{
public:
  explicit RecordingLogRecordExporterBuilder(std::shared_ptr<LogRecordBuffer> buffer)
      : buffer_(std::move(buffer))
  {}
  std::unique_ptr<logs_sdk::LogRecordExporter> Build(
      const config_sdk::ExtensionLogRecordExporterConfiguration *) const override
  {
    return std::make_unique<RecordingLogRecordExporter>(buffer_);
  }

private:
  std::shared_ptr<LogRecordBuffer> buffer_;
};

class RecordingPushMetricExporterBuilder : public config_sdk::ExtensionPushMetricExporterBuilder
{
public:
  explicit RecordingPushMetricExporterBuilder(std::shared_ptr<MetricBuffer> buffer)
      : buffer_(std::move(buffer))
  {}
  std::unique_ptr<metrics_sdk::PushMetricExporter> Build(
      const config_sdk::ExtensionPushMetricExporterConfiguration *) const override
  {
    auto exporter = std::make_unique<RecordingPushMetricExporter>(buffer_);
    return exporter;
  }

private:
  std::shared_ptr<MetricBuffer> buffer_;
};

// ---------------------------------------------------------------------------
// TextMapCarrier for propagator tests.

class MapCarrier : public propagation::TextMapCarrier
{
public:
  nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    auto it = map_.find(std::string(key));
    return it != map_.end() ? nostd::string_view(it->second) : "";
  }
  void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    map_[std::string(key)] = std::string(value);
  }

  const std::map<std::string, std::string> &map() const { return map_; }

private:
  std::map<std::string, std::string> map_;
};

//---------------------------------------------------------------------------
// ProgrammaticConfigTest fixture: This supports integration testing of the configured SDK.
// It registers the recording exporters and maintains buffers for inspection of exported signal
// data.

class ProgrammaticConfigTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    MakeRegistry();

    propagation::GlobalTextMapPropagator::SetGlobalPropagator(
        {std::make_shared<propagation::NoOpPropagator>()});
    trace::Provider::SetTracerProvider({std::make_shared<trace::NoopTracerProvider>()});
    logs::Provider::SetLoggerProvider({std::make_shared<logs::NoopLoggerProvider>()});
    metrics::Provider::SetMeterProvider({std::make_shared<metrics::NoopMeterProvider>()});
  }

  void TearDown() override
  {
    if (sdk_)
    {
      sdk_->UnInstall();
    }
  }

  void CreateAndInstallSdk(const std::unique_ptr<config_sdk::Configuration> &model)
  {
    ASSERT_TRUE(sdk_ == nullptr);
    ASSERT_NO_THROW(sdk_ = config_sdk::ConfiguredSdk::Create(registry_, model));
    ASSERT_FALSE(sdk_ == nullptr);
    sdk_->Install();
  }

  void MakeRegistry()
  {
    registry_ = std::make_shared<config_sdk::Registry>();
    registry_->SetExtensionSpanExporterBuilder(
        "recording", std::make_unique<RecordingSpanExporterBuilder>(span_buffer_));
    registry_->SetExtensionLogRecordExporterBuilder(
        "recording", std::make_unique<RecordingLogRecordExporterBuilder>(log_buffer_));
    registry_->SetExtensionPushMetricExporterBuilder(
        "recording", std::make_unique<RecordingPushMetricExporterBuilder>(metric_buffer_));
  }

  static std::unique_ptr<config_sdk::TracerProviderConfiguration> MakeTracerProviderConfig()
  {
    auto exporter       = std::make_unique<config_sdk::ExtensionSpanExporterConfiguration>();
    exporter->name      = "recording";
    auto processor      = std::make_unique<config_sdk::SimpleSpanProcessorConfiguration>();
    processor->exporter = std::move(exporter);
    auto config         = std::make_unique<config_sdk::TracerProviderConfiguration>();
    config->processors.emplace_back(std::move(processor));
    return config;
  }

  static std::unique_ptr<config_sdk::LoggerProviderConfiguration> MakeLoggerProviderConfig()
  {
    auto exporter       = std::make_unique<config_sdk::ExtensionLogRecordExporterConfiguration>();
    exporter->name      = "recording";
    auto processor      = std::make_unique<config_sdk::SimpleLogRecordProcessorConfiguration>();
    processor->exporter = std::move(exporter);
    auto config         = std::make_unique<config_sdk::LoggerProviderConfiguration>();
    config->processors.emplace_back(std::move(processor));
    return config;
  }

  static std::unique_ptr<config_sdk::MeterProviderConfiguration> MakeMeterProviderConfig()
  {
    auto exporter    = std::make_unique<config_sdk::ExtensionPushMetricExporterConfiguration>();
    exporter->name   = "recording";
    auto reader      = std::make_unique<config_sdk::PeriodicMetricReaderConfiguration>();
    reader->exporter = std::move(exporter);
    reader->interval = 3'600'000;  // milliseconds. Set to a large value and rely on ForceFlush to
                                   // trigger collection.
    reader->timeout = 60'000;      // milliseconds
    auto config     = std::make_unique<config_sdk::MeterProviderConfiguration>();
    config->readers.emplace_back(std::move(reader));
    return config;
  }

  std::shared_ptr<SpanBuffer> span_buffer_{std::make_shared<SpanBuffer>()};
  std::shared_ptr<LogRecordBuffer> log_buffer_{std::make_shared<LogRecordBuffer>()};
  std::shared_ptr<MetricBuffer> metric_buffer_{std::make_shared<MetricBuffer>()};

  std::shared_ptr<config_sdk::Registry> registry_;
  std::unique_ptr<config_sdk::ConfiguredSdk> sdk_;
};

}  // namespace

//---------------------------------------------------------------------------
// Resource configuration tests

TEST_F(ProgrammaticConfigTest, ResourceAttributesFromList)
{
  auto resource_config             = std::make_unique<config_sdk::ResourceConfiguration>();
  resource_config->attributes_list = "service.name=test-service,service.version=1.0";
  auto model                       = std::make_unique<config_sdk::Configuration>();
  model->resource                  = std::move(resource_config);

  CreateAndInstallSdk(model);

  const auto &attributes = sdk_->resource.GetAttributes();
  ASSERT_NE(attributes.find("service.name"), attributes.end());
  EXPECT_EQ(nostd::get<std::string>(attributes.at("service.name")), "test-service");
  ASSERT_NE(attributes.find("service.version"), attributes.end());
  EXPECT_EQ(nostd::get<std::string>(attributes.at("service.version")), "1.0");
}

//--------------------------------------------------------------------------
// Disabled SDK configuration tests

TEST_F(ProgrammaticConfigTest, DisabledConfigProducesNullProviders)
{
  auto model             = std::make_unique<config_sdk::Configuration>();
  model->disabled        = true;
  model->tracer_provider = MakeTracerProviderConfig();
  model->logger_provider = MakeLoggerProviderConfig();
  model->meter_provider  = MakeMeterProviderConfig();

  CreateAndInstallSdk(model);

  EXPECT_EQ(sdk_->tracer_provider, nullptr);
  EXPECT_EQ(sdk_->logger_provider, nullptr);
  EXPECT_EQ(sdk_->meter_provider, nullptr);
  EXPECT_EQ(sdk_->propagator, nullptr);
}

TEST_F(ProgrammaticConfigTest, EnabledConfigProducesProviders)
{
  auto model                        = std::make_unique<config_sdk::Configuration>();
  model->disabled                   = false;
  model->tracer_provider            = MakeTracerProviderConfig();
  model->logger_provider            = MakeLoggerProviderConfig();
  model->meter_provider             = MakeMeterProviderConfig();
  model->propagator                 = std::make_unique<config_sdk::PropagatorConfiguration>();
  model->propagator->composite_list = "tracecontext";

  CreateAndInstallSdk(model);

  EXPECT_NE(sdk_->tracer_provider, nullptr);
  EXPECT_NE(sdk_->logger_provider, nullptr);
  EXPECT_NE(sdk_->meter_provider, nullptr);
  EXPECT_NE(sdk_->propagator, nullptr);
}

//---------------------------------------------------------------------------
// LoggerProvider tests

TEST_F(ProgrammaticConfigTest, LoggerProviderWithDefaults)
{
  auto logger_provider_config = MakeLoggerProviderConfig();

  auto model             = std::make_unique<config_sdk::Configuration>();
  model->logger_provider = std::move(logger_provider_config);

  CreateAndInstallSdk(model);

  ASSERT_NE(sdk_->logger_provider, nullptr);

  auto logger = logs::Provider::GetLoggerProvider()->GetLogger("test");
  logger->EmitLogRecord(
      logs::Severity::kInfo, nostd::string_view("test-message"),
      common::MakeAttributes({{"key1", "value1"}, {"key2", "value2"}, {"key3", "value3"}}));

  ASSERT_TRUE(sdk_->logger_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->logger_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_GE(log_buffer_->size(), 1);
}

TEST_F(ProgrammaticConfigTest, LoggerProviderWithLogRecordLimits)
{
  config_sdk::LogRecordLimitsConfiguration limits{
      0, 0};  // TODO: Remove the default initialization once the limit members are initialized.
  limits.attribute_count_limit        = 2;
  limits.attribute_value_length_limit = 5;

  auto logger_provider_config = MakeLoggerProviderConfig();
  logger_provider_config->limits =
      std::make_unique<config_sdk::LogRecordLimitsConfiguration>(limits);

  auto model             = std::make_unique<config_sdk::Configuration>();
  model->logger_provider = std::move(logger_provider_config);

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->logger_provider, nullptr);

  auto logger = logs::Provider::GetLoggerProvider()->GetLogger("test");
  logger->EmitLogRecord(
      logs::Severity::kInfo, nostd::string_view("test-message"),
      common::MakeAttributes({{"key1", "value1"}, {"key2", "value2"}, {"key3", "value3"}}));

  ASSERT_TRUE(sdk_->logger_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->logger_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_GE(log_buffer_->size(), 1);
  auto *record = log_buffer_->front().get();
  EXPECT_EQ(nostd::get<std::string>(record->GetBody()), "test-message");
  const auto &attributes = record->GetAttributes();
  EXPECT_EQ(attributes.size(), limits.attribute_count_limit);
  for (const auto &attr : attributes)
  {
    EXPECT_EQ(nostd::get<std::string>(attr.second).size(), limits.attribute_value_length_limit);
  }
}

TEST_F(ProgrammaticConfigTest, LoggerProviderWithLoggerConfigurator)
{
  auto error_logger_matcher                    = config_sdk::LoggerMatcherAndConfigConfiguration();
  error_logger_matcher.name                    = "error_logger";
  error_logger_matcher.config.enabled          = true;
  error_logger_matcher.config.minimum_severity = config_sdk::SeverityNumber::error;

  auto logger_configurator = std::make_unique<config_sdk::LoggerConfiguratorConfiguration>();
  logger_configurator->default_config.enabled          = true;
  logger_configurator->default_config.minimum_severity = config_sdk::SeverityNumber::info;
  logger_configurator->loggers.emplace_back(std::move(error_logger_matcher));

  auto logger_provider_config                 = MakeLoggerProviderConfig();
  logger_provider_config->logger_configurator = std::move(logger_configurator);

  auto model             = std::make_unique<config_sdk::Configuration>();
  model->logger_provider = std::move(logger_provider_config);

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->logger_provider, nullptr);

  // The default logger should be enabled and have a minimum severity of info
  auto default_logger = logs::Provider::GetLoggerProvider()->GetLogger("default_logger");
  default_logger->EmitLogRecord(
      logs::Severity::kInfo, nostd::string_view("test-message"),
      common::MakeAttributes({{"key1", "value1"}, {"key2", "value2"}, {"key3", "value3"}}));
  EXPECT_TRUE(default_logger->Enabled(logs::Severity::kInfo));

  // The error_logger should be enabled and have a minimum severity of error
  auto error_logger = logs::Provider::GetLoggerProvider()->GetLogger("error_logger");
  error_logger->EmitLogRecord(logs::Severity::kError, nostd::string_view("test-message"));

  EXPECT_FALSE(error_logger->Enabled(logs::Severity::kInfo));
  EXPECT_TRUE(error_logger->Enabled(logs::Severity::kError));

  ASSERT_TRUE(sdk_->logger_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->logger_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_GE(log_buffer_->size(), 2);
}

TEST_F(ProgrammaticConfigTest, LoggerProviderWithBatchProcessorDefaults)
{
  auto exporter       = std::make_unique<config_sdk::ExtensionLogRecordExporterConfiguration>();
  exporter->name      = "recording";
  auto processor      = std::make_unique<config_sdk::BatchLogRecordProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  auto logger_provider_config = std::make_unique<config_sdk::LoggerProviderConfiguration>();
  logger_provider_config->processors.emplace_back(std::move(processor));

  auto model             = std::make_unique<config_sdk::Configuration>();
  model->logger_provider = std::move(logger_provider_config);

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->logger_provider, nullptr);

  logs::Provider::GetLoggerProvider()->GetLogger("test")->Info("test-message");
  ASSERT_TRUE(sdk_->logger_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->logger_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_GE(log_buffer_->size(), 1);
}

TEST_F(ProgrammaticConfigTest, LoggerProviderWithBatchProcessorConfigured)
{
  auto exporter       = std::make_unique<config_sdk::ExtensionLogRecordExporterConfiguration>();
  exporter->name      = "recording";
  auto processor      = std::make_unique<config_sdk::BatchLogRecordProcessorConfiguration>();
  processor->exporter = std::move(exporter);
  processor->schedule_delay        = 60000;
  processor->max_queue_size        = 100;
  processor->max_export_batch_size = 50;
  processor->export_timeout        = 5000;
  auto logger_provider_config      = std::make_unique<config_sdk::LoggerProviderConfiguration>();
  logger_provider_config->processors.emplace_back(std::move(processor));

  auto model             = std::make_unique<config_sdk::Configuration>();
  model->logger_provider = std::move(logger_provider_config);

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->logger_provider, nullptr);

  logs::Provider::GetLoggerProvider()->GetLogger("test")->Info("test-message");
  ASSERT_TRUE(sdk_->logger_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->logger_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_GE(log_buffer_->size(), 1);
}

//--------------------------------------------------------------------------
// MeterProvider tests

// TODO: These test cases may timeout due to threading in the PeriodicExportingMetricReader
// that cause ForceFlush or Shutdown to block indefinitely. Disabling for now until we can fix the
// underlying issue.
TEST_F(ProgrammaticConfigTest, DISABLED_MeterProviderWithDefaults)
{
  auto model            = std::make_unique<config_sdk::Configuration>();
  model->meter_provider = MakeMeterProviderConfig();

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->meter_provider, nullptr);

  metrics::Provider::GetMeterProvider()
      ->GetMeter("test")
      ->CreateUInt64Counter("test-counter")
      ->Add(1);

  ASSERT_TRUE(sdk_->meter_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->meter_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_GE(metric_buffer_->size(), 1);
}

TEST_F(ProgrammaticConfigTest, DISABLED_MeterProviderWithMeterConfigurator)
{
  auto disabled_meter_config           = config_sdk::MeterMatcherAndConfigConfiguration();
  disabled_meter_config.name           = "disabled-meter";
  disabled_meter_config.config.enabled = false;

  auto meter_configurator = std::make_unique<config_sdk::MeterConfiguratorConfiguration>();
  meter_configurator->default_config.enabled = true;
  meter_configurator->meters.push_back(disabled_meter_config);

  auto model                                = std::make_unique<config_sdk::Configuration>();
  model->meter_provider                     = MakeMeterProviderConfig();
  model->meter_provider->meter_configurator = std::move(meter_configurator);

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->meter_provider, nullptr);

  auto default_meter = metrics::Provider::GetMeterProvider()->GetMeter("default-meter");
  default_meter->CreateUInt64Counter("test-counter")->Add(1);

  auto disabled_meter = metrics::Provider::GetMeterProvider()->GetMeter(disabled_meter_config.name);
  disabled_meter->CreateUInt64Counter("disabled-test-counter")->Add(1);

  ASSERT_TRUE(sdk_->meter_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->meter_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_GE(metric_buffer_->size(), 1);
  for (const auto &metric : *metric_buffer_)
  {
    EXPECT_NE(metric.instrument_descriptor.name_, "disabled-test-counter");
  }
}

TEST_F(ProgrammaticConfigTest, DISABLED_MeterProviderWithViews)
{
  // View 1: Base2 exponential aggregation
  const std::size_t max_scale   = 10;
  const std::size_t max_buckets = 135;
  auto base2_histogram_aggregation =
      std::make_unique<config_sdk::Base2ExponentialBucketHistogramAggregationConfiguration>();
  base2_histogram_aggregation->max_scale = max_scale;
  base2_histogram_aggregation->max_size  = max_buckets;

  auto base2_histogram_stream         = std::make_unique<config_sdk::ViewStreamConfiguration>();
  base2_histogram_stream->aggregation = std::move(base2_histogram_aggregation);

  auto base2_histogram_selector = std::make_unique<config_sdk::ViewSelectorConfiguration>();
  base2_histogram_selector->instrument_type = config_sdk::InstrumentType::histogram;
  base2_histogram_selector->instrument_name = "exponential-histogram";

  auto base2_histogram_view      = std::make_unique<config_sdk::ViewConfiguration>();
  base2_histogram_view->selector = std::move(base2_histogram_selector);
  base2_histogram_view->stream   = std::move(base2_histogram_stream);

  // View 2: Explicit bucket histogram aggregation.
  auto explicit_histogram_aggregation =
      std::make_unique<config_sdk::ExplicitBucketHistogramAggregationConfiguration>();
  explicit_histogram_aggregation->boundaries     = {0.0, 10.0, 50.0, 100.0};
  explicit_histogram_aggregation->record_min_max = true;

  auto explicit_histogram_stream         = std::make_unique<config_sdk::ViewStreamConfiguration>();
  explicit_histogram_stream->aggregation = std::move(explicit_histogram_aggregation);

  auto explicit_selector             = std::make_unique<config_sdk::ViewSelectorConfiguration>();
  explicit_selector->instrument_type = config_sdk::InstrumentType::histogram;
  explicit_selector->instrument_name = "explicit-histogram";

  auto explicit_histogram_view      = std::make_unique<config_sdk::ViewConfiguration>();
  explicit_histogram_view->selector = std::move(explicit_selector);
  explicit_histogram_view->stream   = std::move(explicit_histogram_stream);

  // View 3: no aggregation set
  auto default_stream         = std::make_unique<config_sdk::ViewStreamConfiguration>();
  default_stream->aggregation = nullptr;  // intentionally null

  auto default_selector             = std::make_unique<config_sdk::ViewSelectorConfiguration>();
  default_selector->instrument_type = config_sdk::InstrumentType::counter;
  default_selector->instrument_name = "default-counter";

  auto default_view      = std::make_unique<config_sdk::ViewConfiguration>();
  default_view->selector = std::move(default_selector);
  default_view->stream   = std::move(default_stream);

  auto meter_provider_config = MakeMeterProviderConfig();
  meter_provider_config->views.emplace_back(std::move(base2_histogram_view));
  meter_provider_config->views.emplace_back(std::move(explicit_histogram_view));
  meter_provider_config->views.emplace_back(std::move(default_view));

  auto model            = std::make_unique<config_sdk::Configuration>();
  model->meter_provider = std::move(meter_provider_config);

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->meter_provider, nullptr);

  auto context = context::Context{};
  auto meter   = metrics::Provider::GetMeterProvider()->GetMeter("test");
  meter->CreateDoubleHistogram("exponential-histogram")->Record(42.0, context);
  meter->CreateDoubleHistogram("explicit-histogram")->Record(42.0, context);
  meter->CreateUInt64Counter("default-counter")->Add(1, context);
  ASSERT_TRUE(sdk_->meter_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->meter_provider->Shutdown(std::chrono::milliseconds(5000)));

  // check that instances of the three data points were collected and are of the right type.
  EXPECT_GE(metric_buffer_->size(), 3);
  bool found_base2_histogram    = false;
  bool found_explicit_histogram = false;
  bool found_default_counter    = false;

  for (const auto &metric : *metric_buffer_)
  {
    auto &point_data = metric.point_data_attr_.front().point_data;
    if (nostd::holds_alternative<metrics_sdk::Base2ExponentialHistogramPointData>(point_data))
    {
      found_base2_histogram = true;
      auto &base2_point_data =
          nostd::get<metrics_sdk::Base2ExponentialHistogramPointData>(point_data);
      EXPECT_EQ(base2_point_data.max_buckets_, max_buckets);
      EXPECT_LE(base2_point_data.scale_, max_scale);
    }
    else if (nostd::holds_alternative<metrics_sdk::HistogramPointData>(point_data))
    {
      found_explicit_histogram = true;
    }
    else if (nostd::holds_alternative<metrics_sdk::SumPointData>(point_data))
    {
      found_default_counter = true;
    }
  }

  EXPECT_TRUE(found_base2_histogram);
  EXPECT_TRUE(found_explicit_histogram);
  EXPECT_TRUE(found_default_counter);
}

//---------------------------------------------------------------------------
// TracerProvider tests

TEST_F(ProgrammaticConfigTest, TracerProviderWithDefaults)
{
  auto model             = std::make_unique<config_sdk::Configuration>();
  model->tracer_provider = MakeTracerProviderConfig();

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->tracer_provider, nullptr);

  auto default_tracer = trace::Provider::GetTracerProvider()->GetTracer("default-tracer");
  default_tracer->StartSpan("test-span")->End();

  ASSERT_TRUE(sdk_->tracer_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->tracer_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_EQ(span_buffer_->size(), 1);
}

TEST_F(ProgrammaticConfigTest, TracerProviderWithTracerConfigurator)
{
  auto disabled_tracer_matcher           = config_sdk::TracerMatcherAndConfigConfiguration();
  disabled_tracer_matcher.name           = "disabled-tracer";
  disabled_tracer_matcher.config.enabled = false;

  auto tracer_configurator = std::make_unique<config_sdk::TracerConfiguratorConfiguration>();
  tracer_configurator->default_config.enabled = true;
  tracer_configurator->tracers.push_back(disabled_tracer_matcher);

  auto model                                  = std::make_unique<config_sdk::Configuration>();
  model->tracer_provider                      = MakeTracerProviderConfig();
  model->tracer_provider->tracer_configurator = std::move(tracer_configurator);

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->tracer_provider, nullptr);

  auto default_tracer = trace::Provider::GetTracerProvider()->GetTracer("default-tracer");
  default_tracer->StartSpan("test-span")->End();

  auto disabled_tracer = trace::Provider::GetTracerProvider()->GetTracer("disabled-tracer");
  disabled_tracer->StartSpan("disabled-test-span")->End();

  ASSERT_TRUE(sdk_->tracer_provider->ForceFlush(std::chrono::milliseconds(5000)));

  ASSERT_EQ(span_buffer_->size(), 1);
  EXPECT_NE(span_buffer_->at(0)->GetName(), "disabled-test-span");
}

TEST_F(ProgrammaticConfigTest, TracerProviderWithSampler)
{
  auto sampler                    = std::make_unique<config_sdk::AlwaysOffSamplerConfiguration>();
  auto model                      = std::make_unique<config_sdk::Configuration>();
  model->tracer_provider          = MakeTracerProviderConfig();
  model->tracer_provider->sampler = std::move(sampler);

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->tracer_provider, nullptr);

  trace::Provider::GetTracerProvider()->GetTracer("test")->StartSpan("test-span")->End();
  ASSERT_TRUE(sdk_->tracer_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->tracer_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_EQ(span_buffer_->size(), 0);
}

TEST_F(ProgrammaticConfigTest, TracerProviderWithParentBasedSamplerNullRoot)
{
  auto sampler                    = std::make_unique<config_sdk::ParentBasedSamplerConfiguration>();
  sampler->root                   = nullptr;  // explicitly null
  auto model                      = std::make_unique<config_sdk::Configuration>();
  model->tracer_provider          = MakeTracerProviderConfig();
  model->tracer_provider->sampler = std::move(sampler);

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->tracer_provider, nullptr);

  trace::Provider::GetTracerProvider()->GetTracer("test")->StartSpan("test-span")->End();
  ASSERT_TRUE(sdk_->tracer_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->tracer_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_EQ(span_buffer_->size(), 1);
}

// TODO: Re-enable this test once the BatchSpanProcessorConfiguration is initialized with spec
// defaults.
TEST_F(ProgrammaticConfigTest, TracerProviderWithBatchProcessor)
{
  auto exporter               = std::make_unique<config_sdk::ExtensionSpanExporterConfiguration>();
  exporter->name              = "recording";
  auto processor              = std::make_unique<config_sdk::BatchSpanProcessorConfiguration>();
  processor->exporter         = std::move(exporter);
  auto tracer_provider_config = std::make_unique<config_sdk::TracerProviderConfiguration>();
  tracer_provider_config->processors.emplace_back(std::move(processor));

  auto model             = std::make_unique<config_sdk::Configuration>();
  model->tracer_provider = std::move(tracer_provider_config);

  CreateAndInstallSdk(model);

  trace::Provider::GetTracerProvider()->GetTracer("test")->StartSpan("test-span")->End();
  ASSERT_TRUE(sdk_->tracer_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->tracer_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_GE(span_buffer_->size(), 1);
}

TEST_F(ProgrammaticConfigTest, TracerProviderWithBatchProcessorConfigured)
{
  auto exporter             = std::make_unique<config_sdk::ExtensionSpanExporterConfiguration>();
  exporter->name            = "recording";
  auto processor            = std::make_unique<config_sdk::BatchSpanProcessorConfiguration>();
  processor->schedule_delay = 60000;
  processor->max_queue_size = 100;
  processor->max_export_batch_size = 50;
  processor->export_timeout        = 5000;
  processor->exporter              = std::move(exporter);
  auto tracer_provider_config      = std::make_unique<config_sdk::TracerProviderConfiguration>();
  tracer_provider_config->processors.emplace_back(std::move(processor));

  auto model             = std::make_unique<config_sdk::Configuration>();
  model->tracer_provider = std::move(tracer_provider_config);

  CreateAndInstallSdk(model);

  trace::Provider::GetTracerProvider()->GetTracer("test")->StartSpan("test-span")->End();
  ASSERT_TRUE(sdk_->tracer_provider->ForceFlush(std::chrono::milliseconds(5000)));
  ASSERT_TRUE(sdk_->tracer_provider->Shutdown(std::chrono::milliseconds(5000)));

  EXPECT_GE(span_buffer_->size(), 1);
}

//---------------------------------------------------------------------------
// Propagator tests

namespace
{
void CheckPropagators()
{
  auto tracer = trace::Provider::GetTracerProvider()->GetTracer("test");

  auto span = tracer->StartSpan("test-span");
  auto ctx0 = context::RuntimeContext::GetCurrent();
  auto ctx1 = trace::SetSpan(ctx0, span);

  // Add baggage so the baggage propagator has something to inject
  auto baggage = baggage::Baggage::GetDefault()->Set("key", "value");
  auto ctx     = baggage::SetBaggage(ctx1, baggage);

  MapCarrier carrier;
  propagation::GlobalTextMapPropagator::GetGlobalPropagator()->Inject(carrier, ctx);

  ASSERT_NE(carrier.map().find("traceparent"), carrier.map().end());  // tracecontext
  // traceparent format: "00-<32 hex trace_id>-<16 hex span_id>-<2 hex flags>" = 55 chars
  const std::string &traceparent = carrier.map().at("traceparent");
  EXPECT_EQ(traceparent.size(), 55U);
  EXPECT_EQ(traceparent.substr(0, 3), "00-");
  EXPECT_NE(carrier.map().find("baggage"), carrier.map().end());        // baggage
  EXPECT_NE(carrier.map().find("b3"), carrier.map().end());             // b3 single
  EXPECT_NE(carrier.map().find("X-B3-TraceId"), carrier.map().end());   // b3multi
  EXPECT_NE(carrier.map().find("uber-trace-id"), carrier.map().end());  // jaeger
}
}  // namespace

TEST_F(ProgrammaticConfigTest, PropagatorsCompositeList)
{
  auto propagator_config            = std::make_unique<config_sdk::PropagatorConfiguration>();
  propagator_config->composite_list = "tracecontext,baggage,b3,b3multi,jaeger";

  auto model             = std::make_unique<config_sdk::Configuration>();
  model->tracer_provider = MakeTracerProviderConfig();
  model->propagator      = std::move(propagator_config);

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->propagator, nullptr);

  CheckPropagators();
}

TEST_F(ProgrammaticConfigTest, PropagatorsComposite)
{
  auto propagator_config = std::make_unique<config_sdk::PropagatorConfiguration>();
  propagator_config->composite.emplace_back("tracecontext");
  propagator_config->composite.emplace_back("baggage");
  propagator_config->composite.emplace_back("b3");
  propagator_config->composite.emplace_back("b3multi");
  propagator_config->composite.emplace_back("jaeger");

  auto model             = std::make_unique<config_sdk::Configuration>();
  model->tracer_provider = MakeTracerProviderConfig();
  model->propagator      = std::move(propagator_config);

  CreateAndInstallSdk(model);
  ASSERT_NE(sdk_->propagator, nullptr);

  CheckPropagators();
}
