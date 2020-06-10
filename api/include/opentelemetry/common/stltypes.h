#pragma once

#include <opentelemetry/version.h>

#ifndef HAVE_STDLIB_CPP
/* Private implementation of standard C++20 types */
#  include "opentelemetry/nostd/stl/shared_ptr.h"
#  include "opentelemetry/nostd/stl/span.h"
#  include "opentelemetry/nostd/stl/string_view.h"
#  include "opentelemetry/nostd/stl/unique_ptr.h"
#  include "opentelemetry/nostd/stl/utility.h"
#  include "opentelemetry/nostd/stl/variant.h"
#else
/* Standard library implementation */
#  include <cstddef>
#  include <memory>
#  include <string_view>
#  include <utility>
#  include <variant>

#  ifdef HAVE_GSL
#    include <gsl/gsl>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
template <class ElementType, std::size_t Extent = gsl::dynamic_extent>
using span = gsl::span<ElementType, Extent>;
}
OPENTELEMETRY_END_NAMESPACE
#  else

#    include <span>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
template <class ElementType, std::size_t Extent = std::dynamic_extent>
using span = std::span<ElementType, Extent>;
}
OPENTELEMETRY_END_NAMESPACE
#  endif
OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
template <class... _Types>
using variant = std::variant<_Types...>;

using string_view = std::string_view;

/**
 * Back port of std::size
 *
 * See https://en.cppreference.com/w/cpp/iterator/size
 */
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

template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template <class... _Types>
using unique_ptr = std::unique_ptr<_Types...>;

template <class... _Types>
using shared_ptr = std::shared_ptr<_Types...>;

// template <class T, class... Types>
// using get = std::get<T, Types...>;

/* TODO: check if we require C++17 for that */
template <class T>
constexpr auto get = [](auto &&t) constexpr -> decltype(auto)
{
  return std::get<T>(std::forward<decltype(t)>(t));
};

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
#endif
