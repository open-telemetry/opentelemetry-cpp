#pragma once

#include <memory>
#include <vector>

#include "opentelemetry/trace/tracer_factory.h"

using opentelemetry::nostd::string_view;
using opentelemetry::trace::Tracer;

namespace opentelemetry
{
namespace sdk
{
namespace trace
{

class TracerFactory : public opentelemetry::trace::TracerFactory
{
public:
  Tracer *const GetTracer(string_view library_name, string_view library_version = "") override;

private:
  std::vector<std::unique_ptr<Tracer>> tracers_;
};

}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
