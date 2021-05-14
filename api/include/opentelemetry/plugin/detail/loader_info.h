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

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace plugin
{
/**
 * LoaderInfo describes the versioning of the loader.
 *
 * Plugins can check against this information and properly error out if they were built against an
 * incompatible OpenTelemetry API.
 */
struct LoaderInfo
{
  nostd::string_view opentelemetry_version     = OPENTELEMETRY_VERSION;
  nostd::string_view opentelemetry_abi_version = OPENTELEMETRY_ABI_VERSION;
};
}  // namespace plugin
OPENTELEMETRY_END_NAMESPACE
