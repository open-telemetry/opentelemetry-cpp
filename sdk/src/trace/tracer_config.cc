//
// Created by sharmapranav on 11/8/24.
//

#include "opentelemetry/sdk/trace/tracer_config.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

TracerConfig TracerConfig::kDefaultConfig  = TracerConfig();
TracerConfig TracerConfig::kDisabledConfig = TracerConfig(true);

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
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE