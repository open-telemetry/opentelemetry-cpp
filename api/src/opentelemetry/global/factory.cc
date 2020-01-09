#include "opentelemetry/global/factory.h"

using opentelemetry::trace::TracerFactory;
using opentelemetry::trace::Tracer;

namespace opentelemetry
{
namespace global
{

class DefaultTracerFactory : public TracerFactory
{
  Tracer* const getTracer(const string_view& name)
  {
    return getTracer(name, "");
  }

  Tracer* const getTracer(const string_view& name, const string_view& version)
  {
    // TODO: return a no-op tracer
    return nullptr;
  }
};

TracerFactory* Factory::tracerFactory = nullptr;

TracerFactory* Factory::getTracerFactory()
{
  if (!Factory::tracerFactory)
  {
    Factory::tracerFactory = new DefaultTracerFactory();
  }
  return Factory::tracerFactory;
}

void Factory::setTracerFactory(TracerFactory* tf)
{
  Factory::tracerFactory = tf;
}

} // namespace global
} // namespace opentelemetry
