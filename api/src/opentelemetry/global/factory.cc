#include "opentelemetry/global/factory.h"

using opentelemetry::trace::Tracer;
using opentelemetry::trace::TracerFactory;

namespace opentelemetry
{
namespace global
{
class DefaultTracerFactory : public TracerFactory
{
  Tracer *const GetTracer(string_view library_name, string_view library_version = "") override
  {
    // TODO: return a no-op tracer
    return nullptr;
  }
};

TracerFactory *Factory::tracer_factory_ = nullptr;

TracerFactory *Factory::GetTracerFactory()
{
  if (!Factory::tracer_factory_)
  {
    Factory::tracer_factory_ = new DefaultTracerFactory();
  }
  return Factory::tracer_factory_;
}

void Factory::SetTracerFactory(TracerFactory *tf)
{
  if (Factory::tracer_factory_)
  {
    delete Factory::tracer_factory_;
  }

  Factory::tracer_factory_ = tf;
}
}  // namespace global
}  // namespace opentelemetry
