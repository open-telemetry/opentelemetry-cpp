// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <map>
#include <memory>

#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/trace_state.h"

#include "custom_sampler.h"

opentelemetry::sdk::trace::SamplingResult CustomSampler::ShouldSample(
    const opentelemetry::trace::SpanContext &parent_context,
    opentelemetry::trace::TraceId /* trace_id */,
    opentelemetry::nostd::string_view /* name */,
    opentelemetry::trace::SpanKind /* span_kind */,
    const opentelemetry::common::KeyValueIterable & /* attributes */,
    const opentelemetry::trace::SpanContextKeyValueIterable & /* links */) noexcept
{
  if (!parent_context.IsValid())
  {
    return {opentelemetry::sdk::trace::Decision::RECORD_AND_SAMPLE, nullptr,
            opentelemetry::trace::TraceState::GetDefault()};
  }
  else
  {
    return {opentelemetry::sdk::trace::Decision::RECORD_AND_SAMPLE, nullptr,
            parent_context.trace_state()};
  }
}

opentelemetry::nostd::string_view CustomSampler::GetDescription() const noexcept
{
  return "CustomSampler";
}
