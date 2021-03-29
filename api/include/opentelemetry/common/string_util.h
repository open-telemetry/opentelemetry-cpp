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
namespace common
{

class StringUtil
{
public:
  static nostd::string_view Trim(nostd::string_view str, size_t left, size_t right)
  {
    while (str[static_cast<std::size_t>(right)] == ' ' && left < right)
    {
      right--;
    }
    while (str[static_cast<std::size_t>(left)] == ' ' && left < right)
    {
      left++;
    }
    return str.substr(left, right - left + 1);
  }
};

}  // namespace common

OPENTELEMETRY_END_NAMESPACE
