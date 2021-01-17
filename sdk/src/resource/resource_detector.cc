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
  char *attributes_str = std::getenv(OTEL_RESOURCE_ATTRIBUTES);
  if (attributes_str == nullptr)
    return Resource();
  // return Resource::GetEmpty();
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
