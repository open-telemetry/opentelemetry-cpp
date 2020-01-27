#pragma once

#include "opentelemetry/core/timestamp.h"

namespace opentelemetry
{
namespace trace
{
/**
 * StartSpanOptions provides options to set properties of span at the time of starting a new span.
 */
struct StartSpanOptions
{
  // Sets the start time of a span.
  //
  // If the start time of a span is set, timestamps from both the system clock and steady clock
  // should be provided.
  //
  // Timestamps from the steady clock can be used to most accurately measure a span's
  // duration, while timestamps from the system clock can be used to most accurately place a span's
  // time point relative to other spans collected across a distributed system.
  core::SystemTimestamp start_system_time;
  core::SteadyTimestamp start_steady_time;
};

class Tracer;

/**
 * A span represents a single operation within a trace.
 */
class Span
{
public:
  virtual ~Span() = default;

  virtual Tracer &tracer() const noexcept = 0;
};
}  // namespace trace
}  // namespace opentelemetry
