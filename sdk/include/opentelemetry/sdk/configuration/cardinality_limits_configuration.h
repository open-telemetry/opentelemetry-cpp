// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/meter_provider.yaml
// YAML-NODE: CardinalityLimits
class CardinalityLimitsConfiguration
{
public:
  std::size_t default_limit{0};
  // For all limits, 0 means unset, use default_limit
  std::size_t counter{0};
  std::size_t gauge{0};
  std::size_t histogram{0};
  std::size_t observable_counter{0};
  std::size_t observable_gauge{0};
  std::size_t observable_up_down_counter{0};
  std::size_t up_down_counter{0};
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
