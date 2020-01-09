#pragma once

#include "opentelemetry/trace/tracer-factory.h"

using opentelemetry::trace::TracerFactory;

namespace opentelemetry
{
namespace global
{

class Factory
{
public:
  static TracerFactory* getTracerFactory();
  static void setTracerFactory(TracerFactory*);
private:
  static TracerFactory* tracerFactory;
};

}  // namespace global
}  // namespace opentelemetry
