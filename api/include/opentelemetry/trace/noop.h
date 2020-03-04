#pragma once
// Please refer to provider.h for documentation on how to obtain a Tracer object.
//
// This file is part of the internal implementation of OpenTelemetry. Nothing in this file should be
// used directly. Please refer to span.h and tracer.h for documentation on these interfaces.

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/tracer.h"

#include <memory>

namespace opentelemetry
{
namespace trace
{
/**
 * No-op implementation of Span. This class should not be used directly.
 */
class NoopSpan final : public Span
{
public:
  explicit NoopSpan(const std::shared_ptr<Tracer> &tracer) noexcept : tracer_{tracer} {}

  void AddEvent(nostd::string_view name) noexcept override {}

  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept override {}
  void AddEvent(nostd::string_view name, core::SteadyTimestamp timestamp) noexcept override {}

  void SetStatus(CanonicalCode code, nostd::string_view description) noexcept override {}

  void UpdateName(nostd::string_view name) noexcept override {}

  void End() noexcept override {}

  bool IsRecording() const noexcept override { return false; }

  Tracer &tracer() const noexcept override { return *tracer_; }

private:
  std::shared_ptr<Tracer> tracer_;
};

/**
 * No-op implementation of Tracer.
 */
class NoopTracer final : public Tracer, public std::enable_shared_from_this<NoopTracer>
{
public:
  // Tracer
  nostd::unique_ptr<Span> StartSpan(nostd::string_view /*name*/,
                                    const StartSpanOptions & /*options*/) noexcept override
  {
    return nostd::unique_ptr<Span>{new (std::nothrow) NoopSpan{this->shared_from_this()}};
  }

  bool FlushWithMicroseconds(uint64_t /*timeout*/) noexcept override { return true; }

  bool CloseWithMicroseconds(uint64_t /*timeout*/) noexcept override { return true; }
};
}  // namespace trace
}  // namespace opentelemetry
