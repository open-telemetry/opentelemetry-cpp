#pragma once

#include <cstdint>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
using ContextValue = nostd::variant<bool, int64_t, uint64_t, double>;
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
