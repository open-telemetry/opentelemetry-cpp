#include "opentelemetry/version.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/nostd/span.h"


OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

const std::string TELEMETRY_SDK_LANGUAGE = "telemetry.sdk.language" ;
const std::string TELEMETRY_SDK_NAME = "telemetry.sdk.name" ;
const std::string TELEMETRY_SDK_VERSION = "telemetry.sdk.version" ;

Resource::Resource(const opentelemetry::common::KeyValueIterable &attributes) noexcept : attribute_map_(attributes){ }

Resource::Resource(const opentelemetry::sdk::trace::AttributeMap &attributes) noexcept : attribute_map_(attributes){ }

Resource::Resource(opentelemetry::sdk::trace::AttributeMap &&attributes) noexcept : attribute_map_(std::move(attributes)){ }




const opentelemetry::sdk::trace::AttributeMap& Resource::GetAttributes() const noexcept
{
    return attribute_map_;
}

std::unique_ptr<Resource> Resource::Merge(const Resource &other)
{
    opentelemetry::sdk::trace::AttributeMap merged_resource_attributes(other.GetAttributes());
    merged_resource_attributes.AddAttributes(attribute_map_);
    return std::unique_ptr<Resource>(new Resource(merged_resource_attributes));
}

std::unique_ptr<Resource> Resource::Create(const opentelemetry::common::KeyValueIterable &attributes)
{
    Resource default_resource ( {{
        { TELEMETRY_SDK_LANGUAGE, "cpp"},
        { TELEMETRY_SDK_NAME, "opentelemetry"},
        { TELEMETRY_SDK_VERSION, OPENTELEMETRY_SDK_VERSION}
    } });
    
    if (attributes.size() > 0) {
        Resource input_resource(attributes);
        auto merged_resource = default_resource.Merge(input_resource);
        return merged_resource->Merge(*(OTELResourceDetector().Detect()));
    }
    return default_resource.Merge(*(OTELResourceDetector().Detect()));
}

std::unique_ptr<Resource> Resource::CreateEmpty()
{
    return Create({});
}

std::unique_ptr<Resource> Resource::Create(const std::string &attributes)
{
    opentelemetry::sdk::trace::AttributeMap resource_attributes;
    std::istringstream iss(attributes);
    std::string token;
    while (std::getline(iss, token, ',' ))
    {
        size_t pos = token.find('=');
        std::string key = token.substr(0, pos);
        std::string value = token.substr(pos + 1);
        resource_attributes.SetAttribute(key, value);
    }
    return std::unique_ptr<Resource>(new Resource(resource_attributes));
}

}
}
OPENTELEMETRY_END_NAMESPACE