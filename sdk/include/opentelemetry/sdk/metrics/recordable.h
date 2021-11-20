// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class Recordable
{
public:
  virtual ~Recordable() = default;

  // TBD
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE;