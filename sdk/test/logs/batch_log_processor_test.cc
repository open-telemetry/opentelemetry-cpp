#include "opentelemetry/sdk/logs/batch_log_processor.h"

#include <gtest/gtest.h>
#include <chrono>
#include <thread>

OPENTELEMETRY_BEGIN_NAMESPACE

/**
 * Returns a mock log exporter meant exclusively for testing only
 */
class MockLogExporter final : public sdk::logs::LogExporter
{
public:
  MockLogExporter(
      std::shared_ptr<std::vector<std::unique_ptr<opentelemetry::logs::LogRecord>>> logs_received,
      std::shared_ptr<std::atomic<bool>> is_shutdown,
      std::shared_ptr<std::atomic<bool>> is_export_completed,
      const std::chrono::milliseconds export_delay = std::chrono::milliseconds(0)) noexcept
      : logs_received_(logs_received),
        is_shutdown_(is_shutdown),
        is_export_completed_(is_export_completed),
        export_delay_(export_delay)
  {}

  sdk::logs::ExportResult Export(
      const nostd::log<std::unique_ptr<sdk::logs::Recordable>> &recordables) noexcept override
  {
    *is_export_completed_ = false;

    std::this_thread::sleep_for(export_delay_);

    for (auto &recordable : recordables)
    {
      auto log = std::unique_ptr<sdk::logs::SpanData>(
          static_cast<sdk::logs::SpanData *>(recordable.release()));

      if (log != nullptr)
      {
        logs_received_->push_back(std::move(log));
      }
    }

    *is_export_completed_ = true;
    return sdk::logs::ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {
    *is_shutdown_ = true;
  }

  bool IsExportCompleted() { return is_export_completed_->load(); }

private:
  std::shared_ptr<std::vector<std::unique_ptr<sdk::logs::SpanData>>> logs_received_;
  std::shared_ptr<std::atomic<bool>> is_shutdown_;
  std::shared_ptr<std::atomic<bool>> is_export_completed_;
  // Meant exclusively to test force flush timeout
  const std::chrono::milliseconds export_delay_;
};

/**
 * Fixture Class
 */
class BatchLogProcessorTestPeer : public testing::Test
{
public:
  std::shared_ptr<sdk::logs::LogProcessor> GetMockProcessor(
      std::shared_ptr<std::vector<std::unique_ptr<sdk::logs::SpanData>>> logs_received,
      std::shared_ptr<std::atomic<bool>> is_shutdown,
      std::shared_ptr<std::atomic<bool>> is_export_completed =
          std::shared_ptr<std::atomic<bool>>(new std::atomic<bool>(false)),
      const std::chrono::milliseconds export_delay          = std::chrono::milliseconds(0),
      const std::chrono::milliseconds schedule_delay_millis = std::chrono::milliseconds(5000),
      const size_t max_queue_size                           = 2048,
      const size_t max_export_batch_size                    = 512)
  {
    return std::shared_ptr<sdk::logs::LogProcessor>(new sdk::logs::BatchLogProcessor(
        GetMockExporter(logs_received, is_shutdown, is_export_completed, export_delay),
        max_queue_size, schedule_delay_millis, max_export_batch_size));
  }

  std::unique_ptr<std::vector<std::unique_ptr<sdk::logs::Recordable>>> GetTestSpans(
      std::shared_ptr<sdk::logs::LogProcessor> processor,
      const int num_logs)
  {
    std::unique_ptr<std::vector<std::unique_ptr<sdk::logs::Recordable>>> test_logs(
        new std::vector<std::unique_ptr<sdk::logs::Recordable>>);

    for (int i = 0; i < num_logs; ++i)
    {
      test_logs->push_back(processor->MakeRecordable());
      static_cast<sdk::logs::SpanData *>(test_logs->at(i).get())
          ->SetName("Span " + std::to_string(i));
    }

    return test_logs;
  }

private:
  std::unique_ptr<sdk::logs::LogExporter> GetMockExporter(
      std::shared_ptr<std::vector<std::unique_ptr<sdk::logs::SpanData>>> logs_received,
      std::shared_ptr<std::atomic<bool>> is_shutdown,
      std::shared_ptr<std::atomic<bool>> is_export_completed,
      const std::chrono::milliseconds export_delay = std::chrono::milliseconds(0))
  {
    return std::unique_ptr<sdk::logs::LogExporter>(
        new MockLogExporter(logs_received, is_shutdown, is_export_completed, export_delay));
  }
};

/* ##################################   TESTS   ############################################ */

TEST_F(BatchLogProcessorTestPeer, TestShutdown)
{
  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<sdk::logs::SpanData>>> logs_received(
      new std::vector<std::unique_ptr<sdk::logs::SpanData>>);

  auto batch_processor = GetMockProcessor(logs_received, is_shutdown);
  const int num_logs  = 3;

  auto test_logs = GetTestSpans(batch_processor, num_logs);

  for (int i = 0; i < num_logs; ++i)
  {
    batch_processor->OnEnd(std::move(test_logs->at(i)));
  }

  batch_processor->Shutdown();

  EXPECT_EQ(num_logs, logs_received->size());
  for (int i = 0; i < num_logs; ++i)
  {
    EXPECT_EQ("Span " + std::to_string(i), logs_received->at(i)->GetName());
  }

  EXPECT_TRUE(is_shutdown->load());
}

TEST_F(BatchLogProcessorTestPeer, TestForceFlush)
{
  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<sdk::logs::SpanData>>> logs_received(
      new std::vector<std::unique_ptr<sdk::logs::SpanData>>);

  auto batch_processor = GetMockProcessor(logs_received, is_shutdown);
  const int num_logs  = 2048;

  auto test_logs = GetTestSpans(batch_processor, num_logs);

  for (int i = 0; i < num_logs; ++i)
  {
    batch_processor->OnEnd(std::move(test_logs->at(i)));
  }

  // Give some time to export
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  batch_processor->ForceFlush();

  EXPECT_EQ(num_logs, logs_received->size());
  for (int i = 0; i < num_logs; ++i)
  {
    EXPECT_EQ("Span " + std::to_string(i), logs_received->at(i)->GetName());
  }

  // Create some more logs to make sure that the processor still works
  auto more_test_logs = GetTestSpans(batch_processor, num_logs);
  for (int i = 0; i < num_logs; ++i)
  {
    batch_processor->OnEnd(std::move(more_test_logs->at(i)));
  }

  // Give some time to export the logs
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  batch_processor->ForceFlush();

  EXPECT_EQ(num_logs * 2, logs_received->size());
  for (int i = 0; i < num_logs; ++i)
  {
    EXPECT_EQ("Span " + std::to_string(i % num_logs),
              logs_received->at(num_logs + i)->GetName());
  }
}

TEST_F(BatchLogProcessorTestPeer, TestManySpansLoss)
{
  /* Test that when exporting more than max_queue_size logs, some are most likely lost*/

  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<sdk::logs::SpanData>>> logs_received(
      new std::vector<std::unique_ptr<sdk::logs::SpanData>>);

  const int max_queue_size = 4096;

  auto batch_processor = GetMockProcessor(logs_received, is_shutdown);

  auto test_logs = GetTestSpans(batch_processor, max_queue_size);

  for (int i = 0; i < max_queue_size; ++i)
  {
    batch_processor->OnEnd(std::move(test_logs->at(i)));
  }

  // Give some time to export the logs
  std::this_thread::sleep_for(std::chrono::milliseconds(700));

  batch_processor->ForceFlush();

  // Span should be exported by now
  EXPECT_GE(max_queue_size, logs_received->size());
}

TEST_F(BatchLogProcessorTestPeer, TestManySpansLossLess)
{
  /* Test that no logs are lost when sending max_queue_size logs */

  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<sdk::logs::SpanData>>> logs_received(
      new std::vector<std::unique_ptr<sdk::logs::SpanData>>);

  const int num_logs = 2048;

  auto batch_processor = GetMockProcessor(logs_received, is_shutdown);

  auto test_logs = GetTestSpans(batch_processor, num_logs);

  for (int i = 0; i < num_logs; ++i)
  {
    batch_processor->OnEnd(std::move(test_logs->at(i)));
  }

  // Give some time to export the logs
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  batch_processor->ForceFlush();

  EXPECT_EQ(num_logs, logs_received->size());
  for (int i = 0; i < num_logs; ++i)
  {
    EXPECT_EQ("Span " + std::to_string(i), logs_received->at(i)->GetName());
  }
}

TEST_F(BatchLogProcessorTestPeer, TestScheduleDelayMillis)
{
  /* Test that max_export_batch_size logs are exported every schedule_delay_millis
     seconds */

  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::atomic<bool>> is_export_completed(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<sdk::logs::SpanData>>> logs_received(
      new std::vector<std::unique_ptr<sdk::logs::SpanData>>);

  const std::chrono::milliseconds export_delay(0);
  const std::chrono::milliseconds schedule_delay_millis(2000);
  const size_t max_export_batch_size = 512;

  auto batch_processor = GetMockProcessor(logs_received, is_shutdown, is_export_completed,
                                          export_delay, schedule_delay_millis);

  auto test_logs = GetTestSpans(batch_processor, max_export_batch_size);

  for (size_t i = 0; i < max_export_batch_size; ++i)
  {
    batch_processor->OnEnd(std::move(test_logs->at(i)));
  }

  // Sleep for schedule_delay_millis milliseconds
  std::this_thread::sleep_for(schedule_delay_millis);

  // small delay to give time to export
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // Spans should be exported by now
  EXPECT_TRUE(is_export_completed->load());
  EXPECT_EQ(max_export_batch_size, logs_received->size());
  for (size_t i = 0; i < max_export_batch_size; ++i)
  {
    EXPECT_EQ("Span " + std::to_string(i), logs_received->at(i)->GetName());
  }
}

OPENTELEMETRY_END_NAMESPACE
