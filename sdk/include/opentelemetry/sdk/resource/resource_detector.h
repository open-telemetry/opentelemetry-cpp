#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/sdk/resource/resource.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

class ResourceDetector
{
public:
    virtual std::unique_ptr<opentelemetry::sdk::resource::Resource> Detect() = 0; 
};

class OTELResourceDetector : public ResourceDetector
{
public:
    std::unique_ptr<opentelemetry::sdk::resource::Resource> Detect() noexcept override ;

};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE