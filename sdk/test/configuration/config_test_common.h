// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// Shared test helpers for the SDK configuration tests
//
// Provides:
//  Noop Exporters and Builders:
//   - NoopSpanExporter / NoopSpanExporterBuilder
//   - NoopLogRecordExporter / NoopLogRecordExporterBuilder
//   - NoopPushMetricExporter / NoopPeriodicMetricReaderBuilder
//
//  Recording Exporters and Builders: (data collection with no threading)
//   - RecordingLogRecordExporter / RecordingLogRecordExporterBuilder
//   - RecordingSpanExporter / RecordingSpanExporterBuilder
//   - SyncMetricReader / SyncPeriodicMetricReaderBuilder
//   - CapturedPeriodicReaderArgs / CapturingPeriodicMetricReaderBuilder
//
//  Propagator:
//   - MapCarrier

#pragma once

#include <chrono>
#include <cstddef>
#include <memory>
#include <vector>

#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"

namespace config_test
{

// ---------------------------------------------------------------------------
// Export buffer type aliases

using SpanBuffer      = std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanData>>;
using LogRecordBuffer = std::vector<std::unique_ptr<opentelemetry::sdk::logs::ReadWriteLogRecord>>;
using MetricBuffer    = std::vector<opentelemetry::sdk::metrics::MetricData>;

// ---------------------------------------------------------------------------
// No-op exporters

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

class NoopPushMetricExporter : public opentelemetry::sdk::metrics::PushMetricExporter
{
public:
  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::sdk::metrics::ResourceMetrics &) noexcept override
  {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }
  opentelemetry::sdk::metrics::AggregationTemporality GetAggregationTemporality(
      opentelemetry::sdk::metrics::InstrumentType) const noexcept override
  {
    return opentelemetry::sdk::metrics::AggregationTemporality::kCumulative;
  }
  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }
};

// ---------------------------------------------------------------------------
// No-op extension builders

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

class NoopPushMetricExporterBuilder
    : public opentelemetry::sdk::configuration::ExtensionPushMetricExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const opentelemetry::sdk::configuration::ExtensionPushMetricExporterConfiguration *)
      const override
  {
    return std::make_unique<NoopPushMetricExporter>();
  }
};

// ---------------------------------------------------------------------------
// No-op metric reader

class NoopMetricReader : public opentelemetry::sdk::metrics::MetricReader
{
public:
  opentelemetry::sdk::metrics::AggregationTemporality GetAggregationTemporality(
      opentelemetry::sdk::metrics::InstrumentType) const noexcept override
  {
    return opentelemetry::sdk::metrics::AggregationTemporality::kCumulative;
  }

private:
  bool OnForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool OnShutDown(std::chrono::microseconds) noexcept override { return true; }
};

class NoopPeriodicMetricReaderBuilder
    : public opentelemetry::sdk::configuration::PeriodicMetricReaderBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *,
      std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> &&exporter) const override
  {
    auto unused = std::move(exporter);
    return std::make_unique<NoopMetricReader>();
  }
};

// ---------------------------------------------------------------------------
// Recording exporters: capture exported data into shared buffers for
// inspection in integration tests.

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

class RecordingPushMetricExporter : public opentelemetry::sdk::metrics::PushMetricExporter
{
public:
  explicit RecordingPushMetricExporter(std::shared_ptr<MetricBuffer> buffer)
      : buffer_(std::move(buffer))
  {}

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::sdk::metrics::ResourceMetrics &resource_metrics) noexcept override
  {
    for (const auto &scope : resource_metrics.scope_metric_data_)
    {
      for (const auto &metric : scope.metric_data_)
      {
        buffer_->emplace_back(metric);
      }
    }
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  opentelemetry::sdk::metrics::AggregationTemporality GetAggregationTemporality(
      opentelemetry::sdk::metrics::InstrumentType) const noexcept override
  {
    return opentelemetry::sdk::metrics::AggregationTemporality::kCumulative;
  }

  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }

private:
  std::shared_ptr<MetricBuffer> buffer_;
};

// ---------------------------------------------------------------------------
// Recording extension builders

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

class RecordingPushMetricExporterBuilder
    : public opentelemetry::sdk::configuration::ExtensionPushMetricExporterBuilder
{
public:
  explicit RecordingPushMetricExporterBuilder(std::shared_ptr<MetricBuffer> buffer)
      : buffer_(std::move(buffer))
  {}
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const opentelemetry::sdk::configuration::ExtensionPushMetricExporterConfiguration *)
      const override
  {
    return std::make_unique<RecordingPushMetricExporter>(buffer_);
  }

private:
  std::shared_ptr<MetricBuffer> buffer_;
};

// ---------------------------------------------------------------------------
// Synchronous metric reader: collects and exports on ForceFlush in
// the calling thread.
class SyncMetricReader : public opentelemetry::sdk::metrics::MetricReader
{
public:
  explicit SyncMetricReader(
      std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> exporter)
      : exporter_(std::move(exporter))
  {}

  opentelemetry::sdk::metrics::AggregationTemporality GetAggregationTemporality(
      opentelemetry::sdk::metrics::InstrumentType instrument_type) const noexcept override
  {
    return exporter_->GetAggregationTemporality(instrument_type);
  }

private:
  bool CollectAndExport() noexcept
  {
    const bool success = Collect([this](opentelemetry::sdk::metrics::ResourceMetrics &metric_data) {
      return (exporter_->Export(metric_data) == opentelemetry::sdk::common::ExportResult::kSuccess);
    });
    return success;
  }

  bool OnForceFlush(std::chrono::microseconds timeout) noexcept override
  {
    const bool collect_result = CollectAndExport();
    const bool flush_result   = exporter_->ForceFlush(timeout);
    return collect_result && flush_result;
  }

  bool OnShutDown(std::chrono::microseconds timeout) noexcept override
  {
    return exporter_->Shutdown(timeout);
  }

  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> exporter_;
};

class SyncPeriodicMetricReaderBuilder
    : public opentelemetry::sdk::configuration::PeriodicMetricReaderBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *,
      std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> &&exporter) const override
  {
    return std::make_unique<SyncMetricReader>(std::move(exporter));
  }
};

// ---------------------------------------------------------------------------
// Capturing periodic metric reader builder. Records the configuration
// arguments passed to Build()

struct CapturedPeriodicReaderArgs
{
  std::size_t interval{0};
  std::size_t timeout{0};
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> exporter;
  // TODO: add cardinality limits and producers when we support them in the builder
  bool called{false};
};

class CapturingPeriodicMetricReaderBuilder
    : public opentelemetry::sdk::configuration::PeriodicMetricReaderBuilder
{
public:
  explicit CapturingPeriodicMetricReaderBuilder(
      std::shared_ptr<CapturedPeriodicReaderArgs> captured)
      : captured_(std::move(captured))
  {}

  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const opentelemetry::sdk::configuration::PeriodicMetricReaderConfiguration *model,
      std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> &&exporter) const override
  {
    captured_->called   = true;
    captured_->interval = model->interval;
    captured_->timeout  = model->timeout;
    captured_->exporter = std::move(exporter);
    return std::make_unique<NoopMetricReader>();
  }

private:
  std::shared_ptr<CapturedPeriodicReaderArgs> captured_;
};

// ---------------------------------------------------------------------------
// TextMapCarrier for propagator tests.

class MapCarrier : public opentelemetry::context::propagation::TextMapCarrier
{
public:
  opentelemetry::nostd::string_view Get(
      opentelemetry::nostd::string_view key) const noexcept override
  {
    auto it = map_.find(std::string(key));
    return it != map_.end() ? opentelemetry::nostd::string_view(it->second) : "";
  }
  void Set(opentelemetry::nostd::string_view key,
           opentelemetry::nostd::string_view value) noexcept override
  {
    map_[std::string(key)] = std::string(value);
  }

  const std::map<std::string, std::string> &map() const { return map_; }

private:
  std::map<std::string, std::string> map_;
};

}  // namespace config_test
