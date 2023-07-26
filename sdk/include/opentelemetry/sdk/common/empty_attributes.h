// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <array>
#include <string>
#include <utility>

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/common/macros.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

// [[maybe_used]] is not supported before C++17
#if defined(_MSVC_LANG) && _MSVC_LANG >= 201703L

#pragma warning(push)
#pragma warning(disable: 4505) // warning C4505: 'function': unreferenced local function has been removed

#endif

/**
 * Maintain a static empty array of pairs that represents empty (default) attributes.
 * This helps to avoid constructing a new empty container every time a call is made
 * with default attributes.
 */
OPENTELEMETRY_MAYBE_UNUSED static const opentelemetry::common::KeyValueIterableView<
    std::array<std::pair<std::string, int>, 0>>
    &GetEmptyAttributes() noexcept
{
  static const std::array<std::pair<std::string, int>, 0> array{};
  static const opentelemetry::common::KeyValueIterableView<
      std::array<std::pair<std::string, int>, 0>>
      kEmptyAttributes(array);

  return kEmptyAttributes;
}

#if defined(_MSVC_LANG) && _MSVC_LANG >= 201703L

#pragma warning(pop)

#endif

}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
