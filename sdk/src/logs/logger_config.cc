// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/logs/logger_config.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
OPENTELEMETRY_EXPORT bool LoggerConfig::operator==(const LoggerConfig &other) const noexcept
{
  return enabled_ == other.enabled_ && minimum_severity_ == other.minimum_severity_ &&
         trace_based_ == other.trace_based_ &&
         log_record_limits_.attribute_value_length_limit ==
             other.log_record_limits_.attribute_value_length_limit;
}

OPENTELEMETRY_EXPORT bool LoggerConfig::IsEnabled() const noexcept
{
  return enabled_;
}

OPENTELEMETRY_EXPORT opentelemetry::logs::Severity LoggerConfig::GetMinimumSeverity() const noexcept
{
  return minimum_severity_;
}

OPENTELEMETRY_EXPORT bool LoggerConfig::IsTraceBased() const noexcept
{
  return trace_based_;
}

OPENTELEMETRY_EXPORT const LogRecordLimits &LoggerConfig::GetLogRecordLimits() const noexcept
{
  return log_record_limits_;
}

OPENTELEMETRY_EXPORT LoggerConfig
LoggerConfig::Create(bool enabled,
                     opentelemetry::logs::Severity minimum_severity,
                     bool trace_based,
                     const LogRecordLimits &log_record_limits) noexcept
{
  return LoggerConfig(enabled, minimum_severity, trace_based, log_record_limits);
}

OPENTELEMETRY_EXPORT LoggerConfig LoggerConfig::Enabled()
{
  return Default();
}

OPENTELEMETRY_EXPORT LoggerConfig LoggerConfig::Disabled()
{
  static const LoggerConfig disabled_config =
      Create(false, opentelemetry::logs::Severity::kInvalid, false, GetDefaultLogRecordLimits());
  return disabled_config;
}

OPENTELEMETRY_EXPORT LoggerConfig LoggerConfig::Default()
{
  static const LoggerConfig default_config = Default(GetDefaultLogRecordLimits());
  return default_config;
}

OPENTELEMETRY_EXPORT LoggerConfig LoggerConfig::Default(const LogRecordLimits &log_record_limits)
{
  return Create(true, opentelemetry::logs::Severity::kInvalid, false, log_record_limits);
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
