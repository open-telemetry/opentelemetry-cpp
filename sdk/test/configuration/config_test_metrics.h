// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// Metrics-signal test helpers for SDK configuration tests.

#pragma once

#include <chrono>
#include <cstddef>
#include <memory>
#include <vector>

#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

namespace config_test
{

using MetricBuffer = std::vector<opentelemetry::sdk::metrics::MetricData>;

// ---------------------------------------------------------------------------
// No-op metrics helpers

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

class NoopConsolePushMetricExporterBuilder
    : public opentelemetry::sdk::configuration::ConsolePushMetricExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration *)
      const override
  {
    return std::make_unique<NoopPushMetricExporter>();
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

class NoopPullMetricExporterBuilder
    : public opentelemetry::sdk::configuration::ExtensionPullMetricExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const opentelemetry::sdk::configuration::ExtensionPullMetricExporterConfiguration *)
      const override
  {
    return std::make_unique<NoopMetricReader>();
  }
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
// Recording metrics helpers

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

class RecordingConsolePushMetricExporterBuilder
    : public opentelemetry::sdk::configuration::ConsolePushMetricExporterBuilder
{
public:
  explicit RecordingConsolePushMetricExporterBuilder(std::shared_ptr<MetricBuffer> buffer)
      : buffer_(std::move(buffer))
  {}
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const opentelemetry::sdk::configuration::ConsolePushMetricExporterConfiguration *)
      const override
  {
    return std::make_unique<RecordingPushMetricExporter>(buffer_);
  }

private:
  std::shared_ptr<MetricBuffer> buffer_;
};

class RecordingOtlpHttpPushMetricExporterBuilder
    : public opentelemetry::sdk::configuration::OtlpHttpPushMetricExporterBuilder
{
public:
  explicit RecordingOtlpHttpPushMetricExporterBuilder(std::shared_ptr<MetricBuffer> b)
      : buffer_(std::move(b))
  {}
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const opentelemetry::sdk::configuration::OtlpHttpPushMetricExporterConfiguration *)
      const override
  {
    return std::make_unique<RecordingPushMetricExporter>(buffer_);
  }

private:
  std::shared_ptr<MetricBuffer> buffer_;
};

class RecordingOtlpGrpcPushMetricExporterBuilder
    : public opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterBuilder
{
public:
  explicit RecordingOtlpGrpcPushMetricExporterBuilder(std::shared_ptr<MetricBuffer> b)
      : buffer_(std::move(b))
  {}
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterConfiguration *)
      const override
  {
    return std::make_unique<RecordingPushMetricExporter>(buffer_);
  }

private:
  std::shared_ptr<MetricBuffer> buffer_;
};

class RecordingOtlpFilePushMetricExporterBuilder
    : public opentelemetry::sdk::configuration::OtlpFilePushMetricExporterBuilder
{
public:
  explicit RecordingOtlpFilePushMetricExporterBuilder(std::shared_ptr<MetricBuffer> b)
      : buffer_(std::move(b))
  {}
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const opentelemetry::sdk::configuration::OtlpFilePushMetricExporterConfiguration *)
      const override
  {
    return std::make_unique<RecordingPushMetricExporter>(buffer_);
  }

private:
  std::shared_ptr<MetricBuffer> buffer_;
};

class SyncPullMetricReader : public opentelemetry::sdk::metrics::MetricReader
{
public:
  explicit SyncPullMetricReader(std::shared_ptr<MetricBuffer> buffer) : buffer_(std::move(buffer))
  {}
  opentelemetry::sdk::metrics::AggregationTemporality GetAggregationTemporality(
      opentelemetry::sdk::metrics::InstrumentType) const noexcept override
  {
    return opentelemetry::sdk::metrics::AggregationTemporality::kCumulative;
  }

private:
  bool OnForceFlush(std::chrono::microseconds) noexcept override
  {
    return Collect([this](opentelemetry::sdk::metrics::ResourceMetrics &data) {
      for (const auto &scope : data.scope_metric_data_)
        for (const auto &metric : scope.metric_data_)
          buffer_->emplace_back(metric);
      return true;
    });
  }
  bool OnShutDown(std::chrono::microseconds) noexcept override { return true; }

  std::shared_ptr<MetricBuffer> buffer_;
};

class RecordingPrometheusPullMetricExporterBuilder
    : public opentelemetry::sdk::configuration::PrometheusPullMetricExporterBuilder
{
public:
  explicit RecordingPrometheusPullMetricExporterBuilder(std::shared_ptr<MetricBuffer> buffer)
      : buffer_(std::move(buffer))
  {}
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *)
      const override
  {
    return std::make_unique<SyncPullMetricReader>(buffer_);
  }

private:
  std::shared_ptr<MetricBuffer> buffer_;
};

// ---------------------------------------------------------------------------
// Synchronous metric reader: collects and exports on ForceFlush in the calling thread.

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
    return Collect([this](opentelemetry::sdk::metrics::ResourceMetrics &metric_data) {
      return (exporter_->Export(metric_data) == opentelemetry::sdk::common::ExportResult::kSuccess);
    });
  }

  bool OnForceFlush(std::chrono::microseconds timeout) noexcept override
  {
    return CollectAndExport() && exporter_->ForceFlush(timeout);
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
// Capturing periodic metric reader builder: records args passed to Build().

struct CapturedPeriodicReaderArgs
{
  std::size_t interval{0};
  std::size_t timeout{0};
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> exporter;
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

}  // namespace config_test
