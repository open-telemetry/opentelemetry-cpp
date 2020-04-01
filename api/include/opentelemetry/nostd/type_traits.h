#pragma once

#include <array>

#include "opentelemetry/nostd/detail/void.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
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
