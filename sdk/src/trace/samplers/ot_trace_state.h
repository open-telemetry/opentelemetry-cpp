// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/trace_state.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

// Key of the OpenTelemetry sub-state inside the W3C tracestate.
static constexpr const char *kOtTraceStateKey = "ot";

// Number of bits used for randomness and thresholds in consistent probability
// sampling (W3C Trace Context Level 2 / OpenTelemetry tracestate "ot").
static constexpr int kRandomValueBits = 56;

// threshold == kMaxThreshold corresponds to 0% sampling probability.
// threshold == 0 corresponds to 100% sampling probability. A span is kept when
// its randomness R satisfies R >= threshold.
static constexpr uint64_t kMaxThreshold   = static_cast<uint64_t>(1) << kRandomValueBits;
static constexpr uint64_t kMaxRandomValue = kMaxThreshold - 1;

// Maps a sampling probability in [0, 1] to a 56-bit rejection threshold in the
// range [0, kMaxThreshold].
uint64_t CalculateThreshold(double sampling_probability) noexcept;

// Returns ratio unchanged when it is 0 or within [2^-56, 1.0]. Anything else
// (including NaN) logs a warning under sampler_name and returns the
// configuration default 1.0.
double ValidateRatio(double ratio, const char *sampler_name) noexcept;

// Extracts the 56-bit randomness value from the least significant 56 bits of
// the trace id, as specified by W3C Trace Context Level 2.
uint64_t GetRandomnessFromTraceId(const opentelemetry::trace::TraceId &trace_id) noexcept;

// Returns the "ot" tracestate value as a view into trace_state's own storage. Empty if trace_state
// is null or has no "ot" entry.
nostd::string_view GetOtValue(
    const nostd::shared_ptr<opentelemetry::trace::TraceState> &trace_state) noexcept;

// Parsed view of the OpenTelemetry "ot" tracestate value. Holds the "th"
// (threshold) and "rv" (randomness) sub-keys; any other sub-keys are preserved
// verbatim and re-emitted on serialization.
struct OtelTraceState
{
  bool has_threshold = false;
  uint64_t threshold = 0;  // valid in [0, kMaxThreshold]

  bool has_random_value = false;
  uint64_t random_value = 0;  // valid in [0, kMaxRandomValue]

  // Views into the ot_value passed to Parse(): valid only as long as that
  // tracestate buffer is alive.
  std::vector<nostd::string_view> other_subkeys;

  // Parses the value of the "ot" tracestate key. On any parse error the
  // offending sub-key is dropped; the parser never throws.
  //
  // other_subkeys in the result view into ot_value, so ot_value's backing
  // storage must outlive the returned OtelTraceState.
  static OtelTraceState Parse(
      nostd::string_view ot_value OPENTELEMETRY_ATTRIBUTE_LIFETIME_BOUND) noexcept;

  // Serializes back to the "ot" tracestate value. Returns an empty string when
  // there is nothing to emit.
  std::string Serialize() const;
};

// The randomness value to use for a sampling decision: the explicit "rv" from
// an upstream Level 2 participant when present (keeping the decision
// consistent across the trace), otherwise derived from the trace id.
uint64_t GetSamplingRandomness(const OtelTraceState &ot_state,
                               const opentelemetry::trace::TraceId &trace_id) noexcept;

// Applies a sampling decision's threshold state to trace_state's "ot" entry.
// had_threshold/had_threshold_value are what OtelTraceState::Parse(ot_value)
// produced before the decision mutated ot_state. If "th" ends up numerically
// unchanged, trace_state is returned as-is (no allocation, no Set()/Delete()).
// Otherwise "ot" is rewritten, or removed if nothing of it remains.
nostd::shared_ptr<opentelemetry::trace::TraceState> GetTraceStateForOtValue(
    const OtelTraceState &ot_state,
    bool had_threshold,
    uint64_t had_threshold_value,
    nostd::string_view ot_value,
    nostd::shared_ptr<opentelemetry::trace::TraceState> trace_state);

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
