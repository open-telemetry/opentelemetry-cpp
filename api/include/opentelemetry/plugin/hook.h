#pragma once

#include "opentelemetry/plugin/loader_info.h"
#include "opentelemetry/plugin/factory.h"

#define OPENTELEMETRY_PLUGIN_HOOK OpenTelemetryMakeFactoryImpl

namespace opentelemetry
{
namespace plugin
{
using OpenTelemetryHook =
    nostd::unique_ptr<Factory::FactoryImpl> (*)(const OpenTelemetryLoaderInfo &loader_info,
                                                nostd::unique_ptr<char[]> &error_message) noexcept;
}  // namespace plugin
}  // namespace opentelemetry
