#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;

TEST(TracerProvider, GetTracer)
{
  std::shared_ptr<SpanProcessor> processor(new SimpleSpanProcessor(nullptr));

  TracerProvider tf(processor);
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
  auto sdkTracer = dynamic_cast<Tracer *>(t1.get());
  ASSERT_NE(nullptr, sdkTracer);
  ASSERT_EQ(processor, sdkTracer->GetProcessor());
}
