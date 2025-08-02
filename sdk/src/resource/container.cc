// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/resource/container_resource_detector.h"

#include <fstream>
#include <regex>
#include <string>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{
std::string GetContainerIDFromCgroup(const char *file_path)
{
  std::ifstream cgroup_file(file_path);
  std::string line;

  while (std::getline(cgroup_file, line))
  {
    std::string container_id = ExtractContainerIDFromLine(line);
    if (!container_id.empty())
    {
      return container_id;
    }
  }
  return "";
}

std::string ExtractContainerIDFromLine(const std::string &line)
{
  static const std::regex container_id_regex(R"(^.*/(?:.*[-:])?([0-9a-f]+)(?:\.|\s*$))");
  std::smatch match;

  if (std::regex_search(line, match, container_id_regex))
  {
    return match.str(1);
  }

  return "";
}
}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
