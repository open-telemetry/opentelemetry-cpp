#pragma once

#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/canonical_code.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

/**
 * Maintains a representation of a span in a format that can be processed by a recorder.
 */
class Recordable
{
public:
  virtual ~Recordable() = default;

  /**
   * Add an event to a span.
   * @param name the name of the event
   * @param timestamp the timestamp of the event
   */
  virtual void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept = 0;

  /**
   * Set the status of the span.
   * @param code the status code
   * @param description a description of the status
   */
  virtual void SetStatus(trace_api::CanonicalCode code,
                         nostd::string_view description) noexcept = 0;

  /**
   * Set the name of the span.
   * @param name the name to set
   */
  virtual void SetName(nostd::string_view name) noexcept = 0;
};
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
