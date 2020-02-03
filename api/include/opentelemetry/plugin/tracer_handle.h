#pragma once

#include "opentelemetry/trace/tracer.h"

namespace opentelemetry
{
namespace plugin
{
class TracerHandle
{
public:
  virtual ~TracerHandle() = default;

  virtual trace::Tracer &tracer() const noexcept = 0;
};
}  // namespace plugin
}  // namespace opentelemetry
