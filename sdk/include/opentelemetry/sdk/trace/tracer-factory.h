#pragma once

#include <vector>
#include <memory>

#include "opentelemetry/trace/tracer-factory.h"

using opentelemetry::trace::Tracer;
using opentelemetry::nostd::string_view;

namespace opentelemetry
{
namespace sdk
{
namespace trace
{

class TracerFactory : public opentelemetry::trace::TracerFactory
{
public:
  Tracer* const getTracer(const string_view&);
  Tracer* const getTracer(const string_view&, const string_view&);
private:
  std::vector<std::unique_ptr<Tracer>> tracers;
};

}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
