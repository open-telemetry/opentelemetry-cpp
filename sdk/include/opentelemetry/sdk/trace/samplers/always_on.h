// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/trace/sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

/**
 * The always on sampler is a default sampler which always return Decision::RECORD_AND_SAMPLE
 */
class AlwaysOnSampler : public Sampler
{
public:
  /**
   * @return Always return Decision RECORD_AND_SAMPLE
   */
  inline SamplingResult ShouldSample(
      const trace_api::SpanContext &parent_context,
      trace_api::TraceId /*trace_id*/,
      nostd::string_view /*name*/,
      trace_api::SpanKind /*span_kind*/,
      const opentelemetry::common::KeyValueIterable & /*attributes*/,
      const trace_api::SpanContextKeyValueIterable & /*links*/) noexcept override
  {
    if (!parent_context.IsValid())
    {
      return {Decision::RECORD_AND_SAMPLE, nullptr, opentelemetry::trace::TraceState::GetDefault()};
    }
    else
    {
      return {Decision::RECORD_AND_SAMPLE, nullptr, parent_context.trace_state()};
    }
  }

  /**
   * @return Description MUST be AlwaysOnSampler
   */
  inline nostd::string_view GetDescription() const noexcept override { return "AlwaysOnSampler"; }
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
