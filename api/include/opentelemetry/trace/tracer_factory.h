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
  virtual Tracer *const getTracer(const string_view &)                      = 0;
  virtual Tracer *const getTracer(const string_view &, const string_view &) = 0;
};
}  // namespace trace
}  // namespace opentelemetry
