#include "opentelemetry/sdk/trace/tracer-factory.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{

Tracer *const TracerFactory::getTracer(const string_view &name)
{
  return getTracer(name, "");
}

Tracer *const TracerFactory::getTracer(const string_view &name, const string_view &version)
{
  tracers.emplace_back(new Tracer(name, version));
  return tracers.back().get();
}

}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
