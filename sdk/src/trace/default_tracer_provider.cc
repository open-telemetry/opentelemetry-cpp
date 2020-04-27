#include "opentelemetry/sdk/trace/default_tracer_provider.h"
#include "src/trace/tracer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
DefaultTracerProvider::DefaultTracerProvider(std::unique_ptr<SpanProcessor> &&processor) noexcept
    : tracer_(new Tracer(std::move(processor)))
{}

opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> DefaultTracerProvider::GetTracer(
    nostd::string_view library_name,
    nostd::string_view library_version) noexcept
{
  return tracer_;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
