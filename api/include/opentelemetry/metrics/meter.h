#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/version.h"

#include <chrono>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace meter
{

class Meter
{
public:
  virtual ~Meter() = default;
  Meter() = default;
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
