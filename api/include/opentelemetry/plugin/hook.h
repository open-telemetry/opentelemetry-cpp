#pragma once

#include "opentelemetry/plugin/factory.h"
#include "opentelemetry/plugin/loader_info.h"

#ifdef WIN32

#  define OPENTELEMETRY_DEFINE_PLUGIN_HOOK(X)                                        \
    extern "C" {                                                                      \
    extern __declspec(dllexport)                                                      \
        opentelemetry::plugin::OpenTelemetryHook const OpenTelemetryMakeFactoryImpl;     \
                                                                                      \
    __declspec(selectany)                                                             \
        opentelemetry::plugin::OpenTelemetryHook const OpenTelemetryMakeFactoryImpl = X; \
    }  // extern "C"

#else

#  define OPENTELEMETRY_DEFINE_PLUGIN_HOOK(X)                                                   \
    extern "C" {                                                                                 \
    __attribute(                                                                                 \
        (weak)) extern opentelemetry::plugin::OpenTelemetryHook const OPENTELEMETRY_PLUGIN_HOOK; \
                                                                                                 \
    opentelemetry::plugin::OpenTelemetryHook const OPENTELEMETRY_PLUGIN_HOOK = X;                \
    }  // extern "C"

#endif

namespace opentelemetry
{
namespace plugin
{
using OpenTelemetryHook =
    nostd::unique_ptr<Factory::FactoryImpl> (*)(const LoaderInfo &loader_info,
                                                nostd::unique_ptr<char[]> &error_message) noexcept;
}  // namespace plugin
}  // namespace opentelemetry
