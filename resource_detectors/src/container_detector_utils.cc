// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/resource_detectors/detail/container_detector_utils.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/global_log_handler.h"

#include <algorithm>
#include <fstream>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{
namespace detail
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
  std::match_results<const char *> match;

  if (std::regex_search(line.data(), line.data() + line.size(), match, container_id_regex))
  {
    return match.str(1);
  }

  return std::string();
}

std::string GetContainerIDFromMountInfo(const char *file_path)
{
  std::ifstream mountinfo_file(file_path);
  std::string line;
  std::vector<std::string> container_ids;

  // In Kubernetes the pod sandbox container's files are mounted first (e.g. /etc/hostname), and
  // the application container's own mounts (e.g. /run/secrets) come later, so the last id wins.
  while (std::getline(mountinfo_file, line))
  {
    std::string id = ExtractContainerIDFromMountInfoLine(line);
    if (!id.empty() &&
        std::find(container_ids.begin(), container_ids.end(), id) == container_ids.end())
    {
      container_ids.push_back(std::move(id));
    }
  }

  if (container_ids.empty())
  {
    return std::string();
  }

  if (container_ids.size() > 1)
  {
    OTEL_INTERNAL_LOG_WARN("[Container Resource Detector] Multiple container ids found in "
                           << file_path << ", using the last one: " << container_ids.back());
  }

  return container_ids.back();
}

std::string ExtractContainerIDFromMountInfoLine(nostd::string_view line)
{
  /**
   * This regex is designed to extract container IDs from /proc/self/mountinfo file lines.
   * When /proc/self/cgroup does not contain the id (e.g. cgroup v2 with a private cgroup
   * namespace) the container id is still present in the source paths of files the container
   * runtime bind mounts into the container. The mountinfo fields are:
   *   <mount id> <parent id> <major:minor> <root> <mount point> <options> ...
   * and <root> must contain "/containers/" followed by a 64 character hex id. Examples:
   * - docker: /docker/containers/<id>/hostname /etc/hostname
   * - podman: /containers/overlay-containers/<id>/userdata/resolv.conf /etc/resolv.conf
   * - cri-o:  /containers/storage/overlay-containers/<id>/userdata/run/secrets /run/secrets
   */
  static const std::regex container_id_regex(
      R"(^\d+\s+\d+\s+\d+:\d+\s+\S*/containers/(?:\S*?/)?([0-9a-f]{64})(?:/|\s|$))");
  std::match_results<const char *> match;

  if (std::regex_search(line.data(), line.data() + line.size(), match, container_id_regex))
  {
    return match.str(1);
  }

  return std::string();
}

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
