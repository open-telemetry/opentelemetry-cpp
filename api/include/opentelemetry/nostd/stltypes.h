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
#  if defined __has_include
#    if __has_include(<version>)  // Check for __cpp_{feature}
#      include <version>
#      if defined(__cpp_lib_span)
#        define HAVE_SPAN
#      endif
#    endif
#    if __has_include(<span>) && !defined(HAVE_SPAN)  // Check for span
#      define HAVE_SPAN
#    endif
#    if !__has_include(<string_view>)  // Check for string_view
#      error \
          "STL library does not support std::span. Possible solution:"                   \
         " - #undef HAVE_CPP_STDLIB // to use OpenTelemetry nostd::string_view"
#    endif
#  endif

#  include <cstddef>
#  include <memory>
#  include <string_view>
#  include <utility>
#  include <variant>

#  if !defined(HAVE_SPAN)

#    if defined(HAVE_GSL)
#      include <type_traits>
// Guidelines Support Library provides an implementation of std::span
#      include <gsl/gsl>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
template <class ElementType, std::size_t Extent = gsl::dynamic_extent>
using span = gsl::span<ElementType, Extent>;
}
OPENTELEMETRY_END_NAMESPACE
#    else
// No span implementation provided.
#      error \
          "STL library does not support std::span. Possible solutions:"                  \
         " - #undef HAVE_CPP_STDLIB // to use OpenTelemetry nostd::span .. or      "    \
         " - #define HAVE_GSL       // to use gsl::span                            "
#    endif

#  else  // HAVE_SPAN
// Using std::span (https://wg21.link/P0122R7) from Standard Library available in C++20 :
// - GCC libstdc++ 10+
// - Clang libc++ 7
// - MSVC Standard Library 19.26*
// - Apple Clang 10.0.0*
#    include <span>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
template <class ElementType, std::size_t Extent = std::dynamic_extent>
using span = std::span<ElementType, Extent> ;
}
OPENTELEMETRY_END_NAMESPACE
#  endif  // of HAVE_SPAN

OPENTELEMETRY_BEGIN_NAMESPACE
// Standard Type aliases in nostd namespace
namespace nostd
{

// nostd::variant<...>
template <class... _Types>
using variant = std::variant<_Types...>;

// nostd::string_view
using string_view = std::string_view;

// nostd::size<T>
template <class T>
auto size(const T &c) noexcept(noexcept(c.size())) -> decltype(c.size())
{
  return c.size();
}

// nostd::size<T, N>
template <class T, size_t N>
size_t size(T (&array)[N]) noexcept
{
  return N;
}

// nostd::enable_if_t<...>
template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

// nostd::unique_ptr<T...>
template <class... _Types>
using unique_ptr = std::unique_ptr<_Types...>;

// nostd::shared_ptr<T...>
template <class... _Types>
using shared_ptr = std::shared_ptr<_Types...>;

#  if defined(__APPLE__) && defined(_LIBCPP_USE_AVAILABILITY_APPLE)
// Apple Platforms provide std::bad_variant_access only in newer versions of OS.
// To keep API compatible with any version of OS - we are providing our own
// implementation of nostd::bad_variant_access exception.
#    if __EXCEPTIONS

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
#    endif

// nostd::get<...> for Apple Clang
template <class T, class... Types>
constexpr auto get = [](auto &&t) constexpr -> decltype(auto)
{
  auto result = std::get_if<T>(&std::forward<decltype(t)>(t));
  if (result)
  {
    return *result;
  }
#    if __EXCEPTIONS
  throw_bad_variant_access();
#    else
  std::terminate();
  return result;
#    endif
};

#  else

// nostd::get<T> for other C++17 compatible compilers
template <class T>
constexpr auto get = [](auto &&t) constexpr -> decltype(auto)
{
  return std::get<T>(std::forward<decltype(t)>(t));
};

#  endif

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
