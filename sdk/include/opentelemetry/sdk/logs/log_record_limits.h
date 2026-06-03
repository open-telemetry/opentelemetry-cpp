// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * Configuration for limits applied to LogRecord attributes.
 */
struct LogRecordLimits
{
  std::size_t attribute_value_length_limit = std::numeric_limits<size_t>::infinity();
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
