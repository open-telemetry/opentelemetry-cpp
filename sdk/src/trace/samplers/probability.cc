// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <atomic>
#include <cstdint>
#include <map>
#include <string>
#include <utility>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/probability.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/trace_state.h"
#include "opentelemetry/version.h"

#include "ot_trace_state.h"

namespace trace_api = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

ProbabilitySampler::ProbabilitySampler(double ratio)
{
  const double valid_ratio = ValidateRatio(ratio, "ProbabilitySampler");
  description_             = "ProbabilitySampler{" + std::to_string(valid_ratio) + "}";
  threshold_               = CalculateThreshold(valid_ratio);
}

SamplingResult ProbabilitySampler::ShouldSample(
    const trace_api::SpanContext &parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view /*name*/,
    trace_api::SpanKind /*span_kind*/,
    const opentelemetry::common::KeyValueIterable & /*attributes*/,
    const trace_api::SpanContextKeyValueIterable & /*links*/) noexcept
{
  const auto &parent_trace_state = parent_context.trace_state();

  nostd::string_view ot_value        = GetOtValue(parent_trace_state);
  OtelTraceState ot_state            = OtelTraceState::Parse(ot_value);
  const bool had_threshold           = ot_state.has_threshold;
  const uint64_t had_threshold_value = ot_state.threshold;

  // A threshold of 0 keeps every span regardless of randomness (e.g. a 100%
  // sampling ratio), so skip computing it entirely.
  bool is_sampled = threshold_ != kMaxThreshold;
  if (is_sampled && threshold_ != 0)
  {
    if (!ot_state.has_random_value && parent_context.IsValid() &&
        !parent_context.trace_flags().IsRandom())
    {
      static std::atomic<bool> warned{false};
      if (!warned.exchange(true))
      {
        OTEL_INTERNAL_LOG_WARN(
            "ProbabilitySampler presumes TraceID randomness, but the W3C random trace flag is "
            "not set. Upgrade the caller to W3C Trace Context Level 2.");
      }
    }
    is_sampled = GetSamplingRandomness(ot_state, trace_id) >= threshold_;
  }

  Decision decision = is_sampled ? Decision::RECORD_AND_SAMPLE : Decision::DROP;

  // Record the effective threshold when sampling; a dropped span carries no
  // probability, so its inherited (now stale) "th" must be erased. The "rv"
  // sub-key and any other "ot" sub-keys are preserved by OtelTraceState.
  if (is_sampled)
  {
    ot_state.has_threshold = true;
    ot_state.threshold     = threshold_;
  }
  else
  {
    ot_state.has_threshold = false;
  }

  nostd::shared_ptr<trace_api::TraceState> trace_state =
      parent_trace_state ? parent_trace_state : trace_api::TraceState::GetDefault();

  return {decision, nullptr,
          GetTraceStateForOtValue(ot_state, had_threshold, had_threshold_value, ot_value,
                                  std::move(trace_state))};
}

nostd::string_view ProbabilitySampler::GetDescription() const noexcept
{
  return description_;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
