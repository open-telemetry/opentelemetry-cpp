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

enum enum_temporality_preference : std::uint8_t
{
  cumulative,
  delta,
  low_memory
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
