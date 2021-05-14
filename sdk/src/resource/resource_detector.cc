// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
