#pragma once

#include <cstdint>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
using AttributeValue =
    nostd::variant<int, int64_t, unsigned int, uint64_t, double, nostd::string_view>;
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
