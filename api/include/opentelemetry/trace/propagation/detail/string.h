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

#include "opentelemetry/nostd/string_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{
namespace detail
{

/**
 * Splits a string by separator, up to given buffer count words.
 * Returns the amount of words the input was split into.
 */
inline size_t SplitString(nostd::string_view s,
                          char separator,
                          nostd::string_view *results,
                          size_t count)
{
  if (count == 0)
  {
    return count;
  }

  size_t filled      = 0;
  size_t token_start = 0;
  for (size_t i = 0; i < s.size(); i++)
  {
    if (s[i] != separator)
    {
      continue;
    }

    results[filled++] = s.substr(token_start, i - token_start);

    if (filled == count)
    {
      return count;
    }

    token_start = i + 1;
  }

  if (filled < count)
  {
    results[filled++] = s.substr(token_start);
  }

  return filled;
}

}  // namespace detail
}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
