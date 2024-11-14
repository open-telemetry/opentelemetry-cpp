// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/tracer_config.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

const TracerConfig TracerConfig::kDefaultConfig  = TracerConfig();
const TracerConfig TracerConfig::kDisabledConfig = TracerConfig(true);

const TracerConfigurator TracerConfig::kDefaultTracerConfigurator =
    [](const instrumentationscope::InstrumentationScope &) { return Default(); };

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

const TracerConfigurator &TracerConfig::DefaultConfigurator()
{
  return kDefaultTracerConfigurator;
}

bool TracerConfig::IsEnabled() const noexcept
{
  return !disabled_;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
