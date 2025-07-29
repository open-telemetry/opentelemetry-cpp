// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <utility>

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/trace/span_context.h"

#include "custom_span_processor.h"

std::unique_ptr<opentelemetry::sdk::trace::Recordable>
CustomSpanProcessor::MakeRecordable() noexcept
{
  auto recordable = std::make_unique<opentelemetry::sdk::trace::SpanData>();
  return recordable;
}

void CustomSpanProcessor::OnStart(
    opentelemetry::sdk::trace::Recordable & /* span */,
    const opentelemetry::trace::SpanContext & /* parent_context */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomSpanProcessor::OnStart(): YOUR CODE HERE");
}

void CustomSpanProcessor::OnEnd(
    std::unique_ptr<opentelemetry::sdk::trace::Recordable> &&span) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomSpanProcessor::OnEnd(): YOUR CODE HERE");
  auto unused = std::move(span);
}

bool CustomSpanProcessor::ForceFlush(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomSpanProcessor::ForceFlush(): YOUR CODE HERE");
  return false;
}

bool CustomSpanProcessor::Shutdown(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomSpanProcessor::Shutdown(): YOUR CODE HERE");
  return false;
}
