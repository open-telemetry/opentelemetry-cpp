// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/logs/logger_config.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

const LoggerConfig LoggerConfig::kDefaultConfig  = LoggerConfig();
const LoggerConfig LoggerConfig::kDisabledConfig = LoggerConfig(true);

bool LoggerConfig::operator==(const LoggerConfig &other) const noexcept
{
  return disabled_ == other.disabled_;
}

bool LoggerConfig::IsEnabled() const noexcept
{
  return !disabled_;
}

LoggerConfig LoggerConfig::Enabled()
{
  return kDefaultConfig;
}

LoggerConfig LoggerConfig::Disabled()
{
  return kDisabledConfig;
}

LoggerConfig LoggerConfig::Default()
{
  return kDefaultConfig;
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
