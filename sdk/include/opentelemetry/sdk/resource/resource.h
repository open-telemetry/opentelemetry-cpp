#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/sdk/trace/attribute_utils.h"
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

class Resource
{
public:
  Resource(const opentelemetry::common::KeyValueIterable &attributes) noexcept;

  Resource(const opentelemetry::sdk::trace::AttributeMap &attributes) noexcept;

  Resource(opentelemetry::sdk::trace::AttributeMap &&attributes) noexcept;

  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  Resource(const T &attributes) noexcept : Resource(common::KeyValueIterableView<T>(attributes))
  {}

  Resource(const std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
               attributes) noexcept
      : Resource(nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
            attributes.begin(), attributes.end()})
  {}

  const opentelemetry::sdk::trace::AttributeMap &GetAttributes() const noexcept;

  std::unique_ptr<Resource> Merge(const Resource &other);

  static std::unique_ptr<Resource> Create(
      const opentelemetry::common::KeyValueIterable &attributes);

  static std::unique_ptr<Resource> Create(
      std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
          attributes) noexcept
  {
    return Create(nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
        attributes.begin(), attributes.end()});
  }

  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  static std::unique_ptr<Resource> Create(const T &attributes) noexcept
  {
    return Create(common::KeyValueIterableView<T>(attributes));
  }

  static std::unique_ptr<Resource> CreateEmpty();

  static std::unique_ptr<Resource> Create(const std::string &attributes);

private:
  const opentelemetry::sdk::trace::AttributeMap attribute_map_;
};

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE