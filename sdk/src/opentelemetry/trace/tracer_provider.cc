#include "opentelemetry/sdk/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
Tracer *const TracerProvider::GetTracer(nostd::string_view library_name,
                                        nostd::string_view library_version)
{
  std::pair<std::string, std::string> key =
      std::pair<std::string, std::string>{std::string{library_name}, std::string{library_version}};

  if (auto cached = tracers_[key])
  {
    return cached.get();
  }

  std::shared_ptr<Tracer> t = std::shared_ptr<Tracer>(new Tracer());
  tracers_[key]             = t;
  return t.get();
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
