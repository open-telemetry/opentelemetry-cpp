// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/attribute_validity.h"

#include <map>
#include <ostream>
#include <unordered_set>
#include <utility>

#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/version.h"

#include "src/common/internal/utf8_range/utf8_range.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{

namespace
{
static AttributeValidator &GetSharedAttributeValidator() noexcept
{
  static AttributeValidator validator;
  return validator;
}
}  // namespace

OPENTELEMETRY_EXPORT bool AttributeIsValidString(nostd::string_view value) noexcept
{
  return 0 != utf8_range::utf8_range_IsValid(value.data(), value.size());
}

OPENTELEMETRY_EXPORT bool AttributeValidator::IsValid(const std::string &value) noexcept
{
  return AttributeIsValidString(value);
}

OPENTELEMETRY_EXPORT bool AttributeValidator::IsValid(nostd::string_view value) noexcept
{
  return AttributeIsValidString(value);
}

OPENTELEMETRY_EXPORT bool AttributeValidator::IsValid(const OwnedAttributeValue &value) noexcept
{
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  try
  {
#endif

    return nostd::visit(GetSharedAttributeValidator(), value);

#if OPENTELEMETRY_HAVE_EXCEPTIONS
  }
  catch (...)
  {
    return false;
  }
#endif
}

OPENTELEMETRY_EXPORT bool AttributeValidator::IsValid(
    const opentelemetry::common::AttributeValue &value) noexcept
{
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  try
  {
#endif

    return nostd::visit(GetSharedAttributeValidator(), value);

#if OPENTELEMETRY_HAVE_EXCEPTIONS
  }
  catch (...)
  {
    return false;
  }
#endif
}

OPENTELEMETRY_EXPORT bool AttributeValidator::IsAllValid(const AttributeMap &attributes) noexcept
{
  for (const auto &kv : attributes)
  {
    if (!AttributeValidator::IsValid(kv.second))
    {
      return false;
    }
  }
  return true;
}

OPENTELEMETRY_EXPORT bool AttributeValidator::IsAllValid(
    const OrderedAttributeMap &attributes) noexcept
{
  for (const auto &kv : attributes)
  {
    if (!AttributeValidator::IsValid(kv.second))
    {
      return false;
    }
  }
  return true;
}

OPENTELEMETRY_EXPORT void AttributeValidator::Filter(AttributeMap &attributes,
                                                     nostd::string_view log_hint)
{
  std::unordered_set<std::string> invalid_keys;
  for (auto &kv : attributes)
  {
    if (!common::AttributeValidator::IsValid(kv.first))
    {
      OTEL_INTERNAL_LOG_WARN(log_hint << " Invalid attribute key " << kv.first
                                      << ". This attribute will be ignored.");

      invalid_keys.insert(kv.first);
      continue;
    }

    if (!common::AttributeValidator::IsValid(kv.second))
    {
      OTEL_INTERNAL_LOG_WARN(log_hint << " Invalid attribute value for " << kv.first
                                      << ". This attribute will be ignored.");

      invalid_keys.insert(kv.first);
    }
  }

  for (auto &invalid_key : invalid_keys)
  {
    attributes.erase(invalid_key);
  }
}

OPENTELEMETRY_EXPORT void AttributeValidator::Filter(OrderedAttributeMap &attributes,
                                                     nostd::string_view log_hint)
{
  std::unordered_set<std::string> invalid_keys;
  for (auto &kv : attributes)
  {
    if (!common::AttributeValidator::IsValid(kv.first))
    {
      OTEL_INTERNAL_LOG_WARN(log_hint << " Invalid attribute key " << kv.first
                                      << ". This attribute will be ignored.");

      invalid_keys.insert(kv.first);
      continue;
    }

    if (!common::AttributeValidator::IsValid(kv.second))
    {
      OTEL_INTERNAL_LOG_WARN(log_hint << " Invalid attribute value for " << kv.first
                                      << ". This attribute will be ignored.");

      invalid_keys.insert(kv.first);
    }
  }

  for (auto &invalid_key : invalid_keys)
  {
    attributes.erase(invalid_key);
  }
}

KeyValueFilterIterable::KeyValueFilterIterable(
    const opentelemetry::common::KeyValueIterable &origin,
    opentelemetry::nostd::string_view log_hint) noexcept
    : origin_(&origin), size_(static_cast<size_t>(-1)), log_hint_(log_hint)
{}

KeyValueFilterIterable::~KeyValueFilterIterable() {}

bool KeyValueFilterIterable::ForEachKeyValue(
    opentelemetry::nostd::function_ref<bool(opentelemetry::nostd::string_view,
                                            opentelemetry::common::AttributeValue)> callback)
    const noexcept
{
  size_t size = 0;
  bool ret =
      origin_->ForEachKeyValue([&size, &callback, this](opentelemetry::nostd::string_view k,
                                                        opentelemetry::common::AttributeValue v) {
        if (AttributeValidator::IsValid(k) && AttributeValidator::IsValid(v))
        {
          ++size;
          return callback(k, v);
        }

        OTEL_INTERNAL_LOG_WARN(log_hint_ << " Invalid value for: " << k << ". It will be ignored.");
        return true;
      });

  // If it return true, we already iterated over all key-values. The the size can be updated.
  if (ret)
  {
    size_ = size;
  }

  return ret;
}

size_t KeyValueFilterIterable::size() const noexcept
{
  // Use cached size if it was already calculated.
  if (size_ != static_cast<size_t>(-1))
  {
    return size_;
  }

  size_t size = 0;
  origin_->ForEachKeyValue(
      [&size](opentelemetry::nostd::string_view k, opentelemetry::common::AttributeValue v) {
        if (AttributeValidator::IsValid(k) && AttributeValidator::IsValid(v))
        {
          ++size;
        }
        return true;
      });

  size_ = size;
  return size_;
}

}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
