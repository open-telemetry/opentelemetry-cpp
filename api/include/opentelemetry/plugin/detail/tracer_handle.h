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
};
}  // namespace plugin
OPENTELEMETRY_END_NAMESPACE
