// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/init/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/trace/exporter.h"

class CustomSpanExporter : public opentelemetry::sdk::trace::SpanExporter
{
public:
  CustomSpanExporter(const std::string & /* comment */) {}
  ~CustomSpanExporter() override = default;

  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override;

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>>
          &spans) noexcept override;

  bool ForceFlush(std::chrono::microseconds timeout) noexcept override;

  bool Shutdown(std::chrono::microseconds timeout) noexcept override;
};
