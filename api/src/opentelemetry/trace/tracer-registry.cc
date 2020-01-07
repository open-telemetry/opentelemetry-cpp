#include "opentelemetry/trace/tracer-registry.h"

namespace opentelemetry
{
namespace trace
{
TracerRegistry* TracerRegistry::instance = nullptr;
TracerRegistry::TracerRegistry() {}

TracerRegistry *TracerRegistry::getInstance()
{
  if (!TracerRegistry::instance)
  {
    TracerRegistry::instance = new TracerRegistry();
  }
  return TracerRegistry::instance;
}
Tracer* const TracerRegistry::get(const std::string& name)
{
  return get(name, "");
}

Tracer* const TracerRegistry::get(const std::string& name, const std::string& version)
{
  auto key = name + ":" + version;
  if (!tracers[key])
  {
    tracers[key] = std::unique_ptr<Tracer>(new Tracer());
  }
  return tracers[key].get();
}
} // namespace trace
} // namespace opentelemetry
