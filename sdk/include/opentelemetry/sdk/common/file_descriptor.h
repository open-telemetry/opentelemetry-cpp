#pragma once

#include <cstdint>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
#ifdef _WIN32
using FileDescriptor = intptr_t;
#else
using FileDescriptor = int;
#endif
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
