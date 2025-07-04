// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdexcept>
#include <string>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class UnsupportedException : public std::runtime_error
{
public:
  UnsupportedException(const std::string &msg) : std::runtime_error(msg) {}
  UnsupportedException(UnsupportedException &&)                      = default;
  UnsupportedException(const UnsupportedException &)                 = default;
  UnsupportedException &operator=(UnsupportedException &&)           = default;
  UnsupportedException &operator=(const UnsupportedException &other) = default;
  ~UnsupportedException() override                                   = default;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
