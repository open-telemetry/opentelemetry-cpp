#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/version/version.h"
#include "opentelemetry/version.h"

#include <cstdlib>
#include <memory>
#include <sstream>
#include <unordered_map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

using ResourceAttributes =
    std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue>;

class Resource
{
public:
  Resource(const ResourceAttributes &attributes = ResourceAttributes()) noexcept;

  const ResourceAttributes &GetAttributes() const noexcept;

  std::shared_ptr<Resource> Merge(const Resource &other) noexcept;

  static std::shared_ptr<Resource> Create(const ResourceAttributes &attributes);

  static Resource &GetEmpty();

  static Resource &GetDefault();

private:
  ResourceAttributes attributes_;
};

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE