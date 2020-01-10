#include "opentelemetry/sdk/trace/tracer_factory.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{

Tracer *const TracerFactory::GetTracer(string_view library_name,
                                       string_view library_version)
{
  tracers_.emplace_back(new Tracer(library_name, library_version));
  return tracers_.back().get();
}

}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
