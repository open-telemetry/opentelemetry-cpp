#pragma once

#include "opentelemetry/trace/tracer_provider.h"

using opentelemetry::trace::TracerProvider;

namespace opentelemetry
{
namespace global
{
class Provider
{
public:
  static TracerProvider *GetTracerProvider();
  static void SetTracerProvider(TracerProvider *);

private:
  static TracerProvider *tracer_provider_;
};
}  // namespace global
}  // namespace opentelemetry
