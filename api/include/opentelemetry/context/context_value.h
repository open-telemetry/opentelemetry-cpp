#pragma once

#include <cstdint>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"
#include "opentelemetry/trace/span_context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
using ContextValue = nostd::variant<int64_t,
                                    uint64_t,
                                    double,
                                    nostd::span<trace::SpanContext>,
                                    nostd::span<nostd::unique_ptr<char[]>>>;
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
