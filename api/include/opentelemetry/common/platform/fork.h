#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace api
{
namespace common
{
namespace platform
{
/**
 * Portable wrapper for pthread_atfork.
 * See
 * https://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_atfork.html
 */
int AtFork(void (*prepare)(), void (*parent)(), void (*child)()) noexcept;
}  // namespace platform
}  // namespace common
}  // namespace api
OPENTELEMETRY_END_NAMESPACE
