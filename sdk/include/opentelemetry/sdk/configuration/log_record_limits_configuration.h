// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <limits>

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
  static constexpr std::size_t kDefaultAttributeValueLengthLimit =
      (std::numeric_limits<std::size_t>::max)();
  static constexpr std::size_t kDefaultAttributeCountLimit = 128;

  std::size_t attribute_value_length_limit{kDefaultAttributeValueLengthLimit};
  std::size_t attribute_count_limit{kDefaultAttributeCountLimit};
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
