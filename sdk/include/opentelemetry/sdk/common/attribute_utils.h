// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
/**
 * A counterpart to AttributeValue that makes sure a value is owned. This
 * replaces all non-owning references with owned copies.
 *
 * The following types are not currently supported by the OpenTelemetry
 * specification, but reserved for future use:
 *  - uint64_t
 *  - std::vector<uint64_t>
 *  - std::vector<uint8_t>
 */
using OwnedAttributeValue = nostd::variant<bool,
                                           int32_t,
                                           uint32_t,
                                           int64_t,
                                           double,
                                           std::string,
                                           std::vector<bool>,
                                           std::vector<int32_t>,
                                           std::vector<uint32_t>,
                                           std::vector<int64_t>,
                                           std::vector<double>,
                                           std::vector<std::string>,
                                           uint64_t,
                                           std::vector<uint64_t>,
                                           std::vector<uint8_t>>;

using OwnedAttributeView = nostd::variant<std::vector<nostd::string_view>, std::unique_ptr<bool[]>>;

enum OwnedAttributeType
{
  kTypeBool,
  kTypeInt,
  kTypeUInt,
  kTypeInt64,
  kTypeDouble,
  kTypeString,
  kTypeSpanBool,
  kTypeSpanInt,
  kTypeSpanUInt,
  kTypeSpanInt64,
  kTypeSpanDouble,
  kTypeSpanString,
  kTypeUInt64,
  kTypeSpanUInt64,
  kTypeSpanByte
};

/**
 * Creates an owned copy (OwnedAttributeValue) of a non-owning AttributeValue.
 */
struct AttributeConverter
{
  OwnedAttributeValue operator()(bool v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(int32_t v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(uint32_t v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(int64_t v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(uint64_t v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(double v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(nostd::string_view v)
  {
    return OwnedAttributeValue(std::string(v));
  }
  OwnedAttributeValue operator()(std::string v) { return OwnedAttributeValue(v); }
  OwnedAttributeValue operator()(const char *v) { return OwnedAttributeValue(std::string(v)); }
  OwnedAttributeValue operator()(nostd::span<const uint8_t> v) { return convertSpan<uint8_t>(v); }
  OwnedAttributeValue operator()(nostd::span<const bool> v) { return convertSpan<bool>(v); }
  OwnedAttributeValue operator()(nostd::span<const int32_t> v) { return convertSpan<int32_t>(v); }
  OwnedAttributeValue operator()(nostd::span<const uint32_t> v) { return convertSpan<uint32_t>(v); }
  OwnedAttributeValue operator()(nostd::span<const int64_t> v) { return convertSpan<int64_t>(v); }
  OwnedAttributeValue operator()(nostd::span<const uint64_t> v) { return convertSpan<uint64_t>(v); }
  OwnedAttributeValue operator()(nostd::span<const double> v) { return convertSpan<double>(v); }
  OwnedAttributeValue operator()(nostd::span<const nostd::string_view> v)
  {
    return convertSpan<std::string>(v);
  }

  template <typename T, typename U = T>
  OwnedAttributeValue convertSpan(nostd::span<const U> vals)
  {
    const std::vector<T> copy(vals.begin(), vals.end());
    return OwnedAttributeValue(std::move(copy));
  }
};

/**
 * Evaluates if an owned value (from an OwnedAttributeValue) is equal to another value (from a
 * non-owning AttributeValue). This only supports the checking equality with
 * nostd::visit(AttributeEqualToVisitor, OwnedAttributeValue, AttributeValue).
 */
struct AttributeEqualToVisitor
{
  // Different types are not equal including containers of different element types
  template <typename T, typename U>
  bool operator()(const T &, const U &) const noexcept
  {
    return false;
  }

  // Compare the same arithmetic types
  template <typename T>
  bool operator()(const T &owned_value, const T &value) const noexcept
  {
    return owned_value == value;
  }

  // Compare std::string and const char*
  bool operator()(const std::string &owned_value, const char *value) const noexcept
  {
    return owned_value == value;
  }

  // Compare std::string and nostd::string_view
  bool operator()(const std::string &owned_value, nostd::string_view value) const noexcept
  {
    return owned_value == value;
  }

  // Compare std::vector<std::string> and nostd::span<const nostd::string_view>
  bool operator()(const std::vector<std::string> &owned_value,
                  const nostd::span<const nostd::string_view> &value) const noexcept
  {
    return owned_value.size() == value.size() &&
           std::equal(owned_value.begin(), owned_value.end(), value.begin(),
                      [](const std::string &owned_element, nostd::string_view element) {
                        return owned_element == element;
                      });
  }

  // Compare nostd::span<const T> and std::vector<T> for arithmetic types
  template <typename T>
  bool operator()(const std::vector<T> &owned_value,
                  const nostd::span<const T> &value) const noexcept
  {
    return owned_value.size() == value.size() &&
           std::equal(owned_value.begin(), owned_value.end(), value.begin());
  }
};

/**
 * Class for storing attributes.
 */
class AttributeMap : public std::unordered_map<std::string, OwnedAttributeValue>
{
public:
  // Construct empty attribute map
  AttributeMap() : std::unordered_map<std::string, OwnedAttributeValue>() {}

  // Construct attribute map and populate with attributes
  AttributeMap(const opentelemetry::common::KeyValueIterable &attributes) : AttributeMap()
  {
    attributes.ForEachKeyValue(
        [&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
          SetAttribute(key, value);
          return true;
        });
  }

  // Construct attribute map and populate with optional attributes
  AttributeMap(const opentelemetry::common::KeyValueIterable *attributes) : AttributeMap()
  {
    if (attributes != nullptr)
    {
      attributes->ForEachKeyValue(
          [&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
            SetAttribute(key, value);
            return true;
          });
    }
  }

  // Construct map from initializer list by applying `SetAttribute` transform for every attribute
  AttributeMap(
      std::initializer_list<std::pair<nostd::string_view, opentelemetry::common::AttributeValue>>
          attributes)
      : AttributeMap()
  {
    for (auto &kv : attributes)
    {
      SetAttribute(kv.first, kv.second);
    }
  }

  // Returns a reference to this map
  const std::unordered_map<std::string, OwnedAttributeValue> &GetAttributes() const noexcept
  {
    return (*this);
  }

  // Convert non-owning key-value to owning std::string(key) and OwnedAttributeValue(value)
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept
  {
    (*this)[std::string(key)] = nostd::visit(converter_, value);
  }

  // Compare the attributes of this map with another KeyValueIterable
  bool EqualTo(const opentelemetry::common::KeyValueIterable &attributes) const noexcept
  {
    if (attributes.size() != this->size())
    {
      return false;
    }

    const bool is_equal = attributes.ForEachKeyValue(
        [this](nostd::string_view key,
               const opentelemetry::common::AttributeValue &value) noexcept {
          // Perform a linear search to find the key assuming the map is small
          // This avoids temporary string creation from this->find(std::string(key))
          for (const auto &kv : *this)
          {
            if (kv.first == key)
            {
              // Order of arguments is important here. OwnedAttributeValue is first then
              // AttributeValue AttributeEqualToVisitor does not support the reverse order
              return nostd::visit(equal_to_visitor_, kv.second, value);
            }
          }
          return false;
        });

    return is_equal;
  }

private:
  AttributeConverter converter_;
  AttributeEqualToVisitor equal_to_visitor_;
};

/**
 * Class for storing attributes.
 */
class OrderedAttributeMap : public std::map<std::string, OwnedAttributeValue>
{
public:
  // Contruct empty attribute map
  OrderedAttributeMap() : std::map<std::string, OwnedAttributeValue>() {}

  // Contruct attribute map and populate with attributes
  OrderedAttributeMap(const opentelemetry::common::KeyValueIterable &attributes)
      : OrderedAttributeMap()
  {
    attributes.ForEachKeyValue(
        [&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
          SetAttribute(key, value);
          return true;
        });
  }

  // Construct map from initializer list by applying `SetAttribute` transform for every attribute
  OrderedAttributeMap(
      std::initializer_list<std::pair<nostd::string_view, opentelemetry::common::AttributeValue>>
          attributes)
      : OrderedAttributeMap()
  {
    for (auto &kv : attributes)
    {
      SetAttribute(kv.first, kv.second);
    }
  }

  // Returns a reference to this map
  const std::map<std::string, OwnedAttributeValue> &GetAttributes() const noexcept
  {
    return (*this);
  }

  // Convert non-owning key-value to owning std::string(key) and OwnedAttributeValue(value)
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept
  {
    (*this)[std::string(key)] = nostd::visit(converter_, value);
  }

private:
  AttributeConverter converter_;
};

/**
 * Class for storing attributes.
 */
struct MixedAttributeMapStorage
{
  std::unordered_map<std::string, opentelemetry::common::AttributeValue> attributes;
  AttributeMap owned_attributes;
  std::unordered_map<std::string, OwnedAttributeView> owened_attributes_view;
};

/**
 * Set an owned copy (OwnedAttributeValue) and attribute view of a non-owning AttributeValue.
 */
class MixedAttributeViewSetter
{
public:
  inline MixedAttributeViewSetter(const nostd::string_view &key,
                                  MixedAttributeMapStorage &storage,
                                  AttributeConverter &converter) noexcept
      : key_(&key), storage_(&storage), converter_(&converter)
  {}

  void operator()(bool v)
  {
    storage_->owned_attributes[std::string(*key_)] = (*converter_)(v);
    storage_->attributes[std::string(*key_)]       = v;
  }

  void operator()(int32_t v)
  {
    storage_->owned_attributes[std::string(*key_)] = (*converter_)(v);
    storage_->attributes[std::string(*key_)]       = v;
  }

  void operator()(uint32_t v)
  {
    storage_->owned_attributes[std::string(*key_)] = (*converter_)(v);
    storage_->attributes[std::string(*key_)]       = v;
  }

  void operator()(int64_t v)
  {
    storage_->owned_attributes[std::string(*key_)] = (*converter_)(v);
    storage_->attributes[std::string(*key_)]       = v;
  }

  void operator()(uint64_t v)
  {
    storage_->owned_attributes[std::string(*key_)] = (*converter_)(v);
    storage_->attributes[std::string(*key_)]       = v;
  }

  void operator()(double v)
  {
    storage_->owned_attributes[std::string(*key_)] = (*converter_)(v);
    storage_->attributes[std::string(*key_)]       = v;
  }

  void operator()(nostd::string_view v)
  {
    auto &owned_value                        = storage_->owned_attributes[std::string(*key_)];
    owned_value                              = (*converter_)(v);
    storage_->attributes[std::string(*key_)] = nostd::get<std::string>(owned_value);
  }

  void operator()(const char *v)
  {
    auto &owned_value                        = storage_->owned_attributes[std::string(*key_)];
    owned_value                              = (*converter_)(v);
    storage_->attributes[std::string(*key_)] = nostd::get<std::string>(owned_value).c_str();
  }

  void operator()(nostd::span<const uint8_t> v)
  {
    auto &owned_value                        = storage_->owned_attributes[std::string(*key_)];
    owned_value                              = (*converter_)(v);
    storage_->attributes[std::string(*key_)] = nostd::get<std::vector<uint8_t>>(owned_value);
  }

  void operator()(nostd::span<const bool> v)
  {
    storage_->owned_attributes[std::string(*key_)] = (*converter_)(v);
    if (v.empty())
    {
      storage_->attributes[std::string(*key_)] = nostd::span<const bool>{};
    }
    else
    {
      std::unique_ptr<bool[]> owned_view{new bool[v.size()]};
      for (size_t i = 0; i < v.size(); i++)
      {
        owned_view[i] = v[i];
      }

      storage_->attributes[std::string(*key_)] =
          nostd::span<const bool>{owned_view.get(), v.size()};
      storage_->owened_attributes_view[std::string(*key_)] = std::move(owned_view);
    }
  }

  void operator()(nostd::span<const int32_t> v)
  {
    auto &owned_value                        = storage_->owned_attributes[std::string(*key_)];
    owned_value                              = (*converter_)(v);
    storage_->attributes[std::string(*key_)] = nostd::get<std::vector<int32_t>>(owned_value);
  }

  void operator()(nostd::span<const uint32_t> v)
  {
    auto &owned_value                        = storage_->owned_attributes[std::string(*key_)];
    owned_value                              = (*converter_)(v);
    storage_->attributes[std::string(*key_)] = nostd::get<std::vector<uint32_t>>(owned_value);
  }

  void operator()(nostd::span<const int64_t> v)
  {
    auto &owned_value                        = storage_->owned_attributes[std::string(*key_)];
    owned_value                              = (*converter_)(v);
    storage_->attributes[std::string(*key_)] = nostd::get<std::vector<int64_t>>(owned_value);
  }

  void operator()(nostd::span<const uint64_t> v)
  {
    auto &owned_value                        = storage_->owned_attributes[std::string(*key_)];
    owned_value                              = (*converter_)(v);
    storage_->attributes[std::string(*key_)] = nostd::get<std::vector<uint64_t>>(owned_value);
  }

  void operator()(nostd::span<const double> v)
  {
    auto &owned_value                        = storage_->owned_attributes[std::string(*key_)];
    owned_value                              = (*converter_)(v);
    storage_->attributes[std::string(*key_)] = nostd::get<std::vector<double>>(owned_value);
  }

  void operator()(nostd::span<const nostd::string_view> v)
  {
    auto &owned_value = storage_->owned_attributes[std::string(*key_)];
    owned_value       = (*converter_)(v);

    if (v.empty())
    {
      storage_->attributes[std::string(*key_)] = nostd::span<const nostd::string_view>{};
    }
    else
    {
      auto &owned_view   = storage_->owened_attributes_view[std::string(*key_)];
      owned_view         = std::vector<nostd::string_view>{};
      auto &owned_vector = nostd::get<std::vector<nostd::string_view>>(owned_view);

      owned_vector.reserve(v.size());
      for (auto &data : nostd::get<std::vector<std::string>>(owned_value))
      {
        owned_vector.push_back(data);
      }

      storage_->attributes[std::string(*key_)] =
          nostd::span<const nostd::string_view>{owned_vector.data(), owned_vector.size()};
    }
  }

private:
  const nostd::string_view *key_;
  MixedAttributeMapStorage *storage_;
  AttributeConverter *converter_;
};

/**
 * Class for storing attributes and attribute view.
 */
class MixedAttributeMap
{
public:
  // Construct empty attribute map
  MixedAttributeMap() {}

  // Construct attribute map and populate with attributes
  MixedAttributeMap(const opentelemetry::common::KeyValueIterable &attributes) : MixedAttributeMap()
  {
    attributes.ForEachKeyValue(
        [&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
          nostd::visit(MixedAttributeViewSetter(key, storage_, converter_), value);
          return true;
        });
  }

  // Construct attribute map and populate with optional attributes
  MixedAttributeMap(const opentelemetry::common::KeyValueIterable *attributes) : MixedAttributeMap()
  {
    if (attributes != nullptr)
    {
      attributes->ForEachKeyValue(
          [&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
            nostd::visit(MixedAttributeViewSetter(key, storage_, converter_), value);
            return true;
          });
    }
  }

  // Construct map from initializer list by applying `SetAttribute` transform for every attribute
  MixedAttributeMap(
      std::initializer_list<std::pair<nostd::string_view, opentelemetry::common::AttributeValue>>
          attributes)
      : MixedAttributeMap()
  {
    for (auto &kv : attributes)
    {
      nostd::visit(MixedAttributeViewSetter(kv.first, storage_, converter_), kv.second);
    }
  }

  // Returns a reference to this map
  const std::unordered_map<std::string, opentelemetry::common::AttributeValue> &GetAttributes()
      const noexcept
  {
    return storage_.attributes;
  }

  const AttributeMap &GetOwnedAttributes() const noexcept { return storage_.owned_attributes; }

  // Convert non-owning key-value to owning std::string(key) and OwnedAttributeValue(value)
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept
  {
    nostd::visit(MixedAttributeViewSetter(key, storage_, converter_), value);
  }

  void Reserve(AttributeMap::size_type size)
  {
    storage_.attributes.reserve(size);
    storage_.owned_attributes.reserve(size);
  }

  AttributeMap::size_type Size() const noexcept { return storage_.attributes.size(); }

  bool Empty() const noexcept { return storage_.attributes.empty(); }

private:
  MixedAttributeMapStorage storage_;
  AttributeConverter converter_;
};

}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
