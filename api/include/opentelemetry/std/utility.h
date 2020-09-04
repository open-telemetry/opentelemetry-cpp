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

#include <cstddef>
#include <utility>

OPENTELEMETRY_BEGIN_NAMESPACE
// Standard Type aliases in nostd namespace
namespace nostd
{

//
// Backport of std::data
//
// See https://en.cppreference.com/w/cpp/iterator/data
//
template <class C>
auto data(C &c) noexcept(noexcept(c.data())) -> decltype(c.data())
{
  return c.data();
}

template <class C>
auto data(const C &c) noexcept(noexcept(c.data())) -> decltype(c.data())
{
  return c.data();
}

template <class T, std::size_t N>
T *data(T (&array)[N]) noexcept
{
  return array;
}

template <class E>
const E *data(std::initializer_list<E> list) noexcept
{
  return list.begin();
}

//
// Backport of std::size
//
// See https://en.cppreference.com/w/cpp/iterator/size
//
template <class C>
auto size(const C &c) noexcept(noexcept(c.size())) -> decltype(c.size())
{
  return c.size();
}

template <class T, std::size_t N>
std::size_t size(T (&array)[N]) noexcept
{
  return N;
}

template <std::size_t N>
using make_index_sequence = std::make_index_sequence<N>;

template <std::size_t... Ints>
using index_sequence = std::index_sequence<Ints...>;

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
