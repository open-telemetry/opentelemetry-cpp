/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "opentelemetry/baggage/baggage.h"
#include "opentelemetry/trace/span_context.h"
#include "opentracing/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace opentracingshim
{

using BaggagePtr = nostd::shared_ptr<opentelemetry::baggage::Baggage>;

class SpanContextShim final : public opentracing::SpanContext
{
public:
  explicit SpanContextShim(const opentelemetry::trace::SpanContext& context, const BaggagePtr& baggage)
    : context_(context), baggage_(baggage) {}
  inline const opentelemetry::trace::SpanContext& context() const { return context_; }
  inline const BaggagePtr baggage() const { return baggage_; }
  SpanContextShim newWithKeyValue(nostd::string_view key, nostd::string_view value) const noexcept;
  bool BaggageItem(nostd::string_view key, std::string& value) const noexcept;
  // Overrides
  using VisitBaggageItem = std::function<bool(const std::string& key, const std::string& value)>;
  void ForeachBaggageItem(VisitBaggageItem f) const override;
  std::unique_ptr<opentracing::SpanContext> Clone() const noexcept override;

private:
  opentelemetry::trace::SpanContext context_;
  BaggagePtr baggage_;
};

} // namespace opentracingshim
OPENTELEMETRY_END_NAMESPACE
