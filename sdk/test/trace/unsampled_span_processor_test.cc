// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/simple_processor_options.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::trace;

namespace
{

/**
 * Mock span exporter for testing
 */
class MockSpanExporter : public SpanExporter
{
public:
  explicit MockSpanExporter(std::vector<std::unique_ptr<SpanData>> *spans) : spans_(spans) {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new SpanData);
  }

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<Recordable>> &recordables) noexcept override
  {
    for (auto &recordable : recordables)
    {
      auto span = std::unique_ptr<SpanData>(static_cast<SpanData *>(recordable.release()));
      spans_->push_back(std::move(span));
    }
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }

  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }

private:
  std::vector<std::unique_ptr<SpanData>> *spans_;
};

/**
 * Create a span with specific sampling status
 */
std::unique_ptr<SpanData> CreateTestSpan(bool sampled, bool valid_context = true)
{
  auto span = std::make_unique<SpanData>();
  span->SetName("test_span");

  if (valid_context)
  {
    TraceFlags flags(sampled ? TraceFlags::kIsSampled : 0);

    // Create valid trace id and span id using arrays
    uint8_t trace_id_bytes[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint8_t span_id_bytes[8]   = {1, 2, 3, 4, 5, 6, 7, 8};

    TraceId trace_id{opentelemetry::nostd::span<const uint8_t, 16>(trace_id_bytes)};
    SpanId span_id{opentelemetry::nostd::span<const uint8_t, 8>(span_id_bytes)};

    SpanContext context(trace_id, span_id, flags, false);
    span->SetIdentity(context, SpanId());
  }
  // If valid_context is false, we leave the default invalid context

  return span;
}

/**
 * Test BatchSpanProcessor with export_unsampled_spans flag
 */
class BatchSpanProcessorUnsampledTest : public testing::Test
{
public:
  void SetUp() override {}

  void TearDown() override
  {
    if (processor_)
    {
      processor_->Shutdown();
    }
  }

protected:
  std::vector<std::unique_ptr<SpanData>> spans_;
  std::shared_ptr<BatchSpanProcessor> processor_;
};

TEST_F(BatchSpanProcessorUnsampledTest, DefaultBehaviorDropsUnsampledSpans)
{
  // Default options should not export unsampled spans
  BatchSpanProcessorOptions options;
  EXPECT_FALSE(options.export_unsampled_spans);

  processor_ = std::make_shared<BatchSpanProcessor>(
      std::unique_ptr<SpanExporter>(new MockSpanExporter(&spans_)), options);

  // Create a sampled span and an unsampled span with valid contexts
  auto sampled_span   = CreateTestSpan(true, true);
  auto unsampled_span = CreateTestSpan(false, true);

  // Process the spans
  std::unique_ptr<Recordable> sampled_recordable(sampled_span.release());
  std::unique_ptr<Recordable> unsampled_recordable(unsampled_span.release());

  processor_->OnEnd(std::move(sampled_recordable));
  processor_->OnEnd(std::move(unsampled_recordable));

  // Force flush to export spans
  processor_->ForceFlush();

  // Should only export the sampled span
  EXPECT_EQ(1, spans_.size());
  EXPECT_TRUE(spans_[0]->GetSpanContext().IsSampled());
}

TEST_F(BatchSpanProcessorUnsampledTest, EnabledFlagExportsUnsampledSpans)
{
  // Enable exporting unsampled spans
  BatchSpanProcessorOptions options;
  options.export_unsampled_spans = true;

  processor_ = std::make_shared<BatchSpanProcessor>(
      std::unique_ptr<SpanExporter>(new MockSpanExporter(&spans_)), options);

  // Create a sampled span and an unsampled span with valid contexts
  auto sampled_span   = CreateTestSpan(true, true);
  auto unsampled_span = CreateTestSpan(false, true);

  // Process the spans
  std::unique_ptr<Recordable> sampled_recordable(sampled_span.release());
  std::unique_ptr<Recordable> unsampled_recordable(unsampled_span.release());

  processor_->OnEnd(std::move(sampled_recordable));
  processor_->OnEnd(std::move(unsampled_recordable));

  // Force flush to export spans
  processor_->ForceFlush();

  // Should export both spans
  EXPECT_EQ(2, spans_.size());

  // Check that we have one sampled and one unsampled span
  bool has_sampled   = false;
  bool has_unsampled = false;
  for (const auto &span : spans_)
  {
    if (span->GetSpanContext().IsSampled())
    {
      has_sampled = true;
    }
    else
    {
      has_unsampled = true;
    }
  }
  EXPECT_TRUE(has_sampled);
  EXPECT_TRUE(has_unsampled);
}

TEST_F(BatchSpanProcessorUnsampledTest, InvalidContextSpansAlwaysExported)
{
  // Default options - should not export valid unsampled spans but should export invalid context
  // spans
  BatchSpanProcessorOptions options;
  options.export_unsampled_spans = false;

  processor_ = std::make_shared<BatchSpanProcessor>(
      std::unique_ptr<SpanExporter>(new MockSpanExporter(&spans_)), options);

  // Create spans with invalid context (like test spans)
  auto invalid_span1 = CreateTestSpan(false, false);
  auto invalid_span2 = CreateTestSpan(true, false);

  // Process the spans
  std::unique_ptr<Recordable> span1_recordable(invalid_span1.release());
  std::unique_ptr<Recordable> span2_recordable(invalid_span2.release());

  processor_->OnEnd(std::move(span1_recordable));
  processor_->OnEnd(std::move(span2_recordable));

  // Force flush to export spans
  processor_->ForceFlush();

  // Should export both spans for backward compatibility
  EXPECT_EQ(2, spans_.size());
}

/**
 * Test SimpleSpanProcessor with export_unsampled_spans flag
 */
class SimpleSpanProcessorUnsampledTest : public testing::Test
{
protected:
  std::vector<std::unique_ptr<SpanData>> spans_;
};

TEST_F(SimpleSpanProcessorUnsampledTest, DefaultBehaviorDropsUnsampledSpans)
{
  // Default constructor should not export unsampled spans
  auto processor = std::make_unique<SimpleSpanProcessor>(
      std::unique_ptr<SpanExporter>(new MockSpanExporter(&spans_)));

  // Create a sampled span and an unsampled span with valid contexts
  auto sampled_span   = CreateTestSpan(true, true);
  auto unsampled_span = CreateTestSpan(false, true);

  // Process the spans
  std::unique_ptr<Recordable> sampled_recordable(sampled_span.release());
  std::unique_ptr<Recordable> unsampled_recordable(unsampled_span.release());

  processor->OnEnd(std::move(sampled_recordable));
  processor->OnEnd(std::move(unsampled_recordable));

  // Should only export the sampled span
  EXPECT_EQ(1, spans_.size());
  EXPECT_TRUE(spans_[0]->GetSpanContext().IsSampled());
}

TEST_F(SimpleSpanProcessorUnsampledTest, EnabledFlagExportsUnsampledSpans)
{
  // Enable exporting unsampled spans
  SimpleSpanProcessorOptions options;
  options.export_unsampled_spans = true;

  auto processor = std::make_unique<SimpleSpanProcessor>(
      std::unique_ptr<SpanExporter>(new MockSpanExporter(&spans_)), options);

  // Create a sampled span and an unsampled span with valid contexts
  auto sampled_span   = CreateTestSpan(true, true);
  auto unsampled_span = CreateTestSpan(false, true);

  // Process the spans
  std::unique_ptr<Recordable> sampled_recordable(sampled_span.release());
  std::unique_ptr<Recordable> unsampled_recordable(unsampled_span.release());

  processor->OnEnd(std::move(sampled_recordable));
  processor->OnEnd(std::move(unsampled_recordable));

  // Should export both spans
  EXPECT_EQ(2, spans_.size());

  // Check that we have one sampled and one unsampled span
  bool has_sampled   = false;
  bool has_unsampled = false;
  for (const auto &span : spans_)
  {
    if (span->GetSpanContext().IsSampled())
    {
      has_sampled = true;
    }
    else
    {
      has_unsampled = true;
    }
  }
  EXPECT_TRUE(has_sampled);
  EXPECT_TRUE(has_unsampled);
}

TEST_F(SimpleSpanProcessorUnsampledTest, InvalidContextSpansAlwaysExported)
{
  // Default options - should not export valid unsampled spans but should export invalid context
  // spans
  SimpleSpanProcessorOptions options;
  options.export_unsampled_spans = false;

  auto processor = std::make_unique<SimpleSpanProcessor>(
      std::unique_ptr<SpanExporter>(new MockSpanExporter(&spans_)), options);

  // Create spans with invalid context (like test spans)
  auto invalid_span1 = CreateTestSpan(false, false);
  auto invalid_span2 = CreateTestSpan(true, false);

  // Process the spans
  std::unique_ptr<Recordable> span1_recordable(invalid_span1.release());
  std::unique_ptr<Recordable> span2_recordable(invalid_span2.release());

  processor->OnEnd(std::move(span1_recordable));
  processor->OnEnd(std::move(span2_recordable));

  // Should export both spans for backward compatibility
  EXPECT_EQ(2, spans_.size());
}

}  // namespace