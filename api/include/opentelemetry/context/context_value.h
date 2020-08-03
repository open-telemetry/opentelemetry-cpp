#pragma once

#include <cstdint>

#include "opentelemetry/nostd/nostd.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
using ContextValue =
    nostd::variant<bool, int64_t, uint64_t, double, nostd::shared_ptr<trace::SpanContext>>;
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
