// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/meter_config.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

const MeterConfig MeterConfig::kDefaultConfig  = MeterConfig();
const MeterConfig MeterConfig::kDisabledConfig = MeterConfig(true);

bool MeterConfig::operator==(const MeterConfig &other) const noexcept
{
  return disabled_ == other.disabled_;
}

bool MeterConfig::IsEnabled() const noexcept
{
  return !disabled_;
}

MeterConfig MeterConfig::Disabled()
{
  return kDisabledConfig;
}

MeterConfig MeterConfig::Enabled()
{
  return kDefaultConfig;
}

MeterConfig MeterConfig::Default()
{
  return kDefaultConfig;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
