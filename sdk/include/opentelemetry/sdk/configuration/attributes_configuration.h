// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <map>
#include <memory>
#include <string>

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/configuration/attribute_value_configuration.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/resource.json
// YAML-NODE: AttributeNameValue
class AttributesConfiguration
{
public:
  std::map<std::string, std::unique_ptr<AttributeValueConfiguration>> kv_map;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
