// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <string>

#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

class CustomPushMetricExporter : public opentelemetry::sdk::metrics::PushMetricExporter
{
public:
  CustomPushMetricExporter(const std::string &comment) : comment_(comment) {}
  CustomPushMetricExporter(CustomPushMetricExporter &&)                      = delete;
  CustomPushMetricExporter(const CustomPushMetricExporter &)                 = delete;
  CustomPushMetricExporter &operator=(CustomPushMetricExporter &&)           = delete;
  CustomPushMetricExporter &operator=(const CustomPushMetricExporter &other) = delete;
  ~CustomPushMetricExporter() override                                       = default;

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::sdk::metrics::ResourceMetrics &data) noexcept override;

  opentelemetry::sdk::metrics::AggregationTemporality GetAggregationTemporality(
      opentelemetry::sdk::metrics::InstrumentType instrument_type) const noexcept override;

  bool ForceFlush(std::chrono::microseconds timeout) noexcept override;

  bool Shutdown(std::chrono::microseconds timeout) noexcept override;

private:
  std::string comment_;
};
