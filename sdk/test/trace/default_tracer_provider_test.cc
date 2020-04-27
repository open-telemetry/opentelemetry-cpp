#include "opentelemetry/sdk/trace/default_tracer_provider.h"
#include "src/trace/simple_processor.h"
#include "src/trace/tracer.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;

TEST(TracerProvider, GetTracer)
{
  SimpleSpanProcessor* processor_ptr = new SimpleSpanProcessor(nullptr);
  std::unique_ptr<SpanProcessor> processor(processor_ptr);
  
  auto tf = DefaultTracerProvider(std::move(processor));
  auto t1 = tf.GetTracer("test");
  auto t2 = tf.GetTracer("test");
  auto t3 = tf.GetTracer("different", "1.0.0");
  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t2);
  ASSERT_NE(nullptr, t3);

  // Should return the same instance each time.
  ASSERT_EQ(t1, t2);
  ASSERT_EQ(t1, t3);

  // Should be an sdk::trace::Tracer with the processor attached.
  auto sdkTracer = static_cast<Tracer*>(t1.get());
  ASSERT_EQ(processor_ptr, &sdkTracer->processor());
}
