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
  Tracer *const getTracer(const string_view &);
  Tracer *const getTracer(const string_view &, const string_view &);

private:
  std::vector<std::unique_ptr<Tracer>> tracers;
};

}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
