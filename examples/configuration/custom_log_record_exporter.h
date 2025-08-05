// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <memory>
#include <string>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/recordable.h"

class CustomLogRecordExporter : public opentelemetry::sdk::logs::LogRecordExporter
{
public:
  CustomLogRecordExporter(const std::string &comment) : comment_(comment) {}
  CustomLogRecordExporter(CustomLogRecordExporter &&)                      = delete;
  CustomLogRecordExporter(const CustomLogRecordExporter &)                 = delete;
  CustomLogRecordExporter &operator=(CustomLogRecordExporter &&)           = delete;
  CustomLogRecordExporter &operator=(const CustomLogRecordExporter &other) = delete;
  ~CustomLogRecordExporter() override                                      = default;

  std::unique_ptr<opentelemetry::sdk::logs::Recordable> MakeRecordable() noexcept override;

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>>
          &records) noexcept override;

  bool ForceFlush(std::chrono::microseconds timeout) noexcept override;

  bool Shutdown(std::chrono::microseconds timeout) noexcept override;

private:
  std::string comment_;
};
