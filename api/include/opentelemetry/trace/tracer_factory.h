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
  virtual Tracer *const getTracer(const string_view &libraryName,
                                  const string_view &libraryVersion) = 0;
};
}  // namespace trace
}  // namespace opentelemetry
