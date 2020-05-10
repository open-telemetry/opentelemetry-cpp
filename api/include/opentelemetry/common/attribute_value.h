#pragma once

#include <cstdint>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
using AttributeValue = nostd::variant<bool,
                                      int,
                                      int64_t,
                                      unsigned int,
                                      uint64_t,
                                      double,
                                      nostd::string_view,
                                      nostd::span<const bool>,
                                      nostd::span<const int>,
                                      nostd::span<const int64_t>,
                                      nostd::span<const unsigned int>,
                                      nostd::span<const uint64_t>,
                                      nostd::span<const nostd::string_view>>;
}  // namespace common
OPENTELEMETRY_END_NAMESPACE
