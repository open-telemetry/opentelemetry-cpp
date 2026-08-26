// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/resource/entity.h"

#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

Entity::Entity(const std::string &type,
               const ResourceAttributes &identity,
               const ResourceAttributes &description,
               const std::string &schema_url) noexcept
    : type_(type), identity_(identity), description_(description), schema_url_(schema_url)
{
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
