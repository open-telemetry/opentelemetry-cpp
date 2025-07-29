// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/sdk/common/container.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/semconv/service_attributes.h"
#include "opentelemetry/semconv/incubating/container_attributes.h"
#include "opentelemetry/version.h"

#include <stddef.h>
#include <sstream>
#include <string>
#include <unordered_map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

const char *OTEL_RESOURCE_ATTRIBUTES = "OTEL_RESOURCE_ATTRIBUTES";
const char *OTEL_SERVICE_NAME        = "OTEL_SERVICE_NAME";
/**
 * This is the file path from where we can get container.id
*/
const char *C_GROUP_PATH = "/proc/self/cgroup";

Resource ResourceDetector::Create(const ResourceAttributes &attributes,
                                  const std::string &schema_url)
{
  return Resource(attributes, schema_url);
}

Resource OTELResourceDetector::Detect() noexcept
{
  std::string attributes_str, service_name;

  bool attributes_exists = opentelemetry::sdk::common::GetStringEnvironmentVariable(
      OTEL_RESOURCE_ATTRIBUTES, attributes_str);
  bool service_name_exists =
      opentelemetry::sdk::common::GetStringEnvironmentVariable(OTEL_SERVICE_NAME, service_name);

  if (!attributes_exists && !service_name_exists)
  {
    return ResourceDetector::Create({});
  }

  ResourceAttributes attributes;

  if (attributes_exists)
  {
    std::istringstream iss(attributes_str);
    std::string token;
    while (std::getline(iss, token, ','))
    {
      size_t pos = token.find('=');
      if (pos != std::string::npos)
      {
        std::string key   = token.substr(0, pos);
        std::string value = token.substr(pos + 1);
        attributes[key]   = value;
      }
    }
  }

  if (service_name_exists)
  {
    attributes[semconv::service::kServiceName] = service_name;
  }

  return ResourceDetector::Create(attributes);
}

Resource ContainerResourceDetector::Detect() noexcept 
{
  std::string container_id = opentelemetry::sdk::common::GetContainerIDFromCgroup(C_GROUP_PATH);
  if(container_id.empty())
  {
    return ResourceDetector::Create({});
  }

  ResourceAttributes attributes;

  if(!container_id.empty())
  {
    attributes[semconv::container::kContainerId] = container_id;
  }
  return ResourceDetector::Create(attributes);
}

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
