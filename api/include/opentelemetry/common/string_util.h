// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/string_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{

class StringUtil
{
public:
  static nostd::string_view Trim(nostd::string_view str, size_t left, size_t right) noexcept
  {
    while (str[static_cast<std::size_t>(left)] == ' ' && left <= right)
    {
      left++;
    }
    while (str[static_cast<std::size_t>(right)] == ' ' && left <= right)
    {
      right--;
    }
    return str.substr(left, 1 + right - left);
  }

  static nostd::string_view Trim(nostd::string_view str) noexcept
  {
    if (str.empty())
    {
      return str;
    }

    return Trim(str, 0, str.size() - 1);
  }
};

}  // namespace common

OPENTELEMETRY_END_NAMESPACE
