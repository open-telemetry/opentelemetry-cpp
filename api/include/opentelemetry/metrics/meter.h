#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/version.h"

#include <chrono>

/*
  Stubbed out Meter Class. Will replace with other PR implementation
*/

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

class Meter
{
public:
  virtual ~Meter() = default;
  Meter()          = default;
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
