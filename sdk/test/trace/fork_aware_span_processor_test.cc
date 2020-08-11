// The MIT License (MIT)

// Copyright (c) 2015-2016 LightStep

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <memory>

#include <gtest/gtest.h>
#include "opentelemetry/sdk/trace/fork_aware_span_processor.h"

using namespace opentelemetry::sdk::trace;

class DummySpanProcessor : public ForkAwareSpanProcessor
{

  std::unique_ptr<Recordable> MakeRecordable() noexcept { return nullptr; }

  void OnStart(Recordable &) noexcept {}

  void OnEnd(std::unique_ptr<Recordable> &&) noexcept {}

  void ForceFlush(std::chrono::microseconds) noexcept {}

  void Shutdown(std::chrono::microseconds) noexcept {}
};

TEST(ForkAwareSpanProcessorTests, TestActiveSpanProcessorListUpdate)
{
  std::unique_ptr<DummySpanProcessor> span_processor_1(new DummySpanProcessor);
  std::unique_ptr<DummySpanProcessor> span_processor_2(new DummySpanProcessor);
  std::unique_ptr<DummySpanProcessor> span_processor_3(new DummySpanProcessor);
  span_processor_2.reset(nullptr);
  EXPECT_EQ(ForkAwareSpanProcessor::GetActiveSpanProcessorsForTesting(),
            std::vector<const ForkAwareSpanProcessor *>(
                {span_processor_3.get(), span_processor_1.get()}));
}
