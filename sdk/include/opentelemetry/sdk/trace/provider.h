// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

/**
 * Changes the singleton global TracerProvider.
 */
class OPENTELEMETRY_EXPORT_TYPE Provider
{
public:
  /**
   * Changes the singleton TracerProvider.
   */
  static void SetTracerProvider(
      const nostd::shared_ptr<opentelemetry::trace::TracerProvider> &tp) noexcept;
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
