// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// Trace-signal test helpers for SDK configuration tests.

#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_configuration.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"

namespace config_test
{

using SpanBuffer = std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanData>>;

// ---------------------------------------------------------------------------
// No-op trace helpers

class NoopSpanExporter : public opentelemetry::sdk::trace::SpanExporter
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override
  {
    return std::make_unique<opentelemetry::sdk::trace::SpanData>();
  }
  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>>
          &) noexcept override
  {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }
  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }
};

class NoopConsoleSpanExporterBuilder
    : public opentelemetry::sdk::configuration::ConsoleSpanExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration *) const override
  {
    return std::make_unique<NoopSpanExporter>();
  }
};

class NoopSpanExporterBuilder
    : public opentelemetry::sdk::configuration::ExtensionSpanExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *) const override
  {
    return std::make_unique<NoopSpanExporter>();
  }
};

// ---------------------------------------------------------------------------
// Recording trace helpers

class RecordingSpanExporter : public opentelemetry::sdk::trace::SpanExporter
{
public:
  explicit RecordingSpanExporter(std::shared_ptr<SpanBuffer> buffer) : buffer_(std::move(buffer)) {}

  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override
  {
    return std::make_unique<opentelemetry::sdk::trace::SpanData>();
  }

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>>
          &spans) noexcept override
  {
    if (!buffer_)
    {
      return opentelemetry::sdk::common::ExportResult::kFailure;
    }
    for (auto &span : spans)
    {
      buffer_->emplace_back(static_cast<opentelemetry::sdk::trace::SpanData *>(span.release()));
    }
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }

private:
  std::shared_ptr<SpanBuffer> buffer_;
};

class RecordingSpanExporterBuilder
    : public opentelemetry::sdk::configuration::ExtensionSpanExporterBuilder
{
public:
  explicit RecordingSpanExporterBuilder(std::shared_ptr<SpanBuffer> buffer)
      : buffer_(std::move(buffer))
  {}
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::ExtensionSpanExporterConfiguration *) const override
  {
    return std::make_unique<RecordingSpanExporter>(buffer_);
  }

private:
  std::shared_ptr<SpanBuffer> buffer_;
};

class RecordingConsoleSpanExporterBuilder
    : public opentelemetry::sdk::configuration::ConsoleSpanExporterBuilder
{
public:
  explicit RecordingConsoleSpanExporterBuilder(std::shared_ptr<SpanBuffer> buffer)
      : buffer_(std::move(buffer))
  {}
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::ConsoleSpanExporterConfiguration *) const override
  {
    return std::make_unique<RecordingSpanExporter>(buffer_);
  }

private:
  std::shared_ptr<SpanBuffer> buffer_;
};

class RecordingOtlpHttpSpanExporterBuilder
    : public opentelemetry::sdk::configuration::OtlpHttpSpanExporterBuilder
{
public:
  explicit RecordingOtlpHttpSpanExporterBuilder(std::shared_ptr<SpanBuffer> b)
      : buffer_(std::move(b))
  {}
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::OtlpHttpSpanExporterConfiguration *) const override
  {
    return std::make_unique<RecordingSpanExporter>(buffer_);
  }

private:
  std::shared_ptr<SpanBuffer> buffer_;
};

class RecordingOtlpGrpcSpanExporterBuilder
    : public opentelemetry::sdk::configuration::OtlpGrpcSpanExporterBuilder
{
public:
  explicit RecordingOtlpGrpcSpanExporterBuilder(std::shared_ptr<SpanBuffer> b)
      : buffer_(std::move(b))
  {}
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::OtlpGrpcSpanExporterConfiguration *) const override
  {
    return std::make_unique<RecordingSpanExporter>(buffer_);
  }

private:
  std::shared_ptr<SpanBuffer> buffer_;
};

class RecordingOtlpFileSpanExporterBuilder
    : public opentelemetry::sdk::configuration::OtlpFileSpanExporterBuilder
{
public:
  explicit RecordingOtlpFileSpanExporterBuilder(std::shared_ptr<SpanBuffer> b)
      : buffer_(std::move(b))
  {}
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const opentelemetry::sdk::configuration::OtlpFileSpanExporterConfiguration *) const override
  {
    return std::make_unique<RecordingSpanExporter>(buffer_);
  }

private:
  std::shared_ptr<SpanBuffer> buffer_;
};

// ---------------------------------------------------------------------------
// Mock batch processor builder: uses SimpleSpanProcessor for synchronous tests.

class MockBatchSpanProcessorBuilder
    : public opentelemetry::sdk::configuration::BatchSpanProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> Build(
      const opentelemetry::sdk::configuration::BatchSpanProcessorConfiguration *,
      std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> &&exporter) const override
  {
    return std::make_unique<opentelemetry::sdk::trace::SimpleSpanProcessor>(std::move(exporter));
  }
};

}  // namespace config_test
