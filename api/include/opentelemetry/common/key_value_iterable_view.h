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

template <class T>
class KeyValueIterableView final : public KeyValueIterable
{
  static_assert(detail::is_key_value_iterable<T>::value, "Must be a key-value iterable");

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
