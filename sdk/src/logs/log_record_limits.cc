// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <cstddef>
#include <cstdint>
#include <limits>

#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/sdk/logs/log_record_limits.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
namespace log_record_limits_env
{

namespace
{
static constexpr const char *kAttributeValueLengthLimitEnv =
    "OTEL_LOGRECORD_ATTRIBUTE_VALUE_LENGTH_LIMIT";
static constexpr std::size_t kDefaultAttributeValueLengthLimit =
    (std::numeric_limits<std::size_t>::max)();
}  // namespace

std::size_t GetAttributeValueLengthLimitFromEnv()
{
  std::uint32_t value{};
  if (!opentelemetry::sdk::common::GetUintEnvironmentVariable(kAttributeValueLengthLimitEnv, value))
  {
    return kDefaultAttributeValueLengthLimit;
  }
  return static_cast<std::size_t>(value);
}

}  // namespace log_record_limits_env

const LogRecordLimits &GetDefaultLogRecordLimits() noexcept
{
  static const LogRecordLimits default_limits = [] {
    LogRecordLimits limits;
    limits.attribute_value_length_limit =
        log_record_limits_env::GetAttributeValueLengthLimitFromEnv();
    return limits;
  }();
  return default_limits;
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
