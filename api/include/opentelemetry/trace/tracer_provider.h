#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/tracer.h"

namespace opentelemetry
{
namespace trace
{
class TracerProvider
{
public:
  virtual ~TracerProvider()                                                 = default;
  virtual Tracer *const GetTracer(nostd::string_view library_name,
                                  nostd::string_view library_version,
                                  const TracerOptions &tracer_options = {}) = 0;
};
}  // namespace trace
}  // namespace opentelemetry
