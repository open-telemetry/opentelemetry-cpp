// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/resource/entity.h"

#include <cstdint>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{
namespace
{

namespace nostd           = opentelemetry::nostd;
using OwnedAttributeValue = opentelemetry::sdk::common::OwnedAttributeValue;

bool Uint64FitsInt64(std::uint64_t value) noexcept
{
  return value <= static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
}

template <typename T>
OwnedAttributeValue WidenIntegerArray(const std::vector<T> &src)
{
  std::vector<std::int64_t> dst;
  dst.reserve(src.size());
  for (T value : src)
  {
    dst.push_back(static_cast<std::int64_t>(value));
  }
  return OwnedAttributeValue{std::move(dst)};
}

OwnedAttributeValue NormalizeIntegerIdentityValue(const OwnedAttributeValue &value)
{
  if (nostd::holds_alternative<std::int32_t>(value))
  {
    return OwnedAttributeValue{static_cast<std::int64_t>(nostd::get<std::int32_t>(value))};
  }
  if (nostd::holds_alternative<std::uint32_t>(value))
  {
    return OwnedAttributeValue{static_cast<std::int64_t>(nostd::get<std::uint32_t>(value))};
  }
  if (nostd::holds_alternative<std::uint64_t>(value))
  {
    const std::uint64_t raw = nostd::get<std::uint64_t>(value);
    if (Uint64FitsInt64(raw))
    {
      return OwnedAttributeValue{static_cast<std::int64_t>(raw)};
    }
    return value;
  }
  if (nostd::holds_alternative<std::vector<std::int32_t>>(value))
  {
    return WidenIntegerArray(nostd::get<std::vector<std::int32_t>>(value));
  }
  if (nostd::holds_alternative<std::vector<std::uint32_t>>(value))
  {
    return WidenIntegerArray(nostd::get<std::vector<std::uint32_t>>(value));
  }
  if (nostd::holds_alternative<std::vector<std::uint64_t>>(value))
  {
    const auto &src = nostd::get<std::vector<std::uint64_t>>(value);
    for (std::uint64_t element : src)
    {
      if (!Uint64FitsInt64(element))
      {
        return value;
      }
    }
    return WidenIntegerArray(src);
  }
  return value;
}

void NormalizeIdentityIntegers(ResourceAttributes &identity)
{
  for (auto &kv : identity)
  {
    kv.second = NormalizeIntegerIdentityValue(kv.second);
  }
}

}  // namespace

Entity::Entity(const std::string &type,
               const ResourceAttributes &identity,
               const ResourceAttributes &description,
               const std::string &schema_url)
    : type_(type), identity_(identity), description_(description), schema_url_(schema_url)
{
  NormalizeIdentityIntegers(identity_);
  for (const auto &kv : identity_)
  {
    description_.erase(kv.first);
  }
}

const std::string &Entity::GetType() const noexcept
{
  return type_;
}

const ResourceAttributes &Entity::GetIdentity() const noexcept
{
  return identity_;
}

const ResourceAttributes &Entity::GetDescription() const noexcept
{
  return description_;
}

const std::string &Entity::GetSchemaURL() const noexcept
{
  return schema_url_;
}

bool Entity::IsValid() const noexcept
{
  return !type_.empty() && !identity_.empty();
}

bool Entity::operator==(const Entity &other) const noexcept
{
  return type_ == other.type_ && identity_ == other.identity_ &&
         description_ == other.description_ && schema_url_ == other.schema_url_;
}

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
