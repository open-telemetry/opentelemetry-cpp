#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
/**
 * Back port of std::void_t
 */
template <class...>
using void_t = void;
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
