#pragma once

#include <random>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
/**
 * @return a seeded thread-local random number generator.
 */
std::mt19937_64 &GetRandomNumberGenerator() noexcept;
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
