#include "opentelemetry/trace/span_context.h"

#include <gtest/gtest.h>

namespace
{

using opentelemetry::trace::SpanContext;

TEST(SpanContextTest, DefaultConstruction)
{
  SpanContext ctx;
}

}  // namespace
