// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

using ResourceAttributes = opentelemetry::sdk::common::AttributeMap;

class Entity
{
public:
  /**
   * Constructs an Entity. Integer identity values (and integer arrays) are
   * canonicalized to int64_t when they fit, so C++ integer widths do not
   * affect identity equality or merge. Description values are not rewritten.
   */
  Entity(const std::string &type,
         const ResourceAttributes &identity,
         const ResourceAttributes &description = ResourceAttributes{},
         const std::string &schema_url         = std::string{});

  const std::string &GetType() const noexcept;
  const ResourceAttributes &GetIdentity() const noexcept;
  const ResourceAttributes &GetDescription() const noexcept;
  const std::string &GetSchemaURL() const noexcept;

  bool IsValid() const noexcept;

  bool operator==(const Entity &other) const noexcept;

private:
  std::string type_;
  ResourceAttributes identity_;
  ResourceAttributes description_;
  std::string schema_url_;
};

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
