// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace plugin
{
/**
 * Manage the ownership of a dynamically loaded tracer.
 */
class TracerHandle
{
public:
  virtual ~TracerHandle() = default;

  virtual trace::Tracer &tracer() const noexcept = 0;
};
}  // namespace plugin
OPENTELEMETRY_END_NAMESPACE
