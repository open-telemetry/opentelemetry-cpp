// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
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
    if (limits_.attribute_value_length_limit == (std::numeric_limits<std::size_t>::max)())
    {
      SetAttributeImpl(key, value);
      return;
    }

#if OPENTELEMETRY_HAVE_EXCEPTIONS
    try
    {
#endif
      AttributeValueLengthLimiter limiter(limits_.attribute_value_length_limit);
      auto limited_value = opentelemetry::nostd::visit(limiter, value);
      SetAttributeImpl(key, limited_value);
#if OPENTELEMETRY_HAVE_EXCEPTIONS
    }
    catch (const std::bad_alloc &)
    {
      // Out of memory while limiting the value: drop the attribute instead of terminating
      // (SetAttribute is noexcept). Deliberately no logging here
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
    // never splitting a multi-byte sequence. Malformed lead/continuation bytes are counted as
    // one byte each (we do not validate or repair user-supplied invalid UTF-8).
    static std::size_t Utf8PrefixLength(opentelemetry::nostd::string_view value,
                                        std::size_t limit) noexcept
    {
      std::size_t i = 0, codepoints = 0;
      while (i < value.size() && codepoints < limit)
      {
        const auto lead = static_cast<unsigned char>(value[i]);
        std::size_t seq = (lead < 0x80)   ? 1  // ASCII
                          : (lead < 0xC0) ? 1  // stray continuation -> pass 1 byte
                          : (lead < 0xE0) ? 2
                          : (lead < 0xF0) ? 3
                          : (lead < 0xF8) ? 4
                                          : 1;  // invalid lead -> pass 1 byte
        if (i + seq > value.size())
        {
          break;  // incomplete sequence at end of input: stop before it
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
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
