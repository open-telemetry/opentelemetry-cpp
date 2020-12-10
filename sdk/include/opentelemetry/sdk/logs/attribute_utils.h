#pragma once

#include <unordered_map>
#include <vector>
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
/**
 * A counterpart to AttributeValue that makes sure a value is owned. This
 * replaces all non-owning references with owned copies.
 */
using LogRecordAttributeValue = nostd::variant<bool,
                                              int32_t,
                                              uint32_t,
                                              int64_t,
                                              uint64_t,
                                              double,
                                              std::string,
                                              std::vector<bool>,
                                              std::vector<int32_t>,
                                              std::vector<uint32_t>,
                                              std::vector<int64_t>,
                                              std::vector<uint64_t>,
                                              std::vector<double>,
                                              std::vector<std::string>>;

/**
 * Creates an owned copy (LogRecordAttributeValue) of a non-owning AttributeValue.
 */
struct AttributeConverter
{
  LogRecordAttributeValue operator()(bool v) { return LogRecordAttributeValue(v); }
  LogRecordAttributeValue operator()(int32_t v) { return LogRecordAttributeValue(v); }
  LogRecordAttributeValue operator()(uint32_t v) { return LogRecordAttributeValue(v); }
  /*LogRecordAttributeValue operator()(int v)
  {
    return LogRecordAttributeValue(static_cast<int64_t>(v));
  }*/
  LogRecordAttributeValue operator()(int64_t v) { return LogRecordAttributeValue(v); }
  LogRecordAttributeValue operator()(uint64_t v) { return LogRecordAttributeValue(v); }
  LogRecordAttributeValue operator()(double v) { return LogRecordAttributeValue(v); }
  LogRecordAttributeValue operator()(nostd::string_view v)
  {
    return LogRecordAttributeValue(std::string(v));
  }
  LogRecordAttributeValue operator()(nostd::span<const bool> v) { return convertSpan<bool>(v); }
  LogRecordAttributeValue operator()(nostd::span<const int32_t> v)
  {
    return convertSpan<int32_t>(v);
  }
  LogRecordAttributeValue operator()(nostd::span<const uint32_t> v)
  {
    return convertSpan<uint32_t>(v);
  }
  LogRecordAttributeValue operator()(nostd::span<const int64_t> v)
  {
    return convertSpan<int64_t>(v);
  }
  LogRecordAttributeValue operator()(nostd::span<const uint64_t> v)
  {
    return convertSpan<uint64_t>(v);
  }
  LogRecordAttributeValue operator()(nostd::span<const double> v) { return convertSpan<double>(v); }
  LogRecordAttributeValue operator()(nostd::span<const nostd::string_view> v)
  {
    return convertSpan<std::string>(v);
  }

  template <typename T, typename U = T>
  LogRecordAttributeValue convertSpan(nostd::span<const U> vals)
  {
    std::vector<T> copy;
    for (auto &val : vals)
    {
      copy.push_back(T(val));
    }

    return LogRecordAttributeValue(std::move(copy));
  }
};

/**
 * Class for storing attributes.
 */
class AttributeMap
{
public:
  // Contruct empty attribute map
  AttributeMap(){};

  // Contruct attribute map and populate with attributes
  AttributeMap(const opentelemetry::common::KeyValueIterable &attributes)
  {
    attributes.ForEachKeyValue([&](nostd::string_view key,
                                   opentelemetry::common::AttributeValue value) noexcept {
      SetAttribute(key, value);
      return true;
    });
  }

  const std::unordered_map<std::string, LogRecordAttributeValue> &GetAttributes() const noexcept
  {
    return attributes_;
  }

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept
  {
    attributes_[std::string(key)] = nostd::visit(converter_, value);
  }

private:
  std::unordered_map<std::string, LogRecordAttributeValue> attributes_;
  AttributeConverter converter_;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
