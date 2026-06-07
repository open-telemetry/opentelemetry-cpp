// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/probability.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/trace_state.h"
#include "opentelemetry/version.h"

namespace trace_api = opentelemetry::trace;
namespace nostd     = opentelemetry::nostd;

namespace
{
constexpr char kOtTraceStateKey[] = "ot";

// Rejection threshold for a ratio of zero, 2^56: drops every span.
constexpr uint64_t kMaxThreshold = 1ULL << 56;

/**
 * Converts a ratio in [0, 1] to a 56-bit rejection threshold.
 */
uint64_t CalculateRejectionThreshold(double ratio) noexcept
{
  // The negated comparison also routes NaN to the never-sample threshold.
  if (!(ratio > 0.0))
    return kMaxThreshold;
  if (ratio >= 1.0)
    return 0;
  return kMaxThreshold -
         static_cast<uint64_t>(std::llround(ratio * static_cast<double>(kMaxThreshold)));
}

bool ParseHex56(nostd::string_view hex, uint64_t *value) noexcept
{
  if (hex.size() != 14)
    return false;
  uint64_t result = 0;
  for (char c : hex)
  {
    result <<= 4;
    if (c >= '0' && c <= '9')
      result |= static_cast<uint64_t>(c - '0');
    else if (c >= 'a' && c <= 'f')
      result |= static_cast<uint64_t>(c - 'a' + 10);
    else
      return false;
  }
  *value = result;
  return true;
}

/**
 * Encodes a 56-bit threshold as lowercase hex with trailing zeros removed,
 * e.g. 2^55 becomes "8" and 0 becomes "0".
 */
std::string EncodeThreshold(uint64_t threshold)
{
  if (threshold == 0)
    return "0";
  static const char kHex[] = "0123456789abcdef";
  std::string hex(14, '0');
  for (int i = 13; i >= 0; --i)
  {
    hex[i] = kHex[threshold & 0xf];
    threshold >>= 4;
  }
  hex.erase(hex.find_last_not_of('0') + 1);
  return hex;
}

uint64_t RandomnessFromTraceId(const trace_api::TraceId &trace_id) noexcept
{
  static_assert(trace_api::TraceId::kSize >= 7, "TraceID must be at least 7 bytes long.");

  const uint8_t *data = trace_id.Id().data();
  uint64_t result     = 0;
  for (size_t i = trace_api::TraceId::kSize - 7; i < trace_api::TraceId::kSize; ++i)
  {
    result = (result << 8) | data[i];
  }
  return result;
}

// Returns true when the ot value carries a valid rv sub-key.
bool ExplicitRandomness(nostd::string_view ot_value, uint64_t *randomness) noexcept
{
  while (!ot_value.empty())
  {
    size_t end                 = ot_value.find(';');
    nostd::string_view sub_key = ot_value.substr(0, end);
    ot_value                   = end == nostd::string_view::npos ? "" : ot_value.substr(end + 1);

    if (sub_key.substr(0, 3) == "rv:")
      return ParseHex56(sub_key.substr(3), randomness);
  }
  return false;
}

// Sub-keys that are empty, lack a key:value separator, or duplicate th are dropped.
std::string SetThresholdSubKey(nostd::string_view ot_value, const std::string &threshold)
{
  std::string result;
  bool replaced = false;
  while (!ot_value.empty())
  {
    size_t end                 = ot_value.find(';');
    nostd::string_view sub_key = ot_value.substr(0, end);
    ot_value                   = end == nostd::string_view::npos ? "" : ot_value.substr(end + 1);

    if (sub_key.empty() || sub_key.find(':') == nostd::string_view::npos)
      continue;
    if (sub_key.substr(0, 3) == "th:")
    {
      if (replaced)
        continue;
      replaced = true;
      if (!result.empty())
        result += ';';
      result += "th:" + threshold;
      continue;
    }
    if (!result.empty())
      result += ';';
    result.append(sub_key.data(), sub_key.size());
  }
  if (!replaced)
  {
    if (!result.empty())
      result += ';';
    result += "th:" + threshold;
  }
  return result;
}
}  // namespace

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
ProbabilitySampler::ProbabilitySampler(double ratio)
    : threshold_(CalculateRejectionThreshold(ratio))
{
  if (!(ratio > 0.0))
    ratio = 0.0;
  if (ratio > 1.0)
    ratio = 1.0;
  description_ = "ProbabilitySampler{" + std::to_string(ratio) + "}";
}

SamplingResult ProbabilitySampler::ShouldSample(
    const trace_api::SpanContext &parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view /*name*/,
    trace_api::SpanKind /*span_kind*/,
    const opentelemetry::common::KeyValueIterable & /*attributes*/,
    const trace_api::SpanContextKeyValueIterable & /*links*/) noexcept
{
  if (threshold_ == kMaxThreshold)
    return {Decision::DROP, nullptr, {}};

  auto parent_trace_state = parent_context.trace_state();
  std::string ot_value;
  bool has_ot = parent_trace_state->Get(kOtTraceStateKey, ot_value);

  uint64_t randomness = 0;
  if (!ExplicitRandomness(ot_value, &randomness))
  {
    if (parent_context.IsValid() && !parent_context.trace_flags().IsRandom())
    {
      static std::atomic<bool> warned{false};
      if (!warned.exchange(true))
      {
        OTEL_INTERNAL_LOG_WARN(
            "ProbabilitySampler presumes TraceID randomness, but the W3C random trace flag is "
            "not set. Upgrade the caller to W3C Trace Context Level 2.");
      }
    }
    randomness = RandomnessFromTraceId(trace_id);
  }

  if (randomness < threshold_)
    return {Decision::DROP, nullptr, {}};

  std::string new_ot_value = SetThresholdSubKey(ot_value, EncodeThreshold(threshold_));
  if (!trace_api::TraceState::IsValidValue(new_ot_value))
    return {Decision::RECORD_AND_SAMPLE, nullptr, {}};

  if (!has_ot)
  {
    size_t entry_count = 0;
    parent_trace_state->GetAllEntries([&entry_count](nostd::string_view, nostd::string_view) {
      ++entry_count;
      return true;
    });
    if (entry_count >= trace_api::TraceState::kMaxKeyValuePairs)
      return {Decision::RECORD_AND_SAMPLE, nullptr, {}};
  }

  auto trace_state =
      has_ot ? parent_trace_state->Delete(kOtTraceStateKey)->Set(kOtTraceStateKey, new_ot_value)
             : parent_trace_state->Set(kOtTraceStateKey, new_ot_value);

  return {Decision::RECORD_AND_SAMPLE, nullptr, trace_state};
}

nostd::string_view ProbabilitySampler::GetDescription() const noexcept
{
  return description_;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
