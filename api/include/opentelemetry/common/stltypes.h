#pragma once

#include <opentelemetry/version.h>

#ifndef HAVE_STDLIB_CPP
// Private OpenTelemetry reimplementation of standard C++20 types
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/nostd/unique_ptr.h"
#  include "opentelemetry/nostd/utility.h"
#  include "opentelemetry/nostd/variant.h"
#else
// Standard library implementation requires at least C++17
#  include <cstddef>
#  include <memory>
#  include <string_view>
#  include <utility>
#  include <variant>

#  ifdef HAVE_GSL
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
// If not using GSL, then use std::span (only available in C++20)
#    include <span>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
template <class ElementType, std::size_t Extent = std::dynamic_extent>
using span = std::span<ElementType, Extent>;
}
OPENTELEMETRY_END_NAMESPACE
#  endif // of HAVE_GSL

OPENTELEMETRY_BEGIN_NAMESPACE
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

// nostd::get<T>
template <class T>
constexpr auto get = [](auto &&t) constexpr -> decltype(auto)
{
  return std::get<T>(std::forward<decltype(t)>(t));
};

} // namespace nostd
OPENTELEMETRY_END_NAMESPACE
#endif
