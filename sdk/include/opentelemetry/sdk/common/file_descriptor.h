#pragma once

#include <cstdint>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk {
namespace common {
#ifdef _WIN32
using FileDescriptor = intptr_t;
#else
using FileDescriptor = int;
#endif
} // namespace common
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE
