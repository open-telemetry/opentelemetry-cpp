// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/meter_provider.yaml
// YAML-NODE: ViewMatchingMode
enum class ViewMatchingMode : std::uint8_t
{
  independent,
  composable
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
