// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <limits>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
namespace log_record_limits_env
{

/// @brief Returns the attribute value length limit from the
/// OTEL_LOGRECORD_ATTRIBUTE_VALUE_LENGTH_LIMIT environment variable or unlimited if not set.
OPENTELEMETRY_EXPORT std::size_t GetAttributeValueLengthLimitFromEnv();

}  // namespace log_record_limits_env

/**
 * Configuration for limits applied to LogRecord attributes.
 */
struct OPENTELEMETRY_EXPORT LogRecordLimits
{
  std::size_t attribute_value_length_limit = (std::numeric_limits<std::size_t>::max)();
};

/// @brief Returns default LogRecordLimits initialized from environment variables.
OPENTELEMETRY_EXPORT const LogRecordLimits &GetDefaultLogRecordLimits() noexcept;

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
