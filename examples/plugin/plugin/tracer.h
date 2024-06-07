// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/trace/tracer.h"

class Tracer final : public opentelemetry::trace::Tracer,
                     public std::enable_shared_from_this<Tracer>
{
public:
  explicit Tracer(opentelemetry::nostd::string_view output);

  // opentelemetry::trace::Tracer
  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span> StartSpan(
      opentelemetry::nostd::string_view name,
      const opentelemetry::common::KeyValueIterable & /*attributes*/,
      const opentelemetry::trace::SpanContextKeyValueIterable & /*links*/,
      const opentelemetry::trace::StartSpanOptions & /*options */) noexcept override;

#if OPENTELEMETRY_ABI_VERSION_NO == 1

  void ForceFlushWithMicroseconds(uint64_t /*timeout*/) noexcept override {}

  void CloseWithMicroseconds(uint64_t /*timeout*/) noexcept override {}

#endif /* OPENTELEMETRY_ABI_VERSION_NO */
};
