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
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  return enabled_ == other.enabled_ && minimum_severity_ == other.minimum_severity_ &&
         trace_based_ == other.trace_based_;
#else
  return enabled_ == other.enabled_;
#endif
}

OPENTELEMETRY_EXPORT bool LoggerConfig::IsEnabled() const noexcept
{
  return enabled_;
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
OPENTELEMETRY_EXPORT opentelemetry::logs::Severity LoggerConfig::GetMinimumSeverity() const noexcept
{
  return minimum_severity_;
}

OPENTELEMETRY_EXPORT bool LoggerConfig::IsTraceBased() const noexcept
{
  return trace_based_;
}

OPENTELEMETRY_EXPORT LoggerConfig
LoggerConfig::Create(bool enabled,
                     opentelemetry::logs::Severity minimum_severity,
                     bool trace_based) noexcept
{
  return LoggerConfig(enabled, minimum_severity, trace_based);
}
#endif  // OPENTELEMETRY_ABI_VERSION_NO >= 2

OPENTELEMETRY_EXPORT LoggerConfig LoggerConfig::Enabled()
{
  return Default();
}

OPENTELEMETRY_EXPORT LoggerConfig LoggerConfig::Disabled()
{
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  static const auto kDisabledConfig = Create(false, opentelemetry::logs::Severity::kInvalid, false);
#else
  static const auto kDisabledConfig = LoggerConfig(false);
#endif
  return kDisabledConfig;
}

OPENTELEMETRY_EXPORT LoggerConfig LoggerConfig::Default()
{
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  static const auto kDefaultConfig = Create(true, opentelemetry::logs::Severity::kInvalid, false);
#else
  static const auto kDefaultConfig = LoggerConfig();
#endif
  return kDefaultConfig;
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
