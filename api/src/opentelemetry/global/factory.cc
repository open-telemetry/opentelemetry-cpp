#include "opentelemetry/global/factory.h"

using opentelemetry::trace::Tracer;
using opentelemetry::trace::TracerFactory;

namespace opentelemetry
{
namespace global
{

class DefaultTracerFactory : public TracerFactory
{
  Tracer *const getTracer(const string_view &libraryName, const string_view &libraryVersion = "") override
  {
    // TODO: return a no-op tracer
    return nullptr;
  }
};

TracerFactory *Factory::tracerFactory = nullptr;

TracerFactory *Factory::getTracerFactory()
{
  if (!Factory::tracerFactory)
  {
    Factory::tracerFactory = new DefaultTracerFactory();
  }
  return Factory::tracerFactory;
}

void Factory::setTracerFactory(TracerFactory *tf)
{
  Factory::tracerFactory = tf;
}

}  // namespace global
}  // namespace opentelemetry
