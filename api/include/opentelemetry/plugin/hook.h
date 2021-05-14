// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "opentelemetry/plugin/detail/loader_info.h"
#include "opentelemetry/plugin/factory.h"
#include "opentelemetry/version.h"

#ifdef _WIN32

/**
 * Cross-platform helper macro to declare the symbol used to load an OpenTelemetry implementation
 * as a plugin.
 *
 * Note: The symbols use weak linkage so as to support using an OpenTelemetry both as a regular
 * library and a dynamically loaded plugin. The weak linkage allows for multiple implementations to
 * be linked in without getting multiple definition errors.
 */
#  define OPENTELEMETRY_DEFINE_PLUGIN_HOOK(X)                                            \
    extern "C" {                                                                         \
    extern __declspec(dllexport)                                                         \
        opentelemetry::plugin::OpenTelemetryHook const OpenTelemetryMakeFactoryImpl;     \
                                                                                         \
    __declspec(selectany)                                                                \
        opentelemetry::plugin::OpenTelemetryHook const OpenTelemetryMakeFactoryImpl = X; \
    }  // extern "C"

#else

#  define OPENTELEMETRY_DEFINE_PLUGIN_HOOK(X)                                                      \
    extern "C" {                                                                                   \
    __attribute((                                                                                  \
        weak)) extern opentelemetry::plugin::OpenTelemetryHook const OpenTelemetryMakeFactoryImpl; \
                                                                                                   \
    opentelemetry::plugin::OpenTelemetryHook const OpenTelemetryMakeFactoryImpl = X;               \
    }  // extern "C"

#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace plugin
{
using OpenTelemetryHook =
    nostd::unique_ptr<Factory::FactoryImpl> (*)(const LoaderInfo &loader_info,
                                                nostd::unique_ptr<char[]> &error_message);
}  // namespace plugin
OPENTELEMETRY_END_NAMESPACE
