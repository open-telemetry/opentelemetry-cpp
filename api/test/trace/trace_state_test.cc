#include "opentelemetry/trace/trace_state.h"

#include <gtest/gtest.h>

namespace
{

using opentelemetry::trace::TraceState;

TEST(TraceStateTest, DefaultConstruction)
{
  TraceState s;
  EXPECT_FALSE(s.Get("key", nullptr));
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(0, s.entries().size());
}
}  // namespace
