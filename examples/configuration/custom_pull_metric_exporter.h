// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <string>

#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"

class CustomPullMetricExporter : public opentelemetry::sdk::metrics::MetricReader
{
public:
  CustomPullMetricExporter(const std::string &comment) : comment_(comment) {}
  CustomPullMetricExporter(CustomPullMetricExporter &&)                      = delete;
  CustomPullMetricExporter(const CustomPullMetricExporter &)                 = delete;
  CustomPullMetricExporter &operator=(CustomPullMetricExporter &&)           = delete;
  CustomPullMetricExporter &operator=(const CustomPullMetricExporter &other) = delete;
  ~CustomPullMetricExporter() override                                       = default;

  opentelemetry::sdk::metrics::AggregationTemporality GetAggregationTemporality(
      opentelemetry::sdk::metrics::InstrumentType instrument_type) const noexcept override;

  bool OnForceFlush(std::chrono::microseconds timeout) noexcept override;

  bool OnShutDown(std::chrono::microseconds timeout) noexcept override;

  void OnInitialized() noexcept override;

private:
  std::string comment_;
};
