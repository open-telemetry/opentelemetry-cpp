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

#include <string>

#ifdef _WIN32
#  include "opentelemetry/plugin/detail/dynamic_load_windows.h"
#else
#  include "opentelemetry/plugin/detail/dynamic_load_unix.h"
#endif

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace plugin
{
/**
 * Load an OpenTelemetry implementation as a plugin.
 * @param plugin the path to the plugin to load
 * @param error_message on failure this is set to an error message
 * @return a Factory that can be used to create OpenTelemetry objects or nullptr on failure.
 */
std::unique_ptr<Factory> LoadFactory(const char *plugin, std::string &error_message) noexcept;
}  // namespace plugin
OPENTELEMETRY_END_NAMESPACE
