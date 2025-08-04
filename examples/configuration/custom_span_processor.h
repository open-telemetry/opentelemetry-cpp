// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <memory>
#include <string>

#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/trace/span_context.h"

class CustomSpanProcessor : public opentelemetry::sdk::trace::SpanProcessor
{
public:
  CustomSpanProcessor(const std::string &comment) : comment_(comment) {}
  CustomSpanProcessor(CustomSpanProcessor &&)                      = delete;
  CustomSpanProcessor(const CustomSpanProcessor &)                 = delete;
  CustomSpanProcessor &operator=(CustomSpanProcessor &&)           = delete;
  CustomSpanProcessor &operator=(const CustomSpanProcessor &other) = delete;
  ~CustomSpanProcessor() override                                  = default;

  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override;

  void OnStart(opentelemetry::sdk::trace::Recordable &span,
               const opentelemetry::trace::SpanContext &parent_context) noexcept override;

  void OnEnd(std::unique_ptr<opentelemetry::sdk::trace::Recordable> &&span) noexcept override;

  bool ForceFlush(std::chrono::microseconds timeout) noexcept override;

  bool Shutdown(std::chrono::microseconds timeout) noexcept override;

private:
  std::string comment_;
};
