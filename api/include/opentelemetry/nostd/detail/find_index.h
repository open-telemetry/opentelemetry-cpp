#pragma once

#include <cstddef>

#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
namespace detail
{
constexpr std::size_t not_found = static_cast<std::size_t>(-1);
constexpr std::size_t ambiguous = static_cast<std::size_t>(-2);

inline constexpr std::size_t find_index_impl(std::size_t result, std::size_t)
{
  return result;
}

template <typename... Bs>
inline constexpr std::size_t find_index_impl(std::size_t result, std::size_t idx, bool b, Bs... bs)
{
  return b ? (result != not_found ? ambiguous : find_index_impl(idx, idx + 1, bs...))
           : find_index_impl(result, idx + 1, bs...);
}

template <typename T, typename... Ts>
inline constexpr std::size_t find_index()
{
  return find_index_impl(not_found, 0, std::is_same<T, Ts>::value...);
}

template <std::size_t I>
using find_index_sfinae_impl =
    enable_if_t<I != not_found && I != ambiguous, std::integral_constant<std::size_t, I>>;

template <typename T, typename... Ts>
using find_index_sfinae = find_index_sfinae_impl<find_index<T, Ts...>()>;

template <std::size_t I>
struct find_index_checked_impl : std::integral_constant<std::size_t, I>
{
  static_assert(I != not_found, "the specified type is not found.");
  static_assert(I != ambiguous, "the specified type is ambiguous.");
};

template <typename T, typename... Ts>
using find_index_checked = find_index_checked_impl<find_index<T, Ts...>()>;
}  // namespace detail
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
