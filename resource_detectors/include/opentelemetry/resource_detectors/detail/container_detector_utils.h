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
 * Reads the container.id from /proc/self/cgroup file.
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
 * Reads the container.id from container runtime bind mount paths in /proc/self/mountinfo.
 * Used as a fallback when /proc/self/cgroup does not contain the id (e.g. cgroup v2 with a
 * private cgroup namespace). When several distinct ids are present (Kubernetes pod sandbox and
 * application container) the last one found is returned and a warning is logged.
 * @param file_path file path of mountinfo
 * @return container.id as string or an empty string if not found on error
 */
std::string GetContainerIDFromMountInfo(const char *file_path);

/**
 * Extracts container.id from a mountinfo line whose mount root contains "/containers/"
 * followed by a 64 character hexadecimal id.
 * @param line a single line of text, typically from the /proc/self/mountinfo file
 * @return matched id or empty string
 */
std::string ExtractContainerIDFromMountInfoLine(nostd::string_view line);

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
