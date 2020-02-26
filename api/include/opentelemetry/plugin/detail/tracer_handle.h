#pragma once

#include "opentelemetry/trace/tracer.h"

namespace opentelemetry
{
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
}  // namespace opentelemetry
