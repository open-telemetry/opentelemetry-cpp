// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <memory>
#include <string>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/recordable.h"

class CustomSpanExporter : public opentelemetry::sdk::trace::SpanExporter
{
public:
  CustomSpanExporter(const std::string &comment) : comment_(comment) {}
  CustomSpanExporter(CustomSpanExporter &&)                      = delete;
  CustomSpanExporter(const CustomSpanExporter &)                 = delete;
  CustomSpanExporter &operator=(CustomSpanExporter &&)           = delete;
  CustomSpanExporter &operator=(const CustomSpanExporter &other) = delete;
  ~CustomSpanExporter() override                                 = default;

  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override;

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>>
          &spans) noexcept override;

  bool ForceFlush(std::chrono::microseconds timeout) noexcept override;

  bool Shutdown(std::chrono::microseconds timeout) noexcept override;

private:
  std::string comment_;
};
