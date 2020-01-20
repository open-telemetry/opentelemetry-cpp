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
  // If start_time is set, it should ideally provide both a system and steady timestamp.
  // Timestamps from the steady clock can be used to most accurately measure a span's duration,
  // while timestamps from the system clock can be used to most accurately place a span's time point
  // relative to spans collected from across other computers.
  core::Timestamp start_time;
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
