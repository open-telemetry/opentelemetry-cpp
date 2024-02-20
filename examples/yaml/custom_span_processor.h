// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/init/extension_span_processor_builder.h"
#include "opentelemetry/sdk/trace/processor.h"

class CustomSpanProcessor : public opentelemetry::sdk::trace::SpanProcessor
{
public:
  CustomSpanProcessor(std::string comment) : m_comment(comment) {}
  ~CustomSpanProcessor() override = default;

  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override;

  void OnStart(opentelemetry::sdk::trace::Recordable &span,
               const opentelemetry::trace::SpanContext &parent_context) noexcept override;

  void OnEnd(std::unique_ptr<opentelemetry::sdk::trace::Recordable> &&span) noexcept override;

  bool ForceFlush(std::chrono::microseconds timeout) noexcept override;

  bool Shutdown(std::chrono::microseconds timeout) noexcept override;

private:
  std::string m_comment;
};
