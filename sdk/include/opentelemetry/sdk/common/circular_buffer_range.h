// Copyright 2021, OpenTelemetry Authors
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

#include <cassert>
#include <iterator>
#include <type_traits>
#include <utility>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
/**
 * A non-owning view into a range of elements in a circular buffer.
 */
template <class T>
class CircularBufferRange
{
public:
  CircularBufferRange() noexcept = default;

  explicit CircularBufferRange(nostd::span<T> first) noexcept : first_{first} {}

  CircularBufferRange(nostd::span<T> first, nostd::span<T> second) noexcept
      : first_{first}, second_{second}
  {}

  operator CircularBufferRange<const T>() const noexcept { return {first_, second_}; }

  /**
   * Iterate over the elements in the range.
   * @param callback the callback to call for each element
   * @return true if we iterated over all elements
   */
  template <class Callback>
  bool ForEach(Callback callback) const
      noexcept(noexcept(std::declval<Callback>()(std::declval<T &>())))
  {
    for (auto &value : first_)
    {
      if (!callback(value))
      {
        return false;
      }
    }
    for (auto &value : second_)
    {
      if (!callback(value))
      {
        return false;
      }
    }
    return true;
  }

  /**
   * @return the number of elements in the range
   */
  size_t size() const noexcept { return first_.size() + second_.size(); }

  /**
   * @return true if the range is empty
   */
  bool empty() const noexcept { return first_.empty(); }

  /**
   * Return a subrange taken from the start of this range.
   * @param n the number of element to take in the subrange
   * @return a subrange of the first n elements in this range
   */
  CircularBufferRange Take(size_t n) const noexcept
  {
    assert(n <= size());
    if (first_.size() >= n)
    {
      return CircularBufferRange{nostd::span<T>{first_.data(), n}};
    }
    return {first_, nostd::span<T>{second_.data(), n - first_.size()}};
  }

private:
  nostd::span<T> first_;
  nostd::span<T> second_;
};
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
