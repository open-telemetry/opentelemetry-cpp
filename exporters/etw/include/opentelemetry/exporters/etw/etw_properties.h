#pragma once

#include "opentelemetry/version.h"

#include "opentelemetry/common/key_value_iterable_view.h"

#include <opentelemetry/nostd/span.h>
#include <map>
#include <string>
#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace core
{};
namespace trace
{};
namespace exporter
{
namespace ETW
{

/**
 * @brief PropertyVariant provides:
 * - a constructor to initialize from initializer lists
 * - an owning wrapper around `common::AttributeValue`
 */
using PropertyVariant =
    nostd::variant<bool,
                   int32_t,
                   int64_t,
                   uint32_t,
                   uint64_t,
                   double,
                   std::string,
#ifdef HAVE_CSTRING_TYPE
                   const char *,
#endif
#ifdef HAVE_SPAN_BYTE
                   // TODO: 8-bit byte arrays / binary blobs are not part of OT spec yet!
                   // Ref: https://github.com/open-telemetry/opentelemetry-specification/issues/780
                   std::vector<uint8_t>,
#endif
                   std::vector<bool>,
                   std::vector<int32_t>,
                   std::vector<int64_t>,
                   std::vector<uint32_t>,
                   std::vector<uint64_t>,
                   std::vector<double>,
                   std::vector<std::string>>;

/**
 * @brief PropertyValue class that holds PropertyVariant and
 * provides converter for non-owning common::AttributeValue
 */
class PropertyValue : public PropertyVariant
{

  /**
   * @brief Convert span<T> to vector<T>
   * @tparam T
   * @param source
   * @return
   */
  template <typename T>
  static std::vector<T> to_vector(const nostd::span<const T, std::size_t(-1)> &source)
  {
    return std::vector<T>(source.begin(), source.end());
  };

  /**
   * @brief Convert span<string_view> to vector<string>
   * @param source
   * @return
   */
  std::vector<std::string> static to_vector(const nostd::span<const nostd::string_view> &source)
  {
    std::vector<std::string> result;
    result.reserve(source.size());
    for (const auto &item : source)
    {
      result.push_back(std::string(item.data()));
    }
    return result;
  };

  /**
   * @brief Convert vector<INTEGRAL> to span<INTEGRAL>.
   * @tparam T
   * @param std::vector<T>
   * @return nostd::span<const T>
   */
  template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
  static nostd::span<const T> to_span(const std::vector<T> &vec)
  {
    nostd::span<const T> result(vec.data(), vec.size());
    return result;
  };

  /**
   * @brief Convert vector<FLOAT> to span<const FLOAT>.
   * @tparam T
   * @param std::vector<T>
   * @return nostd::span<const T>
   */
  template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
  static nostd::span<const T> to_span(const std::vector<T> &vec)
  {
    nostd::span<const T> result(vec.data(), vec.size());
    return result;
  };

#if 0
  /**
   * @brief Convert vector<bool> to span<const bool>.
   * FIXME: std::vector<bool> is a special compact type that does not have .data() member
   *
   * @param v
   * @return
   */
  static nostd::span<const bool> to_span(const std::vector<bool> &vec)
  {
    nostd::span<const bool> result(vec.data(), vec.size());
    return result;
  };
#endif

public:
  /**
   * @brief PropertyValue from bool
   * @param v
   * @return
   */
  PropertyValue(bool value) : PropertyVariant(value){};

  /**
   * @brief PropertyValue from integral.
   * @param v
   * @return
   */
  template <typename TInteger, std::enable_if_t<std::is_integral<TInteger>::value, bool> = true>
  PropertyValue(TInteger number) : PropertyVariant(number){};

  /**
   * @brief PropertyValue from floating point.
   * @param v
   * @return
   */
  template <typename TFloat, std::enable_if_t<std::is_floating_point<TFloat>::value, bool> = true>
  PropertyValue(TFloat number) : PropertyVariant(double(number)){};

  /**
   * @brief Default PropertyValue (int32_t=0)
   * @param v
   * @return
   */
  PropertyValue() : PropertyVariant(int32_t(0)){};

  /**
   * @brief PropertyValue from array of characters as string.
   *
   * @param v
   * @return
   */
  PropertyValue(char value[]) : PropertyVariant(std::string(value)){};

  /**
   * @brief PropertyValue from array of characters as string.
   *
   * @param v
   * @return
   */
  PropertyValue(const char *value) : PropertyVariant(value){};

  /**
   * @brief PropertyValue from string.
   *
   * @param v
   * @return
   */
  PropertyValue(const std::string &value) : PropertyVariant(value){};

  /**
   * @brief PropertyValue from vector as array.
   * @return
   */
  template <typename T>
  PropertyValue(std::vector<T> value) : PropertyVariant(value){};

  /**
   * @brief Convert owning PropertyValue to non-owning common::AttributeValue
   * @return
   */
  PropertyValue &FromAttributeValue(const common::AttributeValue &v)
  {
    switch (v.index())
    {
      case common::AttributeType::TYPE_BOOL:
        PropertyVariant::operator=(nostd::get<bool>(v));
        break;
      case common::AttributeType::TYPE_INT:
        PropertyVariant::operator=(nostd::get<int32_t>(v));
        break;
      case common::AttributeType::TYPE_INT64:
        PropertyVariant::operator=(nostd::get<int64_t>(v));
        break;
      case common::AttributeType::TYPE_UINT:
        PropertyVariant::operator=(nostd::get<uint32_t>(v));
        break;
      case common::AttributeType::TYPE_UINT64:
        PropertyVariant::operator=(nostd::get<uint64_t>(v));
        break;
      case common::AttributeType::TYPE_DOUBLE:
        PropertyVariant::operator=(nostd::get<double>(v));
        break;
      case common::AttributeType::TYPE_STRING:
      {
        PropertyVariant::operator=(nostd::string_view(nostd::get<nostd::string_view>(v)).data());
        break;
      };

#ifdef HAVE_CSTRING_TYPE
      case common::AttributeType::TYPE_CSTRING:
        PropertyVariant::operator=(nostd::get<const char *>(v));
        break;
#endif

#ifdef HAVE_SPAN_BYTE
      case common::AttributeType::TYPE_SPAN_BYTE:
        PropertyVariant::operator=(to_vector(nostd::get<nostd::span<const uint8_t>>(v)));
        break;
#endif

      case common::AttributeType::TYPE_SPAN_BOOL:
        PropertyVariant::operator=(to_vector(nostd::get<nostd::span<const bool>>(v)));
        break;

      case common::AttributeType::TYPE_SPAN_INT:
        PropertyVariant::operator=(to_vector(nostd::get<nostd::span<const int32_t>>(v)));
        break;

      case common::AttributeType::TYPE_SPAN_INT64:
        PropertyVariant::operator=(to_vector(nostd::get<nostd::span<const int64_t>>(v)));
        break;

      case common::AttributeType::TYPE_SPAN_UINT:
        PropertyVariant::operator=(to_vector(nostd::get<nostd::span<const uint32_t>>(v)));
        break;

      case common::AttributeType::TYPE_SPAN_UINT64:
        PropertyVariant::operator=(to_vector(nostd::get<nostd::span<const uint64_t>>(v)));
        break;

      case common::AttributeType::TYPE_SPAN_DOUBLE:
        PropertyVariant::operator=(to_vector(nostd::get<nostd::span<const double>>(v)));
        break;

      case common::AttributeType::TYPE_SPAN_STRING:
        PropertyVariant::operator=(to_vector(nostd::get<nostd::span<const nostd::string_view>>(v)));
        break;

      default:
        break;
    }
    return (*this);
  }

  /**
   * @brief Convert owning PropertyValue to non-owning common::AttributeValue
   * @param other
   */
  common::AttributeValue ToAttributeValue() const
  {
    common::AttributeValue value;

    switch (this->index())
    {
      case common::AttributeType::TYPE_BOOL:
        value = nostd::get<bool>(*this);
        break;
      case common::AttributeType::TYPE_INT:
        value = nostd::get<int32_t>(*this);
        break;
      case common::AttributeType::TYPE_INT64:
        value = nostd::get<int64_t>(*this);
        break;
      case common::AttributeType::TYPE_UINT:
        value = nostd::get<uint32_t>(*this);
        break;
      case common::AttributeType::TYPE_UINT64:
        value = nostd::get<uint64_t>(*this);
        break;
      case common::AttributeType::TYPE_DOUBLE:
        value = nostd::get<double>(*this);
        break;

      case common::AttributeType::TYPE_STRING:
      {
        const std::string &str = nostd::get<std::string>(*this);
        return nostd::string_view(str.data(), str.size());
        break;
      }

#ifdef HAVE_CSTRING_TYPE
      case common::AttributeType::TYPE_CSTRING:
        value = nostd::get<const char *>(*this);
        break;
#endif

#ifdef HAVE_SPAN_BYTE
      case common::AttributeType::TYPE_SPAN_BYTE:
        value = to_span(nostd::get<std::vector<uint8_t>>(self));
        break;
#endif

      case common::AttributeType::TYPE_SPAN_BOOL:
      {
        const auto &vec = nostd::get<std::vector<bool>>(*this);
        // FIXME: sort out how to remap from vector<bool> to span<bool>
        break;
      }

      case common::AttributeType::TYPE_SPAN_INT:
        value = to_span(nostd::get<std::vector<int32_t>>(*this));
        break;

      case common::AttributeType::TYPE_SPAN_INT64:
        value = to_span(nostd::get<std::vector<int64_t>>(*this));
        break;

      case common::AttributeType::TYPE_SPAN_UINT:
        value = to_span(nostd::get<std::vector<uint32_t>>(*this));
        break;

      case common::AttributeType::TYPE_SPAN_UINT64:
        value = to_span(nostd::get<std::vector<uint64_t>>(*this));
        break;

      case common::AttributeType::TYPE_SPAN_DOUBLE:
        value = to_span(nostd::get<std::vector<double>>(*this));
        break;

      case common::AttributeType::TYPE_SPAN_STRING:
        // FIXME: sort out how to remap from vector<string> to span<string_view>
        // value = to_span(nostd::get<std::vector<std::string>>(self));
        break;

      default:
        break;
    }
    return value;
  }
};

/**
 * @brief Map of PropertyValue
 */
using PropertyValueMap = std::map<std::string, PropertyValue>;

/**
 * @brief Map of PropertyValue with common::KeyValueIterable interface.
 */
class Properties : public common::KeyValueIterable, public PropertyValueMap
{

  /**
   * @brief Helper tyoe for map constructor.
   */
  using PropertyValueType = std::pair<const std::string, PropertyValue>;

public:
  /**
   * @brief PropertyValueMap constructor.
   */
  Properties() : PropertyValueMap(){};

  /**
   * @brief PropertyValueMap constructor from initializer list.
   */
  Properties(const std::initializer_list<PropertyValueType> properties) : PropertyValueMap()
  {
    (*this) = (properties);
  };

  /**
   * @brief PropertyValueMap assignment operator from initializer list.
   */
  Properties &operator=(std::initializer_list<PropertyValueType> properties)
  {
    PropertyValueMap::operator=(properties);
    return (*this);
  };

  /**
   * @brief PropertyValueMap constructor from map.
   */
  Properties(const PropertyValueMap &properties) : PropertyValueMap() { (*this) = properties; };

  /**
   * @brief PropertyValueMap assignment operator from map.
   */
  Properties &operator=(const PropertyValueMap &properties)
  {
    PropertyValueMap::operator=(properties);
    return (*this);
  };

  /**
   * @brief PropertyValueMap constructor from KeyValueIterable
   * allows to convert non-Owning KeyValueIterable to owning
   * container.
   *
   */
  Properties(const common::KeyValueIterable &other) { (*this) = other; }

  /**
   * @brief PropertyValueMap assignment operator.
   */
  Properties &operator=(const common::KeyValueIterable &other)
  {
    clear();
    other.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
      std::string k(key.data(), key.length());
      (*this)[k].FromAttributeValue(value);
      return true;
    });
    return (*this);
  };

  /**
   * @brief PropertyValueMap property accessor.
   */
  PropertyValue &operator[](const std::string &k) { return PropertyValueMap::operator[](k); }

  /**
   * Iterate over key-value pairs
   * @param callback a callback to invoke for each key-value. If the callback returns false,
   * the iteration is aborted.
   * @return true if every key-value pair was iterated over
   */
  bool ForEachKeyValue(
      nostd::function_ref<bool(nostd::string_view, common::AttributeValue)> callback) const
      noexcept override
  {
    for (const auto &kv : (*this))
    {
      const common::AttributeValue &value = kv.second.ToAttributeValue();
      if (!callback(nostd::string_view{kv.first}, value))
      {
        return false;
      }
    }
    return true;
  };

  /**
   * @return the number of key-value pairs
   */
  size_t size() const noexcept override { return PropertyValueMap::size(); };
};

}  // namespace ETW
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
