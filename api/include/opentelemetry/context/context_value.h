#pragma once

#include <cstdint>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
using ContextValue = nostd::variant<nostd::monostate,
                                    bool,
                                    int64_t,
                                    uint64_t,
                                    double,
                                    nostd::shared_ptr<trace::Span>,
                                    nostd::shared_ptr<trace::SpanContext>>;
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
