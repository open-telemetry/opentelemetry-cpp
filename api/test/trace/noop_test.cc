#include "opentelemetry/trace/noop.h"

#include <memory>

#include <gtest/gtest.h>

using opentelemetry::trace::NoopTracer;
using opentelemetry::trace::Tracer;

TEST(NoopTest, UseNoopTracers)
{
  std::shared_ptr<Tracer> tracer{new NoopTracer{}};
  auto s1 = tracer->StartSpan("abc");
  EXPECT_EQ(&s1->tracer(), tracer.get());
}
