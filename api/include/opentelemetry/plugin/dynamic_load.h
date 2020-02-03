#pragma once

#ifdef WIN32
#  include "opentelemetry/plugin/detail/dynamic_load_windows.h"
#else
#  include "opentelemetry/plugin/detail/dynamic_load_unix.h"
#endif

namespace opentelemetry
{
namespace plugin
{}  // namespace plugin
}  // namespace opentelemetry
