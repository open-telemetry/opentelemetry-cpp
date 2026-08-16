// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// Logs-signal test helpers for SDK configuration tests.

#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor.h"

namespace config_test
{

using LogRecordBuffer = std::vector<std::unique_ptr<opentelemetry::sdk::logs::ReadWriteLogRecord>>;

// ---------------------------------------------------------------------------
// No-op logs helpers

class NoopLogRecordExporter : public opentelemetry::sdk::logs::LogRecordExporter
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::Recordable> MakeRecordable() noexcept override
  {
    return std::make_unique<opentelemetry::sdk::logs::ReadWriteLogRecord>();
  }
  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>>
          &) noexcept override
  {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }
  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }
};

class NoopConsoleLogRecordExporterBuilder
    : public opentelemetry::sdk::configuration::ConsoleLogRecordExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::ConsoleLogRecordExporterConfiguration *)
      const override
  {
    return std::make_unique<NoopLogRecordExporter>();
  }
};

class NoopLogRecordExporterBuilder
    : public opentelemetry::sdk::configuration::ExtensionLogRecordExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::ExtensionLogRecordExporterConfiguration *)
      const override
  {
    return std::make_unique<NoopLogRecordExporter>();
  }
};

// ---------------------------------------------------------------------------
// Recording logs helpers

class RecordingLogRecordExporter : public opentelemetry::sdk::logs::LogRecordExporter
{
public:
  explicit RecordingLogRecordExporter(std::shared_ptr<LogRecordBuffer> buffer)
      : buffer_(std::move(buffer))
  {}

  std::unique_ptr<opentelemetry::sdk::logs::Recordable> MakeRecordable() noexcept override
  {
    return std::make_unique<opentelemetry::sdk::logs::ReadWriteLogRecord>();
  }

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>>
          &records) noexcept override
  {
    if (!buffer_)
    {
      return opentelemetry::sdk::common::ExportResult::kFailure;
    }
    for (auto &rec : records)
    {
      buffer_->emplace_back(
          static_cast<opentelemetry::sdk::logs::ReadWriteLogRecord *>(rec.release()));
    }
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  bool RecordableEnforcesLogRecordLimits() const noexcept override { return true; }
  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }

private:
  std::shared_ptr<LogRecordBuffer> buffer_;
};

class RecordingLogRecordExporterBuilder
    : public opentelemetry::sdk::configuration::ExtensionLogRecordExporterBuilder
{
public:
  explicit RecordingLogRecordExporterBuilder(std::shared_ptr<LogRecordBuffer> buffer)
      : buffer_(std::move(buffer))
  {}
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::ExtensionLogRecordExporterConfiguration *)
      const override
  {
    return std::make_unique<RecordingLogRecordExporter>(buffer_);
  }

private:
  std::shared_ptr<LogRecordBuffer> buffer_;
};

class RecordingConsoleLogRecordExporterBuilder
    : public opentelemetry::sdk::configuration::ConsoleLogRecordExporterBuilder
{
public:
  explicit RecordingConsoleLogRecordExporterBuilder(std::shared_ptr<LogRecordBuffer> buffer)
      : buffer_(std::move(buffer))
  {}
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::ConsoleLogRecordExporterConfiguration *)
      const override
  {
    return std::make_unique<RecordingLogRecordExporter>(buffer_);
  }

private:
  std::shared_ptr<LogRecordBuffer> buffer_;
};

class RecordingOtlpHttpLogRecordExporterBuilder
    : public opentelemetry::sdk::configuration::OtlpHttpLogRecordExporterBuilder
{
public:
  explicit RecordingOtlpHttpLogRecordExporterBuilder(std::shared_ptr<LogRecordBuffer> b)
      : buffer_(std::move(b))
  {}
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::OtlpHttpLogRecordExporterConfiguration *)
      const override
  {
    return std::make_unique<RecordingLogRecordExporter>(buffer_);
  }

private:
  std::shared_ptr<LogRecordBuffer> buffer_;
};

class RecordingOtlpGrpcLogRecordExporterBuilder
    : public opentelemetry::sdk::configuration::OtlpGrpcLogRecordExporterBuilder
{
public:
  explicit RecordingOtlpGrpcLogRecordExporterBuilder(std::shared_ptr<LogRecordBuffer> b)
      : buffer_(std::move(b))
  {}
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::OtlpGrpcLogRecordExporterConfiguration *)
      const override
  {
    return std::make_unique<RecordingLogRecordExporter>(buffer_);
  }

private:
  std::shared_ptr<LogRecordBuffer> buffer_;
};

class RecordingOtlpFileLogRecordExporterBuilder
    : public opentelemetry::sdk::configuration::OtlpFileLogRecordExporterBuilder
{
public:
  explicit RecordingOtlpFileLogRecordExporterBuilder(std::shared_ptr<LogRecordBuffer> b)
      : buffer_(std::move(b))
  {}
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::OtlpFileLogRecordExporterConfiguration *)
      const override
  {
    return std::make_unique<RecordingLogRecordExporter>(buffer_);
  }

private:
  std::shared_ptr<LogRecordBuffer> buffer_;
};

class RecordingLogRecordProcessorBuilder
    : public opentelemetry::sdk::configuration::ExtensionLogRecordProcessorBuilder
{
public:
  mutable bool called{false};
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> Build(
      const opentelemetry::sdk::configuration::ExtensionLogRecordProcessorConfiguration *)
      const override
  {
    called = true;
    return std::make_unique<opentelemetry::sdk::logs::SimpleLogRecordProcessor>(
        std::make_unique<NoopLogRecordExporter>());
  }
};

// ---------------------------------------------------------------------------
// Mock batch processor builder: uses SimpleLogRecordProcessor for synchronous tests.

class MockBatchLogRecordProcessorBuilder
    : public opentelemetry::sdk::configuration::BatchLogRecordProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> Build(
      const opentelemetry::sdk::configuration::BatchLogRecordProcessorConfiguration *,
      std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> &&exporter) const override
  {
    return std::make_unique<opentelemetry::sdk::logs::SimpleLogRecordProcessor>(
        std::move(exporter));
  }
};

}  // namespace config_test
