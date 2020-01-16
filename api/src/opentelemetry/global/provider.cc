#include "opentelemetry/global/provider.h"
#include "opentelemetry/trace/noop.h"

using opentelemetry::trace::NoopTracer;
using opentelemetry::trace::Tracer;
using opentelemetry::trace::TracerProvider;

namespace opentelemetry
{
namespace global
{

class DefaultTracerProvider final : public TracerProvider
{
public:
  explicit DefaultTracerProvider() : tracer_{new NoopTracer()} {}

  NoopTracer *const GetTracer(string_view library_name, string_view library_version = "") override
  {
    return tracer_;
  }

private:
  NoopTracer *const tracer_;
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
