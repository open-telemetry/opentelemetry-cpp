#include "opentelemetry/sdk/trace/tracer_factory.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{

Tracer *const TracerFactory::getTracer(const string_view &libraryName,
                                       const string_view &libraryVersion)
{
  tracers.emplace_back(new Tracer(libraryName, libraryVersion));
  return tracers.back().get();
}

}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
