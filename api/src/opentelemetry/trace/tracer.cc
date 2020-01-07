#include "opentelemetry/trace/tracer.h"

namespace opentelemetry
{
namespace trace
{
Tracer::Tracer(const string_view& name, const string_view& version)
    : name(name), version(version) {}
}  // namespace trace
}  // namespace opentelemetry
