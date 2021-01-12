#pragma once

#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

class Resource;
class ResourceDetector
{
public:
  virtual std::shared_ptr<Resource> Detect() = 0;
};

class OTELResourceDetector : public ResourceDetector
{
public:
  std::shared_ptr<Resource> Detect() noexcept override;
};

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE