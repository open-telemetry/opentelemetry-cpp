// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/trace/span_context.h"

#include "custom_span_exporter.h"

std::unique_ptr<opentelemetry::sdk::trace::Recordable> CustomSpanExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<opentelemetry::sdk::trace::Recordable>(
      new opentelemetry::sdk::trace::SpanData);
}

opentelemetry::sdk::common::ExportResult CustomSpanExporter::Export(
    const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>>
        &spans) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomSpanExporter::Export(): FIXME");
  return opentelemetry::sdk::common::ExportResult::kSuccess;
}

bool CustomSpanExporter::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomSpanExporter::ForceFlush(): FIXME");
  return false;
}

bool CustomSpanExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomSpanExporter::Shutdown(): FIXME");
  return false;
}
