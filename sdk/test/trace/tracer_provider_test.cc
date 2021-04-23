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
  std::unique_ptr<SpanProcessor> processor(new SimpleSpanProcessor(nullptr));

  TracerProvider tp1(std::make_shared<TracerContext>(std::move(processor), Resource::Create({})));
  auto t1 = tp1.GetTracer("test");
  auto t2 = tp1.GetTracer("test");
  auto t3 = tp1.GetTracer("different", "1.0.0");
  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t2);
  ASSERT_NE(nullptr, t3);

  // Should return the same instance each time.
  ASSERT_EQ(t1, t2);
  ASSERT_NE(t1, t3);

  // Should be an sdk::trace::Tracer with the processor attached.
  auto sdkTracer1 = dynamic_cast<Tracer *>(t1.get());
  ASSERT_NE(nullptr, sdkTracer1);
  ASSERT_EQ("AlwaysOnSampler", sdkTracer1->GetSampler().GetDescription());
  TracerProvider tp2(std::make_shared<TracerContext>(
      std::unique_ptr<SpanProcessor>(new SimpleSpanProcessor(nullptr)), Resource::Create({}),
      std::unique_ptr<Sampler>(new AlwaysOffSampler()),
      std::unique_ptr<IdGenerator>(new RandomIdGenerator)));
  auto sdkTracer2 = dynamic_cast<Tracer *>(tp2.GetTracer("test").get());
  ASSERT_EQ("AlwaysOffSampler", sdkTracer2->GetSampler().GetDescription());

  auto instrumentation_library1 = sdkTracer1->GetInstrumentationLibrary();
  ASSERT_EQ(instrumentation_library1.GetName(), "test");
  ASSERT_EQ(instrumentation_library1.GetVersion(), "");

  // Should be an sdk::trace::Tracer with the processor attached.
  auto sdkTracer3               = dynamic_cast<Tracer *>(t3.get());
  auto instrumentation_library3 = sdkTracer3->GetInstrumentationLibrary();
  ASSERT_EQ(instrumentation_library3.GetName(), "different");
  ASSERT_EQ(instrumentation_library3.GetVersion(), "1.0.0");
}

TEST(TracerProvider, Shutdown)
{
  std::unique_ptr<SpanProcessor> processor1(new SimpleSpanProcessor(nullptr));

  TracerProvider tp1(std::make_shared<TracerContext>(std::move(processor1)));

  EXPECT_TRUE(tp1.Shutdown());
}

TEST(TracerProvider, ForceFlush)
{
  std::unique_ptr<SpanProcessor> processor1(new SimpleSpanProcessor(nullptr));

  TracerProvider tp1(std::move(processor1));

  EXPECT_TRUE(tp1.ForceFlush());
}
