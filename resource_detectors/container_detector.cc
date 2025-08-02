// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/resource_detectors/container_resource_detector.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/semconv/incubating/container_attributes.h"
#include "opentelemetry/version.h"

#include <string>
#include <unordered_map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

/**
 * This is the file path from where we can get container.id
 */
const char *KCGroupPath = "/proc/self/cgroup";

Resource ContainerResourceDetector::Detect() noexcept
{
  std::string container_id = opentelemetry::sdk::resource::GetContainerIDFromCgroup(KCGroupPath);
  if (container_id.empty())
  {
    return ResourceDetector::Create({});
  }

  ResourceAttributes attributes;

  attributes[semconv::container::kContainerId] = container_id;
  return ResourceDetector::Create(attributes);
}

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
