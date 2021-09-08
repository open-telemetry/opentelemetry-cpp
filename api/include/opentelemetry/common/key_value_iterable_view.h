// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
// NOTE - code within `detail` namespace implements internal details, and not part
// of the public interface.
namespace detail
{
inline void take_key_value(nostd::string_view, common::AttributeValue) {}

template <class T>
auto is_key_value_iterable_impl(T iterable)
    -> decltype(take_key_value(std::begin(iterable)->first, std::begin(iterable)->second),
                nostd::size(iterable),
                std::true_type{});

std::false_type is_key_value_iterable_impl(...);

template <class T>
struct is_key_value_iterable
{
  static const bool value = decltype(detail::is_key_value_iterable_impl(std::declval<T>()))::value;
};
}  // namespace detail

/**
 * @brief Container for key-value pairs that can transform every value in it to one of types
 * listed in common::AttributeValue. It may contain value types that are not directly map'able
 * to primitive value types. In that case the `ForEachKeyValue` method acts as a transform to
 * convert the value type to one listed under AtributeValue (bool, int32_t, int64_t, uint32_t,
 * uint64_t, double, nostd::string_view, or arrays of primite types). For example, if UUID,
 * GUID, or UTF-16 string type is passed as one of values stored inside this container, the
 * container itself may provide a custom implementation of `ForEachKeyValue` to transform the
 * 'non-standard' type to one of the standard types.
 */
template <class T>
class KeyValueIterableView final : public KeyValueIterable
{

public:
  explicit KeyValueIterableView(const T &container) noexcept : container_{&container} {}

  // KeyValueIterable
  bool ForEachKeyValue(nostd::function_ref<bool(nostd::string_view, common::AttributeValue)>
                           callback) const noexcept override
  {
    auto iter = std::begin(*container_);
    auto last = std::end(*container_);
    for (; iter != last; ++iter)
    {
      if (!callback(iter->first, iter->second))
      {
        return false;
      }
    }
    return true;
  }

  size_t size() const noexcept override { return nostd::size(*container_); }

private:
  const T *container_;
};
}  // namespace common
OPENTELEMETRY_END_NAMESPACE
