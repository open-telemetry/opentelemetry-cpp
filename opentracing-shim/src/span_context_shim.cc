/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "opentelemetry/opentracingshim/span_context_shim.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace opentracingshim
{

SpanContextShim SpanContextShim::newWithKeyValue(nostd::string_view key,
                                                 nostd::string_view value) const noexcept
{
  return SpanContextShim{context_, baggage_->Set(key, value)};
}

bool SpanContextShim::BaggageItem(nostd::string_view key, std::string &value) const noexcept
{
  return baggage_->GetValue(key, value);
}

void SpanContextShim::ForeachBaggageItem(VisitBaggageItem f) const
{
  baggage_->GetAllEntries([&f](nostd::string_view key, nostd::string_view value) {
    return f(key.data(), value.data());
  });
}

std::unique_ptr<opentracing::SpanContext> SpanContextShim::Clone() const noexcept
{
  return std::unique_ptr<SpanContextShim>(new (std::nothrow) SpanContextShim(context_, baggage_));
}

std::string SpanContextShim::ToTraceID() const noexcept
{
  return toHexString(context_.trace_id());
}

std::string SpanContextShim::ToSpanID() const noexcept
{
  return toHexString(context_.span_id());
}

}  // namespace opentracingshim
OPENTELEMETRY_END_NAMESPACE
