// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/resource_detectors/container_resource_detector.h"

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
  return std::string();
}

std::string ExtractContainerIDFromLine(nostd::string_view line)
{
  /**
   * This regex is designed to extract container IDs from cgroup file lines.
   * It matches hexadecimal container IDs used by container runtimes like Docker, containerd, and
   * cri-o.
   * Examples of matching lines:
   * - 0::/docker/3fae9b2c6d7e8f90123456789abcdef0123456789abcdef0123456789abcdef0
   * - "13:name=systemd:/podruntime/docker/kubepods/ac679f8a8319c8cf7d38e1adf263bc08d23.aaaa"
   * - "e857a4bf05a69080a759574949d7a0e69572e27647800fa7faff6a05a8332aa1"
   * Please see the test cases in resource_test.cc for more examples.
   */
  static const std::regex container_id_regex(R"(^.*/(?:.*[-:])?([0-9a-f]+)(?:\.|\s*$))");
  std::match_results<std::string_view::const_iterator> match;

  if (std::regex_search(line.begin(), line.end(), match, container_id_regex))
  {
    return match.str(1);
  }

  return std::string();
}
}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
