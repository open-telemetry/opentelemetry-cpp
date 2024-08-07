// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/trace/span_context.h"

#include "custom_span_processor.h"

std::unique_ptr<opentelemetry::sdk::trace::Recordable>
CustomSpanProcessor::MakeRecordable() noexcept
{
  return std::unique_ptr<opentelemetry::sdk::trace::Recordable>(
      new opentelemetry::sdk::trace::SpanData);
}

void CustomSpanProcessor::OnStart(
    opentelemetry::sdk::trace::Recordable & /* span */,
    const opentelemetry::trace::SpanContext & /* parent_context */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomSpanProcessor::OnStart(): FIXME");
}

void CustomSpanProcessor::OnEnd(
    std::unique_ptr<opentelemetry::sdk::trace::Recordable> && /* span */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomSpanProcessor::OnEnd(): FIXME");
}

bool CustomSpanProcessor::ForceFlush(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomSpanProcessor::ForceFlush(): FIXME");
  return false;
}

bool CustomSpanProcessor::Shutdown(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomSpanProcessor::Shutdown(): FIXME");
  return false;
}
