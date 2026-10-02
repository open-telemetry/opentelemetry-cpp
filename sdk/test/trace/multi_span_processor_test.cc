// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/sdk/trace/multi_span_processor.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"

using namespace opentelemetry::sdk::trace;
using opentelemetry::trace::SpanContext;

namespace
{

// A processor whose ForceFlush/Shutdown outcome is fixed at construction.
class FixedResultProcessor final : public SpanProcessor
{
public:
  explicit FixedResultProcessor(bool result) : result_(result) {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new SpanData());
  }

  void OnStart(Recordable & /* span */, const SpanContext & /* parent_context */) noexcept override
  {}

  void OnEnd(std::unique_ptr<Recordable> &&span) noexcept override
  {
    auto ignored = std::move(span);
    static_cast<void>(ignored);
  }

  bool ForceFlush(std::chrono::microseconds /* timeout */) noexcept override { return result_; }

  bool Shutdown(std::chrono::microseconds /* timeout */) noexcept override { return result_; }

private:
  bool result_;
};

std::unique_ptr<SpanProcessor> MakeMultiProcessor(bool first, bool second)
{
  std::vector<std::unique_ptr<SpanProcessor>> processors;
  processors.emplace_back(new FixedResultProcessor(first));
  processors.emplace_back(new FixedResultProcessor(second));
  return std::unique_ptr<SpanProcessor>(new MultiSpanProcessor(std::move(processors)));
}

TEST(MultiSpanProcessorTest, ForceFlushFailsWhenAnyChildFails)
{
  EXPECT_TRUE(MakeMultiProcessor(true, true)->ForceFlush());

  EXPECT_FALSE(MakeMultiProcessor(true, false)->ForceFlush());
  EXPECT_FALSE(MakeMultiProcessor(false, true)->ForceFlush());
}

TEST(MultiSpanProcessorTest, ShutdownFailsWhenAnyChildFails)
{
  EXPECT_TRUE(MakeMultiProcessor(true, true)->Shutdown());

  EXPECT_FALSE(MakeMultiProcessor(true, false)->Shutdown());
  EXPECT_FALSE(MakeMultiProcessor(false, true)->Shutdown());
}

}  // namespace
