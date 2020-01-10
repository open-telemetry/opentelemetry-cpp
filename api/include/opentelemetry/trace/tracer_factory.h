#pragma once

#include "opentelemetry/nostd/string_view.h"

#include "./tracer.h"

using opentelemetry::nostd::string_view;

namespace opentelemetry
{
namespace trace
{
class TracerFactory
{
public:
  virtual Tracer *const GetTracer(string_view library_name, string_view library_version) = 0;
};
}  // namespace trace
}  // namespace opentelemetry
