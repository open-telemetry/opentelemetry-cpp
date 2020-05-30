#pragma once

#include <opentelemetry/version.h>

#ifndef HAVE_STDLIB_CPP
/* Private implementation of standard C++20 types */
#include "opentelemetry/nostd/stl/span.h"
#include "opentelemetry/nostd/stl/string_view.h"
#include "opentelemetry/nostd/stl/variant.h"
#include "opentelemetry/nostd/stl/utility.h"
#else
/* Standard library implementation */
#include <cstddef>
#include <variant>
#include <memory>
#include <string_view>
#include <utility>

#ifdef HAVE_GSL
#include <gsl/gsl>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
    template <class ElementType, std::size_t Extent = gsl::dynamic_extent>
    using span = gsl::span<ElementType, Extent>;
}
OPENTELEMETRY_END_NAMESPACE
#else

#include <span>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
    template <class ElementType, std::size_t Extent = std::dynamic_extent>
    using span = std::span<ElementType, Extent>;
}
OPENTELEMETRY_END_NAMESPACE
#endif
OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
    template <class... _Types>
    using variant = std::variant<_Types...>;

    using string_view = std::string_view;

    template <class C>
    constexpr auto size(const C &c) -> decltype(c.size());

    template <class T, std::size_t N>
    constexpr std::size_t size(const T (&array)[N]) noexcept;

    template< bool B, class T = void >
    using enable_if_t = typename std::enable_if<B,T>::type;

    template <class... _Types>
    using unique_ptr = std::unique_ptr<_Types...>;

    template <class... _Types>
    using shared_ptr = std::shared_ptr<_Types...>;

    }  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
#endif
