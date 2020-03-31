// MPark.Variant
//
// Copyright Michael Park, 2015-2017
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#include "opentelemetry/version.h"

#include <cstddef>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{

  struct in_place_t { explicit in_place_t() = default; };

  template <std::size_t I>
  struct in_place_index_t { explicit in_place_index_t() = default; };

  template <typename T>
  struct in_place_type_t { explicit in_place_type_t() = default; };

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
