#pragma once

#include <memory>
#include <vector>

#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/trace/tracer_provider.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
class TracerProvider : public opentelemetry::trace::TracerProvider
{
public:
  Tracer *const GetTracer(nostd::string_view library_name,
                          nostd::string_view library_version                        = "",
                          const opentelemetry::trace::TracerOptions &tracer_options = {}) override;

private:
  std::vector<std::shared_ptr<Tracer>> tracers_;
};
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
