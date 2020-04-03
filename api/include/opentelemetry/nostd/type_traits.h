#pragma once

#include <type_traits>
#include <array>

#include "opentelemetry/nostd/detail/void.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
/**
 * Back port of std::enable_if_t
 */
template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::value;

/**
 * Back port of std::remove_const_t
 */
template <typename T>
using remove_const_t = typename std::remove_const<T>::type;

/**
 * Back port of std::remove_reference_t
 */
template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

/**
 * Back port of std::remove_cvref_t
 */
template <typename T>
using remove_cvref_t = typename std::remove_cv<remove_reference_t<T>>::type;

/**
 * Back port of std::remove_all_extents
 */
template <typename T>
struct remove_all_extents
{
  using type = T;
};

template <typename T, std::size_t N>
struct remove_all_extents<std::array<T, N>> : remove_all_extents<T>
{};

/**
 * Back port of std::remove_all_extents_t
 */
template <typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
