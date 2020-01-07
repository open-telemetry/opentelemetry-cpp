#pragma once

#include <vector>
#include <memory>
#include <string>

#include "opentelemetry/nostd/string_view.h"

#include "./tracer.h"

using opentelemetry::nostd::string_view;

namespace opentelemetry
{
namespace trace
{
class Tracer;
class TracerFactory
{
public:
  static TracerFactory* getInstance();
  Tracer* const getTracer(const string_view&);
  Tracer* const getTracer(const string_view&, const string_view&);

private:
  TracerFactory();
  static TracerFactory* instance;
  std::vector<std::unique_ptr<Tracer>> tracers;
};
}  // namespace trace
}  // namespace opentelemetry
