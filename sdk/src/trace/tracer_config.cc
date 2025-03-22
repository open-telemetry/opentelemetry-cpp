// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/tracer_config.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

TracerConfig TracerConfig::Disabled()
{
  static const auto kDisabledConfig = TracerConfig(true);
  return kDisabledConfig;
}

TracerConfig TracerConfig::Enabled()
{
  return Default();
}

TracerConfig TracerConfig::Default()
{
  static const auto kDefaultConfig = TracerConfig();
  return kDefaultConfig;
}

bool TracerConfig::IsEnabled() const noexcept
{
  return !disabled_;
}

bool TracerConfig::operator==(const TracerConfig &other) const noexcept
{
  return disabled_ == other.disabled_;
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
