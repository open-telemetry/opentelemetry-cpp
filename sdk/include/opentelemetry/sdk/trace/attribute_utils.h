#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * A counterpart to AttributeValue that makes sure a value is owned. This
 * replaces all non-owning references with owned copies.
 */
using SpanDataAttributeValue = nostd::variant<bool,
                                              int32_t,
                                              uint32_t,
                                              int64_t,
                                              uint64_t,
                                              double,
                                              std::string,
#ifdef HAVE_CSTRING_TYPE
                                              const char *,
#endif
#ifdef HAVE_SPAN_BYTE
                                              std::vector<uint8_t>,
#endif
                                              std::vector<bool>,
                                              std::vector<int32_t>,
                                              std::vector<uint32_t>,
                                              std::vector<int64_t>,
                                              std::vector<uint64_t>,
                                              std::vector<double>,
                                              std::vector<std::string>>;

/**
 * Creates an owned copy (SpanDataAttributeValue) of a non-owning AttributeValue.
 */
struct AttributeConverter
{
  SpanDataAttributeValue operator()(bool v) { return SpanDataAttributeValue(v); }
  SpanDataAttributeValue operator()(int32_t v) { return SpanDataAttributeValue(v); }
  SpanDataAttributeValue operator()(uint32_t v) { return SpanDataAttributeValue(v); }
  SpanDataAttributeValue operator()(int64_t v) { return SpanDataAttributeValue(v); }
  SpanDataAttributeValue operator()(uint64_t v) { return SpanDataAttributeValue(v); }
  SpanDataAttributeValue operator()(double v) { return SpanDataAttributeValue(v); }
  SpanDataAttributeValue operator()(nostd::string_view v)
  {
    return SpanDataAttributeValue(std::string(v.data(), v.size()));
  }
  SpanDataAttributeValue operator()(const char *s)
  {
    return SpanDataAttributeValue(std::string(s));
  }
#ifdef HAVE_SPAN_BYTE
  SpanDataAttributeValue operator()(nostd::span<const uint8_t> v)
  {
    return convertSpan<uint8_t>(v);
  }
#endif
  SpanDataAttributeValue operator()(nostd::span<const bool> v) { return convertSpan<bool>(v); }
  SpanDataAttributeValue operator()(nostd::span<const int32_t> v)
  {
    return convertSpan<int32_t>(v);
  }
  SpanDataAttributeValue operator()(nostd::span<const uint32_t> v)
  {
    return convertSpan<uint32_t>(v);
  }
  SpanDataAttributeValue operator()(nostd::span<const int64_t> v)
  {
    return convertSpan<int64_t>(v);
  }
  SpanDataAttributeValue operator()(nostd::span<const uint64_t> v)
  {
    return convertSpan<uint64_t>(v);
  }
  SpanDataAttributeValue operator()(nostd::span<const double> v) { return convertSpan<double>(v); }
  SpanDataAttributeValue operator()(nostd::span<const nostd::string_view> v)
  {
    return convertSpan<std::string>(v);
  }

  template <typename T, typename U = T>
  SpanDataAttributeValue convertSpan(nostd::span<const U> vals)
  {
    const std::vector<T> copy(vals.begin(), vals.end());
    return SpanDataAttributeValue(std::move(copy));
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

  const std::unordered_map<std::string, SpanDataAttributeValue> &GetAttributes() const noexcept
  {
    return attributes_;
  }
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept
  {
    attributes_[std::string(key)] = nostd::visit(converter_, value);
  }

private:
  std::unordered_map<std::string, SpanDataAttributeValue> attributes_;
  AttributeConverter converter_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
