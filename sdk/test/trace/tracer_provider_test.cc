// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::sdk::resource;

#include <iostream>

TEST(TracerProvider, GetTracer)
{
  std::unique_ptr<SpanProcessor> processor(new SimpleSpanProcessor(nullptr));
  std::vector<std::unique_ptr<SpanProcessor>> processors;
  processors.push_back(std::move(processor));
  TracerProvider tp1(std::make_shared<TracerContext>(std::move(processors), Resource::Create({})));
  auto t1 = tp1.GetTracer("test");
  auto t2 = tp1.GetTracer("test");
  auto t3 = tp1.GetTracer("different", "1.0.0");
  auto t4 = tp1.GetTracer("");
  auto t5 = tp1.GetTracer(opentelemetry::nostd::string_view{});
  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t2);
  ASSERT_NE(nullptr, t3);

  // Should return the same instance each time.
  ASSERT_EQ(t1, t2);
  ASSERT_NE(t1, t3);
  ASSERT_EQ(t4, t5);

  // Should be an sdk::trace::Tracer with the processor attached.
  auto sdkTracer1 = dynamic_cast<Tracer *>(t1.get());
  ASSERT_NE(nullptr, sdkTracer1);
  ASSERT_EQ("AlwaysOnSampler", sdkTracer1->GetSampler().GetDescription());
  std::unique_ptr<SpanProcessor> processor2(new SimpleSpanProcessor(nullptr));
  std::vector<std::unique_ptr<SpanProcessor>> processors2;
  processors2.push_back(std::move(processor2));
  TracerProvider tp2(
      std::make_shared<TracerContext>(std::move(processors2), Resource::Create({}),
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
  std::unique_ptr<SpanProcessor> processor(new SimpleSpanProcessor(nullptr));
  std::vector<std::unique_ptr<SpanProcessor>> processors;
  processors.push_back(std::move(processor));

  TracerProvider tp1(std::make_shared<TracerContext>(std::move(processors)));

  EXPECT_TRUE(tp1.Shutdown());
}

TEST(TracerProvider, ForceFlush)
{
  std::unique_ptr<SpanProcessor> processor1(new SimpleSpanProcessor(nullptr));

  TracerProvider tp1(std::move(processor1));

  EXPECT_TRUE(tp1.ForceFlush());
}
