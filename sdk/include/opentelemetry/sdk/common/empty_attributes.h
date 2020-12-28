/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "opentelemetry/common/key_value_iterable_view.h"

#include <array>
#include <map>
#include <string>
#include <utility>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
/**
 * Maintain a static empty array of pairs that represents empty (default) attributes.
 * This helps to avoid constructing a new empty container every time a call is made
 * with default attributes.
 */
static const opentelemetry::common::KeyValueIterableView<std::array<std::pair<std::string, int>, 0>>
    &GetEmptyAttributes() noexcept
{
  static const std::array<std::pair<std::string, int>, 0> array{};
  static const opentelemetry::common::KeyValueIterableView<
      std::array<std::pair<std::string, int>, 0>>
      kEmptyAttributes(array);

  return kEmptyAttributes;
}
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
