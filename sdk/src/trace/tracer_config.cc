// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/tracer_config.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

const TracerConfig TracerConfig::kDefaultConfig  = TracerConfig();
const TracerConfig TracerConfig::kDisabledConfig = TracerConfig(true);

TracerConfig TracerConfig::Disabled()
{
  return kDisabledConfig;
}

TracerConfig TracerConfig::Enabled()
{
  return kDefaultConfig;
}

TracerConfig TracerConfig::Default()
{
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
