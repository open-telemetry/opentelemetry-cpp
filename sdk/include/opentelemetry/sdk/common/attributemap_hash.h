// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iostream>
#include <string>
#include "opentelemetry/sdk/common/attribute_utils.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{

template <class T>
inline void GetHashForAttributeValue(size_t &seed, const T arg)
{
  std::hash<T> hasher;
  // reference -
  // https://www.boost.org/doc/libs/1_37_0/doc/html/hash/reference.html#boost.hash_combine
  seed ^= hasher(arg) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <class T>
inline void GetHashForAttributeValue(size_t &seed, const std::vector<T> &arg)
{
  for (auto v : arg)
  {
    GetHashForAttributeValue<T>(seed, v);
  }
}

struct GetHashForAttributeValueVisitor
{
  GetHashForAttributeValueVisitor(size_t &seed) : seed_(seed) {}
  template <class T>
  void operator()(T &v)
  {
    GetHashForAttributeValue(seed_, v);
  }
  size_t &seed_;
};

// Calculate hash of keys and values of attribute map
inline size_t GetHashForAttributeMap(const OrderedAttributeMap &attribute_map)
{
  size_t seed = 0UL;
  for (auto &kv : attribute_map)
  {
    std::hash<std::string> hasher;
    // reference -
    // https://www.boost.org/doc/libs/1_37_0/doc/html/hash/reference.html#boost.hash_combine
    seed ^= hasher(kv.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    nostd::visit(GetHashForAttributeValueVisitor(seed), kv.second);
  }
  return seed;
}

}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE