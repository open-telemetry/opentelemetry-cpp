// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/resource_detectors/process_detector.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/resource_detectors/detail/process_detector_utils.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/semconv/incubating/process_attributes.h"
#include "opentelemetry/version.h"

#include <cstdint>
#include <exception>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#ifdef _MSC_VER
#  include <process.h>
#  define getpid _getpid
#else
#  include <unistd.h>
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{

opentelemetry::sdk::resource::Resource ProcessResourceDetector::Detect() noexcept
{
  int32_t pid = getpid();
  opentelemetry::sdk::resource::ResourceAttributes attributes;
  attributes[semconv::process::kProcessPid] = pid;

  try
  {
    std::string executable_path = opentelemetry::resource_detector::detail::GetExecutablePath(pid);
    if (!executable_path.empty())
    {
      attributes[semconv::process::kProcessExecutablePath] = executable_path;

      // process.executable.name is the basename of the executable path.
      std::size_t sep = executable_path.find_last_of("/\\");
      std::string executable_name = (sep == std::string::npos)
                                        ? executable_path
                                        : executable_path.substr(sep + 1);
      if (!executable_name.empty())
      {
        attributes[semconv::process::kProcessExecutableName] = std::move(executable_name);
      }
    }
  }
  catch (const ::std::exception &ex)
  {
    OTEL_INTERNAL_LOG_ERROR("[Process Resource Detector] "
                            << "Error extracting the executable path: " << ex.what());
  }

  try
  {
    std::vector<std::string> command_with_args =
        opentelemetry::resource_detector::detail::GetCommandWithArgs(pid);
    if (!command_with_args.empty())
    {
      // Commented until they are properly sanitized
      // attributes[semconv::process::kProcessCommand]     = command_with_args[0];
      // attributes[semconv::process::kProcessCommandArgs] = std::move(command_with_args);
    }
  }
  catch (const std::exception &ex)
  {
    OTEL_INTERNAL_LOG_ERROR("[Process Resource Detector] "
                            << "Error extracting command with arguments: " << ex.what());
  }

  try
  {
    std::string creation_time =
        opentelemetry::resource_detector::detail::GetProcessCreationTime(pid);
    if (!creation_time.empty())
    {
      attributes[semconv::process::kProcessCreationTime] = std::move(creation_time);
    }
  }
  catch (const std::exception &ex)
  {
    OTEL_INTERNAL_LOG_ERROR("[Process Resource Detector] "
                            << "Error extracting process creation time: " << ex.what());
  }

  try
  {
    std::string owner = opentelemetry::resource_detector::detail::GetProcessOwner(pid);
    if (!owner.empty())
    {
      attributes[semconv::process::kProcessOwner] = std::move(owner);
    }
  }
  catch (const std::exception &ex)
  {
    OTEL_INTERNAL_LOG_ERROR("[Process Resource Detector] "
                            << "Error extracting process owner: " << ex.what());
  }

  try
  {
    std::string build_id =
        opentelemetry::resource_detector::detail::GetExecutableBuildIdHtlhash(pid);
    if (!build_id.empty())
    {
      attributes[semconv::process::kProcessExecutableBuildIdHtlhash] = std::move(build_id);
    }
  }
  catch (const std::exception &ex)
  {
    OTEL_INTERNAL_LOG_ERROR("[Process Resource Detector] "
                            << "Error computing executable build id (htlhash): " << ex.what());
  }

  return ResourceDetector::Create(attributes);
}

}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
