#include "opentelemetry/sdk/resource/resource_detector.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

    const char *OTEL_RESOURCE_ATTRIBUTES = "OTEL_RESOURCE_ATTRIBUTES";


    std::unique_ptr<Resource> OTELResourceDetector::Detect() noexcept  
    {
        char *resource_attributes_str = std::getenv(OTEL_RESOURCE_ATTRIBUTES);
        if (resource_attributes_str == nullptr)
            return std::unique_ptr<Resource>(new Resource({}));
        return Resource::Create(std::string(resource_attributes_str));
    }

}
}
OPENTELEMETRY_END_NAMESPACE
