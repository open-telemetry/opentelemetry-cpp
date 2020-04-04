#pragma once

#include <random>

#include "opentelemetry/version.h"
#include "src/common/fast_random_number_generator.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
/**
 * @return a seeded thread-local random number generator.
 */
FastRandomNumberGenerator &GetRandomNumberGenerator() noexcept;
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
