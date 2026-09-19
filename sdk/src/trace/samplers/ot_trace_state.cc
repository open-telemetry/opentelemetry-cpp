// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "ot_trace_state.h"

#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <new>
#include <ostream>
#include <stdexcept>
#include <string>

#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/trace_state.h"
#include "opentelemetry/version.h"

namespace
{

constexpr char kHexDigits[] = "0123456789abcdef";

// TraceState::Set drops the whole tracestate when a value is longer than this,
// so the entire "ot" value has to fit within it.
constexpr std::size_t kMaxOtValueSize =
    static_cast<std::size_t>(opentelemetry::trace::TraceState::kValueMaxSize);

// Number of contiguous zero bits at the least significant end of value.
// value is guaranteed non-zero by the callers.
int CountTrailingZeroBits(uint64_t value) noexcept
{
  int count = 0;
  while ((value & 1U) == 0U)
  {
    value >>= 1;
    ++count;
  }
  return count;
}

// threshold == 0 (AlwaysOn / 100% sampling ratio) always encodes as a single
// "0" digit, so it is special-cased to skip the bit-scan entirely -- it's the
// only value for which CountTrailingZeroBits would need the bit-55 sentinel.
int ThresholdHexStartShift(uint64_t threshold) noexcept
{
  if (threshold == 0)
  {
    return 52;
  }
  return CountTrailingZeroBits(threshold) - 3;
}

// Number of hex digits AppendThreshold emits for a threshold whose
// ThresholdHexStartShift() is start_shift (trailing zero nibbles dropped; a
// threshold of 0 counts as a single digit). Takes start_shift directly, since
// callers that also append the digits already computed it once.
std::size_t ThresholdHexDigitCount(int start_shift) noexcept
{
  return static_cast<std::size_t>(52 - start_shift) / 4 + 1;
}

// Appends "th:" followed by the 56-bit threshold as most-significant-first
// hex digits starting at start_shift (as returned by ThresholdHexStartShift),
// dropping trailing zero nibbles. A threshold of 0 is emitted as a single "0".
void AppendThreshold(std::string &out, uint64_t threshold, int start_shift) noexcept
{
  out.append("th:");
  for (int shift = 52; shift >= start_shift; shift -= 4)
  {
    out.push_back(kHexDigits[(threshold >> shift) & 0xFU]);
  }
}

// Appends "rv:" followed by the 56-bit randomness value as exactly 14
// zero-padded hex digits.
void AppendRandomnessValue(std::string &out, uint64_t value) noexcept
{
  if (!out.empty())
  {
    out.push_back(';');
  }
  out.append("rv:");
  for (int shift = 52; shift >= 0; shift -= 4)
  {
    out.push_back(kHexDigits[(value >> shift) & 0xFU]);
  }
}

void AppendOtherSubkeys(
    std::string &out,
    const std::vector<opentelemetry::nostd::string_view> &other_subkeys) noexcept
{
  for (const auto &subkey : other_subkeys)
  {
    if (!out.empty())
    {
      out.push_back(';');
    }
    out.append(subkey.begin(), subkey.end());
  }
}

// Parses len lowercase hex digits starting at s[start] into a left-aligned
// 56-bit value. Returns false on any non-hex character.
bool ParseHex(opentelemetry::nostd::string_view s,
              std::size_t start,
              std::size_t len,
              uint64_t &result) noexcept
{
  uint64_t r = 0;
  for (std::size_t i = 0; i < len; ++i)
  {
    char c     = s[start + i];
    int nibble = 0;
    if (c >= '0' && c <= '9')
    {
      nibble = c - '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
      nibble = c - 'a' + 10;
    }
    else
    {
      return false;
    }
    int shift = 52 - static_cast<int>(i) * 4;
    r |= static_cast<uint64_t>(nibble) << shift;
  }
  result = r;
  return true;
}

// Whether "th" fits within the tracestate value size limit alongside
// rest_size bytes of "rv"/other sub-keys. Logs a one-time warning and
// returns false if it doesn't; otherwise fills in th_size/th_start_shift for
// the caller to use when writing it.
bool ShouldAppendThreshold(uint64_t threshold,
                           std::size_t rest_size,
                           std::size_t &th_size,
                           int &th_start_shift) noexcept
{
  th_start_shift                  = ThresholdHexStartShift(threshold);
  th_size                         = 3 + ThresholdHexDigitCount(th_start_shift);
  const std::size_t rest_with_sep = rest_size != 0 ? rest_size + 1 : 0;
  if (th_size + rest_with_sep <= kMaxOtValueSize)
  {
    return true;
  }

  static std::atomic<bool> warned{false};
  if (!warned.exchange(true))
  {
    OTEL_INTERNAL_LOG_WARN("[OtelTraceState] omitting th: recording it would exceed the "
                           << kMaxOtValueSize << " character tracestate value limit");
  }
  return false;
}

// Total bytes Serialize() needs to reserve: rest_size, plus th_size and a
// separator when a threshold will be emitted.
std::size_t ComputeReserveSize(bool emit_threshold,
                               std::size_t th_size,
                               std::size_t rest_size) noexcept
{
  std::size_t total = rest_size;
  if (emit_threshold)
  {
    total += th_size;
    if (rest_size != 0)
    {
      ++total;  // separator between "th" and the rest
    }
  }
  return total;
}

}  // namespace

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

uint64_t CalculateThreshold(double sampling_probability) noexcept
{
  if (sampling_probability <= 0.0)
  {
    return kMaxThreshold;
  }
  if (sampling_probability >= 1.0)
  {
    return 0;
  }
  // kMaxThreshold == 2^56. sampling_probability is in (0, 1) here, so the
  // rounded product is in [0, kMaxThreshold] and the subtraction never
  // underflows.
  uint64_t kept = static_cast<uint64_t>(
      std::llround(sampling_probability * static_cast<double>(kMaxThreshold)));
  return kMaxThreshold - kept;
}

double ValidateRatio(double ratio, const char *sampler_name) noexcept
{
  // 2^-56; hex float literals would need C++17.
  constexpr double kMinRatio = 1.0 / static_cast<double>(kMaxThreshold);
  if (ratio == 0.0 || (ratio >= kMinRatio && ratio <= 1.0))
  {
    return ratio;
  }
  OTEL_INTERNAL_LOG_WARN("[" << sampler_name << "] ratio " << ratio
                             << " is not 0 or within [2^-56, 1.0], using the default 1.0");
  return 1.0;
}

uint64_t GetRandomnessFromTraceId(const opentelemetry::trace::TraceId &trace_id) noexcept
{
  const uint8_t *data = trace_id.Id().data();
  uint64_t randomness = 0;
  for (std::size_t i = opentelemetry::trace::TraceId::kSize - 7;
       i < opentelemetry::trace::TraceId::kSize; ++i)
  {
    randomness = (randomness << 8) | data[i];
  }
  return randomness;
}

nostd::string_view GetOtValue(
    const nostd::shared_ptr<opentelemetry::trace::TraceState> &trace_state) noexcept
{
  if (!trace_state || trace_state->Empty())
  {
    return nostd::string_view();
  }

  nostd::string_view ot_value;
  trace_state->GetAllEntries(
      [&ot_value](opentelemetry::nostd::string_view key, opentelemetry::nostd::string_view value) {
        if (key == kOtTraceStateKey)
        {
          ot_value = value;
          return false;  // stop iteration
        }
        return true;  // continue iteration
      });

  return ot_value;
}

OtelTraceState OtelTraceState::Parse(nostd::string_view ot_value) noexcept
{
  OtelTraceState state;
  const std::size_t len = ot_value.size();
  if (len == 0 || len > kMaxOtValueSize)
  {
    return state;
  }

#if OPENTELEMETRY_HAVE_EXCEPTIONS
  try
  {
#endif
    std::size_t pos = 0;
    while (pos < len)
    {
      std::size_t sep   = ot_value.find(';', pos);
      std::size_t end   = (sep == std::string::npos) ? len : sep;
      std::size_t colon = ot_value.substr(pos, end - pos).find(':');

      if (colon != std::string::npos)
      {
        colon += pos;
        const std::size_t value_start = colon + 1;
        const std::size_t value_len   = end - value_start;
        const std::size_t key_len     = colon - pos;
        if (key_len == 2 && ot_value.compare(pos, 2, "th") == 0)
        {
          uint64_t threshold_value{0};
          if (value_len <= 14 && ParseHex(ot_value, value_start, value_len, threshold_value))
          {
            state.has_threshold = true;
            state.threshold     = threshold_value;
          }
        }
        else if (key_len == 2 && ot_value.compare(pos, 2, "rv") == 0)
        {
          uint64_t random_value{0};
          if (value_len == 14 && ParseHex(ot_value, value_start, value_len, random_value))
          {
            state.has_random_value = true;
            state.random_value     = random_value;
          }
        }
        else
        {
          state.other_subkeys.push_back(ot_value.substr(pos, end - pos));
        }
      }

      if (sep == std::string::npos)
      {
        break;
      }
      pos = sep + 1;
    }
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  }
  // substr()/compare() only throw std::out_of_range when pos exceeds the
  // value's length, which the loop bounds never allow; stay defensive anyway.
  catch (const std::out_of_range &)
  {
    return state;
  }
  catch (const std::bad_alloc &)
  {
    // Out of memory recording a sub-key: keep whatever th/rv was parsed so far.
    return state;
  }
#endif
  return state;
}

std::string OtelTraceState::Serialize() const
{
  // Inherited sub-keys are never dropped (the tracestate spec requires
  // preserving existing OpenTelemetry concerns); when adding "th" would push
  // the value past kMaxOtValueSize, the new threshold is omitted instead.
  std::size_t rest_size = has_random_value ? 17 : 0;  // "rv:" + 14 hex digits
  for (const auto &pair : other_subkeys)
  {
    rest_size += (rest_size != 0 ? 1 : 0) + pair.size();
  }

  bool emit_threshold = false;
  std::size_t th_size = 0;
  int th_start_shift  = 0;
  if (has_threshold && threshold < kMaxThreshold)
  {
    emit_threshold = ShouldAppendThreshold(threshold, rest_size, th_size, th_start_shift);
  }

  const std::size_t total_size = ComputeReserveSize(emit_threshold, th_size, rest_size);
  std::string out;
  out.reserve(total_size);

  if (emit_threshold)
  {
    AppendThreshold(out, threshold, th_start_shift);
  }
  if (has_random_value)
  {
    AppendRandomnessValue(out, random_value);
  }
  if (!other_subkeys.empty())
  {
    AppendOtherSubkeys(out, other_subkeys);
  }
  return out;
}

uint64_t GetSamplingRandomness(const OtelTraceState &ot_state,
                               const opentelemetry::trace::TraceId &trace_id) noexcept
{
  if (ot_state.has_random_value)
  {
    return ot_state.random_value;
  }
  return GetRandomnessFromTraceId(trace_id);
}

nostd::shared_ptr<opentelemetry::trace::TraceState> GetTraceStateForOtValue(
    const OtelTraceState &ot_state,
    bool had_threshold,
    uint64_t had_threshold_value,
    nostd::string_view ot_value,
    nostd::shared_ptr<opentelemetry::trace::TraceState> trace_state)
{
  // "th" is the only field ever mutated here. "rv" and other sub-keys always
  // pass through unchanged. So if the threshold state numerically matches what
  // was parsed, the effective "ot" content is unchanged regardless of how it
  // was ordered/formatted on input, and nothing needs to be (re)serialized or
  // written.
  const bool threshold_changed =
      ot_state.has_threshold != had_threshold ||
      (ot_state.has_threshold && ot_state.threshold != had_threshold_value);
  if (!threshold_changed)
  {
    return trace_state;
  }

  std::string new_ot_value = ot_state.Serialize();
  // Set()/Delete() always deep-copy the tracestate, so only call them when
  // the "ot" sub-key actually needs to change.
  if (new_ot_value.empty())
  {
    // An "ot" entry can never have an empty value, so an empty ot_value means
    // there was no "ot" key to remove, regardless of other tracestate entries.
    if (!ot_value.empty())
    {
      trace_state = trace_state->Delete(kOtTraceStateKey);
    }
  }
  else
  {
    trace_state = trace_state->Set(kOtTraceStateKey, new_ot_value);
  }
  return trace_state;
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
