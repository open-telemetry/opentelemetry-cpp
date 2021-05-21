// Copyright (c) 2021 OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/resource/resource_detector.h"
#include <cstdlib>
#include "opentelemetry/sdk/resource/resource.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

const char *OTEL_RESOURCE_ATTRIBUTES = "OTEL_RESOURCE_ATTRIBUTES";

Resource OTELResourceDetector::Detect() noexcept
{
#if defined(_MSC_VER)
  size_t required_size = 0;
  getenv_s(&required_size, nullptr, 0, OTEL_RESOURCE_ATTRIBUTES);
  if (required_size == 0)
    return Resource();
  std::unique_ptr<char> attributes_buffer{new char[required_size]};
  getenv_s(&required_size, attributes_buffer.get(), required_size, OTEL_RESOURCE_ATTRIBUTES);
  char *attributes_str = attributes_buffer.get();
#else
  char *attributes_str = std::getenv(OTEL_RESOURCE_ATTRIBUTES);
  if (attributes_str == nullptr)
    return Resource();
#endif

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
