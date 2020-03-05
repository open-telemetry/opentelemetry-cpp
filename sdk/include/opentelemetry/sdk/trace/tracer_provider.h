#pragma once

#include <map>
#include <memory>
#include <string>

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
                          nostd::string_view library_version = "") override;

private:
  std::map<std::pair<std::string, std::string>, std::shared_ptr<Tracer>> tracers_;
};
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
