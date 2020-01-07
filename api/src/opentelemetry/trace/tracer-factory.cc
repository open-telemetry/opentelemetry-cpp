#include "opentelemetry/trace/tracer-factory.h"

namespace opentelemetry
{
namespace trace
{
TracerFactory* TracerFactory::instance = nullptr;
TracerFactory::TracerFactory() {}

TracerFactory *TracerFactory::getInstance()
{
  if (!TracerFactory::instance)
  {
    TracerFactory::instance = new TracerFactory();
  }
  return TracerFactory::instance;
}

Tracer* const TracerFactory::getTracer(const string_view& name)
{
  return getTracer(name, "");
}

Tracer* const TracerFactory::getTracer(const string_view& name, const string_view& version)
{
  tracers.emplace_back(new Tracer(
    name,
    version
  ));
  return tracers.back().get();
}
} // namespace trace
} // namespace opentelemetry
