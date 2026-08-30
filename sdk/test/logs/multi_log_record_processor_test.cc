// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/logs/multi_log_record_processor.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"

using namespace opentelemetry::sdk::logs;
namespace context               = opentelemetry::context;
namespace logs_api              = opentelemetry::logs;
namespace instrumentation_scope = opentelemetry::sdk::instrumentationscope;
namespace nostd                 = opentelemetry::nostd;

namespace
{

class TestLogRecordable final : public Recordable
{
public:
  void SetTimestamp(opentelemetry::common::SystemTimestamp) noexcept override {}

  void SetObservedTimestamp(opentelemetry::common::SystemTimestamp) noexcept override {}

  void SetSeverity(logs_api::Severity) noexcept override {}

  void SetBody(const opentelemetry::common::AttributeValue &) noexcept override {}

  void SetEventId(int64_t, nostd::string_view) noexcept override {}

  void SetTraceId(const opentelemetry::trace::TraceId &) noexcept override {}

  void SetSpanId(const opentelemetry::trace::SpanId &) noexcept override {}

  void SetTraceFlags(const opentelemetry::trace::TraceFlags &) noexcept override {}

  void SetAttribute(nostd::string_view,
                    const opentelemetry::common::AttributeValue &) noexcept override
  {}

  void SetResource(const opentelemetry::sdk::resource::Resource &) noexcept override {}

  void SetInstrumentationScope(
      const instrumentation_scope::InstrumentationScope &) noexcept override
  {}
};

struct EnabledCallState
{
  context::Context context;
  std::string scope_name;
  logs_api::Severity severity = logs_api::Severity::kInvalid;
  std::string event_name;
  size_t call_count                      = 0;
  size_t on_emit_with_context_call_count = 0;
  nostd::variant<opentelemetry::trace::SpanContext, context::Context> last_emitted_context{
      opentelemetry::trace::SpanContext::GetInvalid()};
};

class EnabledProcessor final : public LogRecordProcessor
{
public:
  explicit EnabledProcessor(bool enabled,
                            std::shared_ptr<EnabledCallState> call_state = nullptr) noexcept
      : enabled_(enabled), call_state_(std::move(call_state))
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new TestLogRecordable());
  }

  void OnEmit(std::unique_ptr<Recordable> &&record) noexcept override
  {
    auto ignored = std::move(record);
    static_cast<void>(ignored);
  }

  void OnEmitWithContext(std::unique_ptr<Recordable> &&record,
                         const nostd::variant<opentelemetry::trace::SpanContext, context::Context>
                             &context_or_span) noexcept override
  {
    auto ignored = std::move(record);
    static_cast<void>(ignored);
    if (call_state_ != nullptr)
    {
      ++call_state_->on_emit_with_context_call_count;
      call_state_->last_emitted_context = context_or_span;
    }
  }

  bool ForceFlush(std::chrono::microseconds /* timeout */) noexcept override { return true; }

  bool Shutdown(std::chrono::microseconds /* timeout */) noexcept override { return true; }

protected:
  bool EnabledImplementation(
      const nostd::variant<opentelemetry::trace::SpanContext, context::Context> &context_or_span,
      const instrumentation_scope::InstrumentationScope &scope,
      logs_api::Severity severity,
      nostd::string_view event_name) const noexcept override
  {
    if (call_state_ != nullptr)
    {
      if (const context::Context *ctx = nostd::get_if<context::Context>(&context_or_span))
      {
        call_state_->context = *ctx;
      }
      else
      {
        call_state_->context = context::Context{};
      }
      call_state_->scope_name = scope.GetName();
      call_state_->severity   = severity;
      call_state_->event_name = std::string(event_name);
      ++call_state_->call_count;
    }
    return enabled_;
  }

private:
  bool enabled_;
  std::shared_ptr<EnabledCallState> call_state_;
};

TEST(MultiLogRecordProcessorTest, EnabledWhenAnyChildEnabled)
{
  auto first_state  = std::make_shared<EnabledCallState>();
  auto second_state = std::make_shared<EnabledCallState>();

  std::vector<std::unique_ptr<LogRecordProcessor>> processors;
  processors.emplace_back(new EnabledProcessor(false, first_state));
  processors.emplace_back(new EnabledProcessor(true, second_state));
  MultiLogRecordProcessor processor(std::move(processors));

  context::Context test_context{"test-key", true};
  auto scope = instrumentation_scope::InstrumentationScope::Create("test-scope");

  EXPECT_TRUE(
      processor.Enabled(test_context, *scope, logs_api::Severity::kError, "test-event-name"));
  EXPECT_EQ(first_state->call_count, 1U);
  EXPECT_EQ(second_state->call_count, 1U);
  EXPECT_EQ(second_state->event_name, "test-event-name");
}

TEST(MultiLogRecordProcessorTest, DisabledWhenAllChildrenDisabled)
{
  std::vector<std::unique_ptr<LogRecordProcessor>> processors;
  processors.emplace_back(new EnabledProcessor(false));
  processors.emplace_back(new EnabledProcessor(false));
  MultiLogRecordProcessor processor(std::move(processors));

  context::Context test_context{"test-key", true};
  auto scope = instrumentation_scope::InstrumentationScope::Create("test-scope");

  EXPECT_FALSE(
      processor.Enabled(test_context, *scope, logs_api::Severity::kError, "test-event-name"));
}

TEST(MultiLogRecordProcessorTest, OnEmitWithContextFansOutToChildren)
{
  auto first_state  = std::make_shared<EnabledCallState>();
  auto second_state = std::make_shared<EnabledCallState>();

  std::vector<std::unique_ptr<LogRecordProcessor>> processors;
  processors.emplace_back(new EnabledProcessor(true, first_state));
  processors.emplace_back(new EnabledProcessor(true, second_state));
  MultiLogRecordProcessor processor(std::move(processors));

  const uint8_t trace_id_bytes[opentelemetry::trace::TraceId::kSize] = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  const uint8_t span_id_bytes[opentelemetry::trace::SpanId::kSize] = {1, 2, 3, 4, 5, 6, 7, 8};
  const opentelemetry::trace::SpanContext span_context(
      opentelemetry::trace::TraceId(trace_id_bytes), opentelemetry::trace::SpanId(span_id_bytes),
      opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled}, false);
  const nostd::variant<opentelemetry::trace::SpanContext, context::Context> resolved_context{
      span_context};

  auto recordable = processor.MakeRecordable();
  processor.OnEmitWithContext(std::move(recordable), resolved_context);

  ASSERT_EQ(first_state->on_emit_with_context_call_count, 1U);
  ASSERT_EQ(second_state->on_emit_with_context_call_count, 1U);
  const auto *received_span_context =
      nostd::get_if<opentelemetry::trace::SpanContext>(&first_state->last_emitted_context);
  ASSERT_NE(received_span_context, nullptr);
  EXPECT_EQ(received_span_context->trace_id(), span_context.trace_id());
  EXPECT_EQ(received_span_context->span_id(), span_context.span_id());
}

TEST(MultiLogRecordProcessorTest, OnEmitWithContextIgnoresNullRecord)
{
  auto first_state = std::make_shared<EnabledCallState>();

  std::vector<std::unique_ptr<LogRecordProcessor>> processors;
  processors.emplace_back(new EnabledProcessor(true, first_state));
  MultiLogRecordProcessor processor(std::move(processors));

  const nostd::variant<opentelemetry::trace::SpanContext, context::Context> resolved_context{
      opentelemetry::trace::SpanContext::GetInvalid()};
  processor.OnEmitWithContext(std::unique_ptr<Recordable>(nullptr), resolved_context);

  EXPECT_EQ(first_state->on_emit_with_context_call_count, 0U);
}

TEST(MultiLogRecordProcessorTest, EmptyProcessorIsDisabled)
{
  MultiLogRecordProcessor processor(std::vector<std::unique_ptr<LogRecordProcessor>>{});

  context::Context test_context{"test-key", true};
  auto scope = instrumentation_scope::InstrumentationScope::Create("test-scope");

  EXPECT_FALSE(
      processor.Enabled(test_context, *scope, logs_api::Severity::kDebug, "test-event-name"));
}

// A processor whose ForceFlush/Shutdown outcome is fixed at construction.
class FixedResultProcessor final : public LogRecordProcessor
{
public:
  explicit FixedResultProcessor(bool result) : result_(result) {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new TestLogRecordable());
  }

  void OnEmit(std::unique_ptr<Recordable> &&record) noexcept override
  {
    auto ignored = std::move(record);
    static_cast<void>(ignored);
  }

  bool ForceFlush(std::chrono::microseconds /* timeout */) noexcept override { return result_; }

  bool Shutdown(std::chrono::microseconds /* timeout */) noexcept override { return result_; }

private:
  bool result_;
};

std::unique_ptr<LogRecordProcessor> MakeMultiProcessor(bool first, bool second)
{
  std::vector<std::unique_ptr<LogRecordProcessor>> processors;
  processors.emplace_back(new FixedResultProcessor(first));
  processors.emplace_back(new FixedResultProcessor(second));
  return std::unique_ptr<LogRecordProcessor>(new MultiLogRecordProcessor(std::move(processors)));
}

TEST(MultiLogRecordProcessorTest, ForceFlushFailsWhenAnyChildFails)
{
  EXPECT_TRUE(MakeMultiProcessor(true, true)->ForceFlush());

  EXPECT_FALSE(MakeMultiProcessor(true, false)->ForceFlush());
  EXPECT_FALSE(MakeMultiProcessor(false, true)->ForceFlush());
}

TEST(MultiLogRecordProcessorTest, ShutdownFailsWhenAnyChildFails)
{
  EXPECT_TRUE(MakeMultiProcessor(true, true)->Shutdown());

  EXPECT_FALSE(MakeMultiProcessor(true, false)->Shutdown());
  EXPECT_FALSE(MakeMultiProcessor(false, true)->Shutdown());
}

}  // namespace
