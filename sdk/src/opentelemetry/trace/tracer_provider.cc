#include "opentelemetry/sdk/trace/tracer_provider.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
Tracer *const TracerProvider::GetTracer(nostd::string_view library_name,
                                        nostd::string_view library_version)
{
  tracers_.emplace_back(new Tracer(library_name, library_version));
  return tracers_.back().get();
}
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
