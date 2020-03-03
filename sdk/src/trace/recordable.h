#pragma once

#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/canonical_code.h"

namespace opentelemetry
{
namespace sdk {
namespace trace
{
namespace trace_api = opentelemetry::trace;

class Recordable
{
public:
  virtual ~Recordable() = default;

  virtual void AddEvent(nostd::string_view name,
                        std::chrono::nanoseconds time_since_epoch) noexcept = 0;

  virtual void SetStatus(trace_api::CanonicalCode code,
                         nostd::string_view description) noexcept = 0;

  virtual void SetName(nostd::string_view name) noexcept = 0;
};
}  // namespace trace
} // namespace sdk
}  // namespace opentelemetry
