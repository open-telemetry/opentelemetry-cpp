#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * Returns a mock span exporter meant exclusively for testing only
 */
class MockSpanExporter final : public SpanExporter
{
public:
  MockSpanExporter(std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received,
                   std::shared_ptr<std::atomic<bool>> is_shutdown,
                   std::shared_ptr<std::atomic<bool>> is_export_completed) noexcept
      : spans_received_(spans_received),
        is_shutdown_(is_shutdown),
        is_export_completed_(is_export_completed)
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new SpanData);
  }

  ExportResult Export(const nostd::span<std::unique_ptr<Recordable>> &recordables) noexcept override
  {
    *is_export_completed_ = false;

    for (auto &recordable : recordables)
    {
      auto span = std::unique_ptr<SpanData>(static_cast<SpanData *>(recordable.release()));

      if (span != nullptr)
      {
        spans_received_->push_back(std::move(span));
      }
    }

    *is_export_completed_ = true;
    return ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {
    *is_shutdown_ = true;
  }

private:
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received_;
  std::shared_ptr<std::atomic<bool>> is_shutdown_;
  std::shared_ptr<std::atomic<bool>> is_export_completed_;
};

/**
 * Fixture Class
 */
class BatchSpanProcessorTestPeer : public testing::Test
{
public:
  std::shared_ptr<SpanProcessor> GetMockProcessor(
      std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received,
      std::shared_ptr<std::atomic<bool>> is_shutdown,
      std::shared_ptr<std::atomic<bool>> is_export_completed =
          std::shared_ptr<std::atomic<bool>>(new std::atomic<bool>(false)),
      const std::chrono::milliseconds schedule_delay_millis = std::chrono::milliseconds(5000),
      const size_t max_queue_size                           = 2048,
      const size_t max_export_batch_size                    = 512)
  {
    return std::shared_ptr<SpanProcessor>(
        new BatchSpanProcessor(GetMockExporter(spans_received, is_shutdown, is_export_completed),
                               max_queue_size, schedule_delay_millis, max_export_batch_size));
  }

  std::unique_ptr<std::vector<std::unique_ptr<Recordable>>> GetTestSpans(
      std::shared_ptr<SpanProcessor> processor,
      const int num_spans)
  {
    std::unique_ptr<std::vector<std::unique_ptr<Recordable>>> test_spans(
        new std::vector<std::unique_ptr<Recordable>>);

    for (int i = 0; i < num_spans; ++i)
    {
      test_spans->push_back(processor->MakeRecordable());
      static_cast<SpanData *>(test_spans->at(i).get())->SetName("Span " + std::to_string(i));
    }

    return test_spans;
  }

private:
  std::unique_ptr<SpanExporter> GetMockExporter(
      std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received,
      std::shared_ptr<std::atomic<bool>> is_shutdown,
      std::shared_ptr<std::atomic<bool>> is_export_completed)
  {
    return std::unique_ptr<SpanExporter>(
        new MockSpanExporter(spans_received, is_shutdown, is_export_completed));
  }
};

/* ##################################   TESTS   ############################################ */

TEST_F(BatchSpanProcessorTestPeer, TestShutdown)
{
  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);

  auto batch_processor = GetMockProcessor(spans_received, is_shutdown);
  const int num_spans  = 3;

  auto test_spans = GetTestSpans(batch_processor, num_spans);

  for (int i = 0; i < num_spans; ++i)
  {
    batch_processor->OnEnd(std::move(test_spans->at(i)));
  }

  batch_processor->Shutdown();

  EXPECT_EQ(num_spans, spans_received->size());
  for (int i = 0; i < num_spans; ++i)
  {
    EXPECT_EQ("Span " + std::to_string(i), spans_received->at(i)->GetName());
  }

  EXPECT_TRUE(is_shutdown->load());
}

TEST_F(BatchSpanProcessorTestPeer, TestForceFlush)
{
  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);

  auto batch_processor = GetMockProcessor(spans_received, is_shutdown);
  const int num_spans  = 2048;

  auto test_spans = GetTestSpans(batch_processor, num_spans);

  for (int i = 0; i < num_spans; ++i)
  {
    batch_processor->OnEnd(std::move(test_spans->at(i)));
  }

  // Give some time to export
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  batch_processor->ForceFlush();

  EXPECT_EQ(num_spans, spans_received->size());
  for (int i = 0; i < num_spans; ++i)
  {
    EXPECT_EQ("Span " + std::to_string(i), spans_received->at(i)->GetName());
  }

  // Create some more spans to make sure that the processor still works
  auto more_test_spans = GetTestSpans(batch_processor, num_spans);
  for (int i = 0; i < num_spans; ++i)
  {
    batch_processor->OnEnd(std::move(more_test_spans->at(i)));
  }

  // Give some time to export the spans
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  batch_processor->ForceFlush();

  EXPECT_EQ(num_spans * 2, spans_received->size());
  for (int i = 0; i < num_spans; ++i)
  {
    EXPECT_EQ("Span " + std::to_string(i % num_spans),
              spans_received->at(num_spans + i)->GetName());
  }
}

TEST_F(BatchSpanProcessorTestPeer, TestManySpansLoss)
{
  /* Test that when exporting more than max_queue_size spans, some are most likely lost*/

  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);

  const int max_queue_size = 4096;

  auto batch_processor = GetMockProcessor(spans_received, is_shutdown);

  auto test_spans = GetTestSpans(batch_processor, max_queue_size);

  for (int i = 0; i < max_queue_size; ++i)
  {
    batch_processor->OnEnd(std::move(test_spans->at(i)));
  }

  // Give some time to export the spans
  std::this_thread::sleep_for(std::chrono::milliseconds(700));

  batch_processor->ForceFlush();

  // Span should be exported by now
  EXPECT_GE(max_queue_size, spans_received->size());
}

TEST_F(BatchSpanProcessorTestPeer, TestManySpansLossLess)
{
  /* Test that no spans are lost when sending max_queue_size spans */

  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);

  const int num_spans = 2048;

  auto batch_processor = GetMockProcessor(spans_received, is_shutdown);

  auto test_spans = GetTestSpans(batch_processor, num_spans);

  for (int i = 0; i < num_spans; ++i)
  {
    batch_processor->OnEnd(std::move(test_spans->at(i)));
  }

  // Give some time to export the spans
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  batch_processor->ForceFlush();

  EXPECT_EQ(num_spans, spans_received->size());
  for (int i = 0; i < num_spans; ++i)
  {
    EXPECT_EQ("Span " + std::to_string(i), spans_received->at(i)->GetName());
  }
}

TEST_F(BatchSpanProcessorTestPeer, TestScheduleDelayMillis)
{
  /* Test that max_export_batch_size spans are exported every schedule_delay_millis
     seconds */

  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::atomic<bool>> is_export_completed(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<SpanData>>> spans_received(
      new std::vector<std::unique_ptr<SpanData>>);

  const std::chrono::milliseconds schedule_delay_millis(2000);
  const size_t max_export_batch_size = 512;

  auto batch_processor =
      GetMockProcessor(spans_received, is_shutdown, is_export_completed, schedule_delay_millis);

  auto test_spans = GetTestSpans(batch_processor, max_export_batch_size);

  for (size_t i = 0; i < max_export_batch_size; ++i)
  {
    batch_processor->OnEnd(std::move(test_spans->at(i)));
  }

  // Sleep for schedule_delay_millis milliseconds
  std::this_thread::sleep_for(schedule_delay_millis);

  // small delay to give time to export
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // Spans should be exported by now
  EXPECT_TRUE(is_export_completed->load());
  EXPECT_EQ(max_export_batch_size, spans_received->size());
  for (size_t i = 0; i < max_export_batch_size; ++i)
  {
    EXPECT_EQ("Span " + std::to_string(i), spans_received->at(i)->GetName());
  }
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE