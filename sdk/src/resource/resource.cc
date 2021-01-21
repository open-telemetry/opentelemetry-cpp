#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

const std::string kTelemetrySdkLanguage = "telemetry.sdk.language";
const std::string kTelemetrySdkName     = "telemetry.sdk.name";
const std::string kTelemetrySdkVersion  = "telemetry.sdk.version";

Resource::Resource(const ResourceAttributes &attributes) noexcept : attributes_(attributes) {}

Resource Resource::Merge(const Resource &other) noexcept
{
  ResourceAttributes merged_resource_attributes(other.attributes_);
  for (auto &elem : attributes_)
  {
    if (merged_resource_attributes.find(elem.first) == merged_resource_attributes.end())
    {
      merged_resource_attributes[elem.first] = elem.second;
    }
  }
  return Resource(merged_resource_attributes);
}

Resource Resource::Create(const ResourceAttributes &attributes)
{
  static auto otel_resource = OTELResourceDetector().Detect();
  auto default_resource     = Resource::GetDefault();

  if (attributes.size() > 0)
  {
    Resource tmp_resource(attributes);
    auto merged_resource = tmp_resource.Merge(default_resource);
    return merged_resource.Merge(otel_resource);
  }
  return default_resource.Merge(otel_resource);
}

Resource &Resource::GetEmpty()
{
  static Resource empty_resource;
  return empty_resource;
}

Resource &Resource::GetDefault()
{
  static Resource default_resource({{kTelemetrySdkLanguage, "cpp"},
                                    {kTelemetrySdkName, "opentelemetry"},
                                    {kTelemetrySdkVersion, OPENTELEMETRY_SDK_VERSION}});
  return default_resource;
}

const ResourceAttributes &Resource::GetAttributes() const noexcept
{
  return attributes_;
}

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE