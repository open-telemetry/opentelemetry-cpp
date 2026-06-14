// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/macros.h"
#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/logs/log_record_limits.h"
#include "opentelemetry/version.h"

#if OPENTELEMETRY_HAVE_EXCEPTIONS
#  include <exception>
#  include <new>

#  include "opentelemetry/sdk/common/global_log_handler.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{
class Resource;
}  // namespace resource

namespace instrumentationscope
{
class InstrumentationScope;
}  // namespace instrumentationscope

namespace logs
{
class MultiRecordable;

/**
 * Maintains a representation of a log in a format that can be processed by a recorder.
 *
 * This class is thread-compatible.
 */

class Recordable : public opentelemetry::logs::LogRecord
{
  friend class MultiRecordable;

public:
  void SetLogRecordLimits(const LogRecordLimits &limits) noexcept { limits_ = limits; }

  void SetAttribute(opentelemetry::nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept final
  {
#if OPENTELEMETRY_HAVE_EXCEPTIONS
    try
    {
#endif
      if (attribute_keys_.size() < limits_.attribute_count_limit)
      {
        attribute_keys_.insert(std::string(key));
      }
      else if (attribute_keys_.find(std::string(key)) == attribute_keys_.end())
      {
        ++dropped_attributes_count_;
        return;
      }

      if (limits_.attribute_value_length_limit == (std::numeric_limits<std::size_t>::max)())
      {
        SetAttributeImpl(key, value);
      }
      else
      {
        AttributeValueLengthLimiter limiter(limits_.attribute_value_length_limit);
        auto limited_value = opentelemetry::nostd::visit(limiter, value);
        SetAttributeImpl(key, limited_value);
      }
#if OPENTELEMETRY_HAVE_EXCEPTIONS
    }
    catch (const std::bad_alloc &)
    {
      // Out of memory while applying limits: drop the attribute instead of terminating
      // (SetAttribute is noexcept). Deliberately no logging here -- the log path allocates.
      return;
    }
    catch (const std::exception &e)
    {
      // Should not happen for AttributeValue (never valueless); surface it if it ever does.
      OTEL_INTERNAL_LOG_ERROR("Recordable::SetAttribute dropped attribute: " << e.what());
      return;
    }
#endif
  }

  /**
   * Returns the number of attributes discarded because the attribute count limit was reached.
   */
  std::size_t GetAttributeDroppedCount() const noexcept { return dropped_attributes_count_; }

  /**
   * Set Resource of this log
   * @param resource the resource to set
   */
  virtual void SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept = 0;

  /**
   * Set instrumentation_scope for this log.
   * @param instrumentation_scope the instrumentation scope to set
   */
  virtual void SetInstrumentationScope(
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope
          &instrumentation_scope) noexcept = 0;

protected:
  const LogRecordLimits &GetLogRecordLimits() const noexcept { return limits_; }

  /**
   * Set an attribute after SDK-level processing is applied.
   *
   * AttributeValue can contain non-owning views. Implementations must consume or copy value
   * synchronously before this method returns.
   */
  virtual void SetAttributeImpl(opentelemetry::nostd::string_view key,
                                const opentelemetry::common::AttributeValue &value) noexcept = 0;

private:
  class AttributeValueLengthLimiter
  {
  public:
    explicit AttributeValueLengthLimiter(std::size_t limit) noexcept : limit_(limit) {}

    opentelemetry::common::AttributeValue operator()(const char *value) const noexcept
    {
      if (value == nullptr)
      {
        return value;
      }

      const auto value_length = std::strlen(value);
      if (value_length <= limit_)
      {
        return value;
      }

      const auto truncated =
          Utf8PrefixLength(opentelemetry::nostd::string_view(value, value_length), limit_);
      return opentelemetry::nostd::string_view(value, truncated);
    }

    opentelemetry::common::AttributeValue operator()(
        opentelemetry::nostd::string_view value) const noexcept
    {
      if (value.size() <= limit_)
      {
        return value;
      }

      return opentelemetry::nostd::string_view(value.data(), Utf8PrefixLength(value, limit_));
    }

    opentelemetry::common::AttributeValue operator()(
        opentelemetry::nostd::span<const opentelemetry::nostd::string_view> values) noexcept
    {
      if (!HasLimitedStringValue(values))
      {
        return values;
      }

      limited_string_values_.clear();
      limited_string_values_.reserve(values.size());
      for (auto value : values)
      {
        limited_string_values_.push_back(GetLimitedStringValue(value));
      }

      return opentelemetry::nostd::span<const opentelemetry::nostd::string_view>(
          limited_string_values_);
    }

    opentelemetry::common::AttributeValue operator()(
        opentelemetry::nostd::span<const uint8_t> value) const noexcept
    {
      if (value.size() <= limit_)
      {
        return value;
      }

      return opentelemetry::nostd::span<const uint8_t>(value.data(), limit_);
    }

    template <typename T>
    opentelemetry::common::AttributeValue operator()(T value) const noexcept
    {
      return value;
    }

  private:
    bool HasLimitedStringValue(
        opentelemetry::nostd::span<const opentelemetry::nostd::string_view> values) const noexcept
    {
      for (auto value : values)
      {
        if (value.size() > limit_)
        {
          return true;
        }
      }

      return false;
    }

    opentelemetry::nostd::string_view GetLimitedStringValue(
        opentelemetry::nostd::string_view value) const noexcept
    {
      if (value.size() <= limit_)
      {
        return value;
      }

      return opentelemetry::nostd::string_view(value.data(), Utf8PrefixLength(value, limit_));
    }

    // Byte length of the longest prefix of `value` holding at most `limit` UTF-8 code points,
    // never splitting a well-formed multi-byte sequence. A lead byte's declared length is only
    // honored when its continuation bytes are present and in range (0x80-0xBF); otherwise the lead
    // is counted as a single byte, so malformed / non-UTF-8 input degrades to plain byte
    // truncation. This is deliberately NOT full UTF-8 validation (overlong, surrogate, and
    // out-of-range sequences pass through as one character each) -- the exporter decides real
    // UTF-8 validity when it routes invalid strings to a bytes value.
    static std::size_t Utf8PrefixLength(opentelemetry::nostd::string_view value,
                                        std::size_t limit) noexcept
    {
      std::size_t i = 0, codepoints = 0;
      while (i < value.size() && codepoints < limit)
      {
        const auto lead = static_cast<unsigned char>(value[i]);
        // Bytes in this character, decoded from the lead byte. UTF-8 byte layout
        // (https://en.wikipedia.org/wiki/UTF-8#Description):
        //   0x00-0x7F  0xxxxxxx  ASCII, 1 byte
        //   0x80-0xBF  10xxxxxx  continuation byte (never a valid lead)
        //   0xC0-0xDF  110xxxxx  lead of a 2-byte sequence
        //   0xE0-0xEF  1110xxxx  lead of a 3-byte sequence
        //   0xF0-0xF7  11110xxx  lead of a 4-byte sequence
        //   0xF8-0xFF            not a valid lead byte
        std::size_t seq = (lead < 0x80)   ? 1
                          : (lead < 0xC0) ? 1
                          : (lead < 0xE0) ? 2
                          : (lead < 0xF0) ? 3
                          : (lead < 0xF8) ? 4
                                          : 1;

        // Honor the multi-byte length only if the continuation bytes are present and valid;
        // otherwise the lead is a lone invalid byte and counts as one character.
        if (seq > 1)
        {
          if (i + seq > value.size())
          {
            seq = 1;
          }
          else
          {
            for (std::size_t k = 1; k < seq; ++k)
            {
              const auto continuation = static_cast<unsigned char>(value[i + k]);
              if (continuation < 0x80 || continuation > 0xBF)
              {
                seq = 1;
                break;
              }
            }
          }
        }

        i += seq;
        ++codepoints;
      }
      return i;
    }

    std::size_t limit_;
    std::vector<opentelemetry::nostd::string_view> limited_string_values_;
  };

  LogRecordLimits limits_;
  std::unordered_set<std::string> attribute_keys_;
  std::size_t dropped_attributes_count_ = 0;
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
