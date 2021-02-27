#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::sdk::resource;

TEST(TracerProvider, GetTracer)
{
  std::shared_ptr<SpanProcessor> processor(new SimpleSpanProcessor(nullptr));

  TracerProvider tp1(processor, Resource::Create({}));
  auto t1 = tp1.GetTracer("test");
  auto t2 = tp1.GetTracer("test");
  auto t3 = tp1.GetTracer("different", "1.0.0");
  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t2);
  ASSERT_NE(nullptr, t3);

  // Should return the same instance each time.
  ASSERT_EQ(t1, t2);
  ASSERT_EQ(t1, t3);

  // Should be an sdk::trace::Tracer with the processor attached.
  auto sdkTracer1 = dynamic_cast<Tracer *>(t1.get());
  ASSERT_NE(nullptr, sdkTracer1);
  ASSERT_EQ(processor, sdkTracer1->GetProcessor());
  ASSERT_EQ("AlwaysOnSampler", sdkTracer1->GetSampler()->GetDescription());

  TracerProvider tp2(processor, Resource::Create({}), std::make_shared<AlwaysOffSampler>());
  auto sdkTracer2 = dynamic_cast<Tracer *>(tp2.GetTracer("test").get());
  ASSERT_EQ("AlwaysOffSampler", sdkTracer2->GetSampler()->GetDescription());
}

TEST(TracerProvider, GetSampler)
{
  std::shared_ptr<SpanProcessor> processor1(new SimpleSpanProcessor(nullptr));

  // Create a TracerProvicer with a default AlwaysOnSampler.
  TracerProvider tp1(processor1);
  auto t1 = tp1.GetSampler();
  auto t2 = tp1.GetSampler();
  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t2);

  // Should return the same sampler each time.
  ASSERT_EQ(t1, t2);

  // Should be AlwaysOnSampler
  ASSERT_EQ("AlwaysOnSampler", t2->GetDescription());

  // Create a TracerProvicer with a custom AlwaysOffSampler.
  std::shared_ptr<SpanProcessor> processor2(new SimpleSpanProcessor(nullptr));
  TracerProvider tp2(processor2, Resource::Create({}), std::make_shared<AlwaysOffSampler>());
  auto t3 = tp2.GetSampler();

  ASSERT_EQ("AlwaysOffSampler", t3->GetDescription());
}

TEST(TracerProvider, Shutdown)
{
  std::shared_ptr<SpanProcessor> processor1(new SimpleSpanProcessor(nullptr));

  TracerProvider tp1(processor1);

  EXPECT_TRUE(tp1.Shutdown());
}

TEST(TracerProvider, ForceFlush)
{
  std::shared_ptr<SpanProcessor> processor1(new SimpleSpanProcessor(nullptr));

  TracerProvider tp1(processor1);

  EXPECT_TRUE(tp1.ForceFlush());
}
