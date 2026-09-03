// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{
namespace detail
{

/**
 * Reads the container.id from /proc/self/cgroup file (cgroup v1).
 * @param file_path file path of cgroup
 * @return container.id as string or an empty string if not found on error
 */
std::string GetContainerIDFromCgroup(const char *file_path);

/**
 * Matches the line with the regex to find container.id
 * @param line a single line of text, typically from the /proc/self/cgroup file
 * @return matched id or empty string
 */
std::string ExtractContainerIDFromLine(nostd::string_view line);

/**
 * Reads the container.id from /proc/self/mountinfo file (cgroup v2).
 * @param file_path file path of mountinfo
 * @return container.id as string or an empty string if not found on error
 */
std::string GetContainerIDFromMountInfo(const char *file_path);

/**
 * Extracts container.id from a mountinfo line by looking for a 64 character hexadecimal
 * path segment on lines that reference "containers" or "hostname".
 * @param line a single line of text, typically from the /proc/self/mountinfo file
 * @return matched id or empty string
 */
std::string ExtractContainerIDFromMountInfoLine(nostd::string_view line);

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
