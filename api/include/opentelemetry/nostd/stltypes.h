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

// Standard library implementation requires at least C++17 compiler.
// Older C++14 compilers may provide support for __has_include as a
// conforming extension.
#if defined __has_include
#  if __has_include(<version>)  // Check for __cpp_{feature}
#    include <version>
#    if defined(__cpp_lib_span)
#      define HAVE_SPAN
#    endif
#  endif
#  if __has_include(<span>) && !defined(HAVE_SPAN)  // Check for span
#    define HAVE_SPAN
#  endif
#  if !__has_include(<string_view>)  // Check for string_view
#    error \
        "STL library does not support std::span. Possible solution:"                   \
         " - #undef HAVE_CPP_STDLIB // to use OpenTelemetry nostd::string_view"
#  endif
#endif

#include <cstddef>
#include <memory>
#include <string_view>
#include <utility>
#include <variant>

#if !defined(HAVE_SPAN)

#  if defined(HAVE_GSL)
#    include <type_traits>
// Guidelines Support Library provides an implementation of std::span
#    include <gsl/gsl>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
template <class ElementType, std::size_t Extent = gsl::dynamic_extent>
using span = gsl::span<ElementType, Extent>;
}
OPENTELEMETRY_END_NAMESPACE
#  else
// No span implementation provided.
#    error \
        "STL library does not support std::span. Possible solutions:"                  \
         " - #undef HAVE_CPP_STDLIB // to use OpenTelemetry nostd::span .. or      "    \
         " - #define HAVE_GSL       // to use gsl::span                            "
#  endif

#else  // HAVE_SPAN
// Using std::span (https://wg21.link/P0122R7) from Standard Library available in C++20 :
// - GCC libstdc++ 10+
// - Clang libc++ 7
// - MSVC Standard Library 19.26*
// - Apple Clang 10.0.0*
#  include <span>
OPENTELEMETRY_BEGIN_NAMESPACE

namespace nostd
{
constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

template <class ElementType, std::size_t Extent = nostd::dynamic_extent>
using span = std::span<ElementType, Extent>;
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
#endif  // of HAVE_SPAN

OPENTELEMETRY_BEGIN_NAMESPACE
// Standard Type aliases in nostd namespace
namespace nostd
{

//
// Back port of std::data
//
// See https://en.cppreference.com/w/cpp/iterator/data
//
template <class C>
auto data(C &c) noexcept(noexcept(c.data())) -> decltype(c.data())
{
  return c.data();
}

template <class C>
auto data(const C &c) noexcept(noexcept(c.data())) -> decltype(c.data())
{
  return c.data();
}

template <class T, size_t N>
T *data(T (&array)[N]) noexcept
{
  return array;
}

template <class E>
const E *data(std::initializer_list<E> list) noexcept
{
  return list.begin();
}

//
// Back port of std::size
//
// See https://en.cppreference.com/w/cpp/iterator/size
//
template <class C>
auto size(const C &c) noexcept(noexcept(c.size())) -> decltype(c.size())
{
  return c.size();
}

template <class T, size_t N>
size_t size(T (&array)[N]) noexcept
{
  return N;
}

// nostd::variant<...>
template <class... _Types>
using variant = std::variant<_Types...>;

// nostd::string_view
using string_view = std::string_view;

// nostd::enable_if_t<...>
template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

// nostd::unique_ptr<T...>
template <class... _Types>
using unique_ptr = std::unique_ptr<_Types...>;

// nostd::shared_ptr<T...>
template <class... _Types>
using shared_ptr = std::shared_ptr<_Types...>;

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
#    define THROW_BAD_VARIANT_ACCESS    throw_bad_variant_access()
#  else
#    define THROW_BAD_VARIANT_ACCESS    std::terminate()
#  endif

//
// nostd::get<...> for Apple Clang
//
template <typename T, class... Types>
constexpr auto get_type = [](auto &&t) constexpr -> decltype(auto)
{
  auto v      = t;
  auto result = std::get_if<T>(&v); // TODO: optimize with std::forward(t) if t is not rvalue
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
constexpr T /*&&*/get(std::variant<Types...> &&v)
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
constexpr const std::variant_alternative_t<I, std::variant<Types...>> &get(const std::variant<Types...> &v)
{
  return std::get<I, Types...>(v);
};

template <std::size_t I, class... Types>
constexpr const std::variant_alternative_t<I, std::variant<Types...>> &&get(const std::variant<Types...> &&v)
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

template <std::size_t N>
using make_index_sequence = std::make_index_sequence<N>;

template <std::size_t... Ints>
using index_sequence = std::index_sequence<Ints...>;

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
