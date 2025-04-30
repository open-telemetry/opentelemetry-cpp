// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

#if !defined(OPENTELEMETRY_STL_VERSION) || (OPENTELEMETRY_STL_VERSION < 2017)
#error OPENTELEMETRY_STL_VERSION must be >= 2017
#endif

#include <type_traits>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
/**
 * Back port of std::add_pointer_t
 */
template <class T>
using add_pointer_t = typename std::add_pointer<T>::type;

/**
 * Back port of std::enable_if_t
 */
template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

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

using std::is_nothrow_swappable;
using std::is_swappable;

/**
 * Back port of
 *  std::is_trivialy_copy_constructible
 *  std::is_trivialy_move_constructible
 *  std::is_trivialy_copy_assignable
 *  std::is_trivialy_move_assignable
 */
using std::is_trivially_copy_assignable;
using std::is_trivially_copy_constructible;
using std::is_trivially_move_assignable;
using std::is_trivially_move_constructible;
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
