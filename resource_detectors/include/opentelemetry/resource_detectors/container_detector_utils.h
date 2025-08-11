// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/version.h"
#include "opentelemetry/nostd/string_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{
namespace detail
{

/**
 * Reads the container.id from /proc/self/cgroup file.
 * @param file_path file path of cgroup
 * @return container.id as string or an empty string if not found on error
 */
std::string OPENTELEMETRY_EXPORT GetContainerIDFromCgroup(const char *file_path);

/**
 * Matches the line with the regex to find container.id
 * @param line a single line of text, typically from the /proc/self/cgroup file
 * @return matched id or empty string
 */
std::string OPENTELEMETRY_EXPORT ExtractContainerIDFromLine(nostd::string_view line);

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
