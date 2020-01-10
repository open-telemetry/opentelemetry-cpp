#pragma once

#include "opentelemetry/trace/tracer_factory.h"

using opentelemetry::trace::TracerFactory;

namespace opentelemetry
{
namespace global
{

class Factory
{
public:
  static TracerFactory *GetTracerFactory();
  static void SetTracerFactory(TracerFactory *);

private:
  static TracerFactory *tracer_factory_;
};

}  // namespace global
}  // namespace opentelemetry
