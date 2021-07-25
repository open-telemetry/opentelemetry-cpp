// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/sdk/resource/resource.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

const char *OTEL_RESOURCE_ATTRIBUTES = "OTEL_RESOURCE_ATTRIBUTES";

Resource OTELResourceDetector::Detect() noexcept
{
  auto attributes_str =
      opentelemetry::sdk::common::GetEnvironmentVariable(OTEL_RESOURCE_ATTRIBUTES);
  if (attributes_str.size() == 0)
  {
    return Resource();
  }
  ResourceAttributes attributes;
  std::istringstream iss(attributes_str);
  std::string token;
  while (std::getline(iss, token, ','))
  {
    size_t pos        = token.find('=');
    std::string key   = token.substr(0, pos);
    std::string value = token.substr(pos + 1);
    attributes[key]   = value;
  }
  return Resource(attributes);
}

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
