// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/string_view.h"

/** DJB2 hash function below is near-perfect hash used by several systems.
 * Ref. http://www.cse.yorku.ca/~oz/hash.html
 * </summary>
 * <param name="str">String to hash</param>
 * <param name="h">Initial offset</param>
 * <returns>32 bit code</returns>
 */
constexpr uint32_t hashCode(const char *str, uint32_t h = 0)
{
  return (uint32_t)(!str[h] ? 5381 : ((uint32_t)hashCode(str, h + 1) * (uint32_t)33) ^ str[h]);
}
#define OTEL_CPP_CONST_UINT32_T(x) std::integral_constant<uint32_t, (uint32_t)x>::value
#define OTEL_CPP_CONST_HASHCODE(name) OTEL_CPP_CONST_UINT32_T(hashCode(#name))

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{

class StringUtil
{
public:
  static nostd::string_view Trim(nostd::string_view str, size_t left, size_t right)
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

  static nostd::string_view Trim(nostd::string_view str)
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
