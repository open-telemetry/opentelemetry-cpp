#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/nostd/shared_ptr.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
class TracerProvider
{
public:
  virtual ~TracerProvider()                                                = default;
  virtual nostd::shared_ptr<Tracer> GetTracer(nostd::string_view library_name,
                                          nostd::string_view library_version = "") = 0;
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
