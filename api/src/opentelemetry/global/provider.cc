#include "opentelemetry/global/provider.h"

using opentelemetry::trace::Tracer;
using opentelemetry::trace::TracerProvider;

namespace opentelemetry
{
namespace global
{
class DefaultTracerProvider : public TracerProvider
{
  Tracer *const GetTracer(string_view library_name, string_view library_version = "") override
  {
    // TODO: return a no-op trace
    return nullptr;
  }
};

TracerProvider *Provider::tracer_provider_ = nullptr;

TracerProvider *Provider::GetTracerProvider()
{
  if (!Provider::tracer_provider_)
  {
    Provider::tracer_provider_ = new DefaultTracerProvider();
  }
  return Provider::tracer_provider_;
}

void Provider::SetTracerProvider(TracerProvider *tf)
{
  if (Provider::tracer_provider_)
  {
    delete Provider::tracer_provider_;
  }

  Provider::tracer_provider_ = tf;
}
}  // namespace global
}  // namespace opentelemetry
