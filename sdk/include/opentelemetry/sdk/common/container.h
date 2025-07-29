// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
/**
  Reads the container.id from /proc/self/cgroup file.
  @param file_path file path of cgroup
  @return container.id as string or empty string
*/
std::string GetContainerIDFromCgroup(const char *file_path);

/**
  Matches the line with the regex to find container.id
  @param line contains 
  @return matched id or empty string
*/
std::string ExtractContainerIDFromLine(std::string &line);
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE