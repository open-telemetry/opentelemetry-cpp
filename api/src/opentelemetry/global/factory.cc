#include "opentelemetry/global/factory.h"
#include "opentelemetry/trace/tracer.h"

using opentelemetry::trace::TracerFactory;

namespace opentelemetry
{
namespace global
{

using opentelemetry::trace::Tracer;
class DefaultTracerFactory : public TracerFactory
{
  Tracer* const getTracer(const string_view& name)
  {
    return getTracer(name, "");
  }

  Tracer* const getTracer(const string_view& name, const string_view& version)
  {
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
