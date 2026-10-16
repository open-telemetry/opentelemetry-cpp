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
#include "opentelemetry/trace/span_context.h"

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

// A processor that fails to create a recordable and counts the spans it receives.
class NullRecordableProcessor final : public SpanProcessor
{
public:
  std::unique_ptr<Recordable> MakeRecordable() noexcept override { return nullptr; }

  void OnStart(Recordable & /* span */, const SpanContext & /* parent_context */) noexcept override
  {
    ++started_;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
  void OnEnd(std::unique_ptr<Recordable> && /* span */) noexcept override { ++ended_; }

  bool ForceFlush(std::chrono::microseconds /* timeout */) noexcept override { return true; }

  bool Shutdown(std::chrono::microseconds /* timeout */) noexcept override { return true; }

  int started_ = 0;
  int ended_   = 0;
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

TEST(MultiSpanProcessorTest, MakeRecordableSkipsChildWithoutRecordable)
{
  auto *null_processor = new NullRecordableProcessor();
  std::vector<std::unique_ptr<SpanProcessor>> processors;
  processors.emplace_back(null_processor);
  processors.emplace_back(new FixedResultProcessor(true));
  MultiSpanProcessor processor(std::move(processors));

  auto recordable = processor.MakeRecordable();
  ASSERT_NE(recordable, nullptr);

  recordable->SetName("span");
  recordable->SetAttribute("key", "value");
  processor.OnStart(*recordable, SpanContext::GetInvalid());
  processor.OnEnd(std::move(recordable));

  EXPECT_EQ(null_processor->started_, 0);
  EXPECT_EQ(null_processor->ended_, 0);
}

}  // namespace
