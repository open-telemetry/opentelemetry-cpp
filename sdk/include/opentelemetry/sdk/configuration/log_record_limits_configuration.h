// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/logger_provider.json
// YAML-NODE: LogRecordLimits
class LogRecordLimitsConfiguration
{
public:
  // Zero values indicate the field is not set.
  std::size_t attribute_value_length_limit{0};
  std::size_t attribute_count_limit{0};
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
