#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

const std::string TELEMETRY_SDK_LANGUAGE = "telemetry.sdk.language";
const std::string TELEMETRY_SDK_NAME     = "telemetry.sdk.name";
const std::string TELEMETRY_SDK_VERSION  = "telemetry.sdk.version";

Resource::Resource(const ResourceAttributes &attributes) noexcept : attributes_(attributes) {}

std::shared_ptr<Resource> Resource::Merge(const Resource &other)
{
  ResourceAttributes merged_resource_attributes(attributes_);
  for (auto &elem : other.attributes_)
  {
    if ((merged_resource_attributes.find(elem.first) == merged_resource_attributes.end()) ||
        (nostd::holds_alternative<std::string>(attributes_[elem.first]) &&
         nostd::get<std::string>(attributes_[elem.first]).size() == 0))
    {
      merged_resource_attributes[elem.first] = elem.second;
    }
  }
  return std::make_shared<Resource>(Resource(merged_resource_attributes));
}

std::shared_ptr<Resource> Resource::Create(const ResourceAttributes &attributes)
{
  auto default_resource = Resource::GetDefault();

  if (attributes.size() > 0)
  {
    Resource tmp_resource(attributes);
    auto merged_resource = tmp_resource.Merge(default_resource);
    return merged_resource->Merge(*(OTELResourceDetector().Detect()));
  }
  return default_resource.Merge(*(OTELResourceDetector().Detect()));
}

Resource &Resource::GetEmpty()
{
  static Resource empty;
  return empty;
}

Resource &Resource::GetDefault()
{
  static Resource default_resource({{TELEMETRY_SDK_LANGUAGE, "cpp"},
                                    {TELEMETRY_SDK_NAME, "opentelemetry"},
                                    {TELEMETRY_SDK_VERSION, OPENTELEMETRY_SDK_VERSION}});
  return default_resource;
}

const ResourceAttributes &Resource::GetAttributes() const noexcept
{
  return attributes_;
}

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE