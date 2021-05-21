/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/tracer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
class DefaultTracer : public Tracer
{
public:
  ~DefaultTracer() = default;

  /**
   * Starts a span.
   *
   * Optionally sets attributes at Span creation from the given key/value pairs.
   *
   * Attributes will be processed in order, previous attributes with the same
   * key will be overwritten.
   */
  nostd::unique_ptr<Span> StartSpan(nostd::string_view name,
                                    const common::KeyValueIterable &attributes,
                                    const StartSpanOptions &options = {}) override noexcept
  {
    return nostd::unique_ptr<Span>(new DefaultSpan::GetInvalid());
  }

  void ForceFlushWithMicroseconds(uint64_t timeout) override noexcept {}

  void CloseWithMicroseconds(uint64_t timeout) override noexcept {}
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
