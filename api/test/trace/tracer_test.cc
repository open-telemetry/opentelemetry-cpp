#include "opentelemetry/trace/scope.h"
#include "opentelemetry/context/threadlocal_context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/noop.h"

#include <gtest/gtest.h>

using opentelemetry::trace::NoopSpan;
using opentelemetry::trace::Scope;
using opentelemetry::trace::Span;
namespace nostd   = opentelemetry::nostd;
namespace context = opentelemetry::context;

TEST(TracerTest, GetCurrentSpan)
{
  std::unique_ptr<Tracer> tracer(new NoopTracer());
  nostd::shared_ptr<Span> span_first(new NoopSpan(nullptr));
  nostd::shared_ptr<Span> span_second(new NoopSpan(nullptr));

  auto current = tracer->GetCurrentSpan();
  ASSERT_TRUE(current->IsInvalid());

  auto scope_first = tracer->WithCurrentSpan(span_first);
  current = tracer->GetCurrentSpan();
  ASSERT_EQ(current, span_first);  

  auto scope_second = tracer->WithCurrentSpan(span_second);
  current = tracer->GetCurrentSpan();
  ASSERT_EQ(current, span_second);  

  scope_second.reset(nullptr);
  current = tracer->GetCurrentSpan();
  ASSERT_EQ(current, span_first);  

  scope_firstt.reset(nullptr);
  current = tracer->GetCurrentSpan();
  ASSERT_TRUE(current->IsInvalid());
}
