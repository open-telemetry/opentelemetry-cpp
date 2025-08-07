// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

/**
 * ContainerResourceDetector to detect resource attributes when running inside a containerized
 * environment. This detector extracts metadata such as container ID from cgroup information and
 * sets attributes like container.id following the OpenTelemetry semantic conventions.
 */
class ContainerResourceDetector : public ResourceDetector
{
public:
  Resource Detect() noexcept override;
};

/**
 * Reads the container.id from /proc/self/cgroup file.
 * @param file_path file path of cgroup
 * @return container.id as string or empty string
 */
std::string GetContainerIDFromCgroup(const char *file_path);

/**
 * Matches the line with the regex to find container.id
 * @param line a single line of text, typically from the /proc/self/cgroup file
 * @return matched id or empty string
 */
std::string ExtractContainerIDFromLine(std::string_view line);

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
