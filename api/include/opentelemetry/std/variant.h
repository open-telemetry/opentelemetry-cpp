// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "opentelemetry/version.h"

#include <cstddef>
#include <memory>
#include <utility>
#include <variant>

OPENTELEMETRY_BEGIN_NAMESPACE
// Standard Type aliases in nostd namespace
namespace nostd
{

// nostd::variant<...>
template <class... _Types>
using variant = std::variant<_Types...>;

template <class... _Types>
using variant_size = std::variant_size<_Types...>;

#if defined(__APPLE__) && defined(_LIBCPP_USE_AVAILABILITY_APPLE)
// Apple Platforms provide std::bad_variant_access only in newer versions of OS.
// To keep API compatible with any version of OS - we are providing our own
// implementation of nostd::bad_variant_access exception.
#  if __EXCEPTIONS

// nostd::bad_variant_access
class bad_variant_access : public std::exception
{
public:
  virtual const char *what() const noexcept override { return "bad_variant_access"; }
};

[[noreturn]] inline void throw_bad_variant_access()
{
  throw bad_variant_access{};
}
#  endif

#  if __EXCEPTIONS
#    define THROW_BAD_VARIANT_ACCESS throw_bad_variant_access()
#  else
#    define THROW_BAD_VARIANT_ACCESS std::terminate()
#  endif

//
// nostd::get<...> for Apple Clang
//
template <typename T, class... Types>
constexpr auto get_type = [](auto &&t) constexpr -> decltype(auto)
{
  auto v      = t;
  auto result = std::get_if<T>(&v);  // TODO: optimize with std::forward(t) if t is not rvalue
  if (result)
  {
    return *result;
  }
  THROW_BAD_VARIANT_ACCESS;
  return *result;
};

template <std::size_t I, class... Types>
constexpr auto get_index = [](auto &&t) constexpr -> decltype(auto)
{
  auto v      = t;
  auto result = std::get_if<I>(&v);  // TODO: optimize with std::forward(t) if t is not rvalue
  if (result)
  {
    return *result;
  }
  THROW_BAD_VARIANT_ACCESS;
  return *result;
};

template <std::size_t I, class... Types>
constexpr std::variant_alternative_t<I, std::variant<Types...>> &get(std::variant<Types...> &v)
{
  return get_index<I, Types...>(v);
};

template <std::size_t I, class... Types>
constexpr std::variant_alternative_t<I, std::variant<Types...>> &&get(std::variant<Types...> &&v)
{
  return get_index<I, Types...>(std::forward<decltype(v)>(v));
};

template <std::size_t I, class... Types>
constexpr const std::variant_alternative_t<I, std::variant<Types...>> &get(
    const std::variant<Types...> &v)
{
  return get_index<I, Types...>(v);
};

template <std::size_t I, class... Types>
constexpr const std::variant_alternative_t<I, std::variant<Types...>> &&get(
    const std::variant<Types...> &&v)
{
  return get_index<I, Types...>(std::forward<decltype(v)>(v));
};

template <class T, class... Types>
constexpr T &get(std::variant<Types...> &v)
{
  return get_type<T, Types...>(v);
};

template <class T, class... Types>
constexpr T /*&&*/ get(std::variant<Types...> &&v)
{
  return get_type<T, Types...>(v);
};

template <class T, class... Types>
constexpr const T &get(const std::variant<Types...> &v)
{
  return get_type<T, Types...>(v);
};

template <class T, class... Types>
constexpr const T &&get(const std::variant<Types...> &&v)
{
  return get_type<T, Types...>(std::forward<decltype(v)>(v));
};

template <class _Callable, class... _Variants>
constexpr auto visit(_Callable &&_Obj, _Variants &&... _Args)
{
  // Ref:
  // https://stackoverflow.com/questions/52310835/xcode-10-call-to-unavailable-function-stdvisit
  return std::__variant_detail::__visitation::__variant::__visit_value(_Obj, _Args...);
};

#else

template <std::size_t I, class... Types>
constexpr std::variant_alternative_t<I, std::variant<Types...>> &get(std::variant<Types...> &v)
{
  return std::get<I, Types...>(v);
};

template <std::size_t I, class... Types>
constexpr std::variant_alternative_t<I, std::variant<Types...>> &&get(std::variant<Types...> &&v)
{
  return std::get<I, Types...>(std::forward<decltype(v)>(v));
};

template <std::size_t I, class... Types>
constexpr const std::variant_alternative_t<I, std::variant<Types...>> &get(
    const std::variant<Types...> &v)
{
  return std::get<I, Types...>(v);
};

template <std::size_t I, class... Types>
constexpr const std::variant_alternative_t<I, std::variant<Types...>> &&get(
    const std::variant<Types...> &&v)
{
  return std::get<I, Types...>(std::forward<decltype(v)>(v));
};

template <class T, class... Types>
constexpr T &get(std::variant<Types...> &v)
{
  return std::get<T, Types...>(v);
};

template <class T, class... Types>
constexpr T &&get(std::variant<Types...> &&v)
{
  return std::get<T, Types...>(std::forward<decltype(v)>(v));
};

template <class T, class... Types>
constexpr const T &get(const std::variant<Types...> &v)
{
  return std::get<T, Types...>(v);
};

template <class T, class... Types>
constexpr const T &&get(const std::variant<Types...> &&v)
{
  return std::get<T, Types...>(std::forward<decltype(v)>(v));
};

template <class _Callable, class... _Variants>
constexpr auto visit(_Callable &&_Obj, _Variants &&... _Args)
{
  return std::visit<_Callable, _Variants...>(static_cast<_Callable &&>(_Obj),
                                             static_cast<_Variants &&>(_Args)...);
};

#endif

/*
# if _HAS_CXX20
template <class _Ret, class _Callable, class... _Variants>
constexpr _Ret visit(_Callable &&_Obj, _Variants &&... _Args)
{
  return std::visit<_Ret, _Callable, _Variants...>(
      static_cast<_Callable &&>(_Obj),
      static_cast<_Variants &&>(_Args)...);
};
# endif
*/

// nostd::holds_alternative
template <std::size_t I, typename... Ts>
inline constexpr bool holds_alternative(const variant<Ts...> &v) noexcept
{
  return v.index() == I;
}

template <typename T, typename... Ts>
inline constexpr bool holds_alternative(const variant<Ts...> &v) noexcept
{
  return std::holds_alternative<T, Ts...>(v);
}

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
