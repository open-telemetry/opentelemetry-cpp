// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace plugin
{
/**
 * Manage the ownership of a dynamically loaded library.
 */
class OPENTELEMETRY_API DynamicLibraryHandle
{
public:
  virtual ~DynamicLibraryHandle() = default;
};
}  // namespace plugin
OPENTELEMETRY_END_NAMESPACE
