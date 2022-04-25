// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0
#ifndef ENABLE_ASYNC_EXPORT
#  include <gtest/gtest.h>
TEST(AsyncBatchLogProcessor, DummyTest)
{
  // For linking
}
#endif

#ifdef ENABLE_LOGS_PREVIEW
#  ifdef ENABLE_ASYNC_EXPORT

#    include "opentelemetry/sdk/logs/async_batch_log_processor.h"
#    include "opentelemetry/sdk/logs/exporter.h"
#    include "opentelemetry/sdk/logs/log_record.h"

#    include <gtest/gtest.h>
#    include <chrono>
#    include <list>
#    include <memory>
#    include <thread>

using namespace opentelemetry::sdk::logs;
using namespace opentelemetry::sdk::common;

/**
 * A sample log exporter
 * for testing the batch log processor
 */
class MockLogExporter final : public LogExporter
{
public:
  MockLogExporter(std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received,
                  std::shared_ptr<std::atomic<bool>> is_shutdown,
                  std::shared_ptr<std::atomic<bool>> is_export_completed,
                  const std::chrono::milliseconds export_delay = std::chrono::milliseconds(0),
                  int callback_count                           = 1)
      : logs_received_(logs_received),
        is_shutdown_(is_shutdown),
        is_export_completed_(is_export_completed),
        export_delay_(export_delay),
        callback_count_(callback_count)
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept
  {
    return std::unique_ptr<Recordable>(new LogRecord());
  }

  // Export method stores the logs received into a shared list of record names
  ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<Recordable>> &records) noexcept override
  {
    *is_export_completed_ = false;  // Meant exclusively to test scheduled_delay_millis

    for (auto &record : records)
    {
      auto log = std::unique_ptr<LogRecord>(static_cast<LogRecord *>(record.release()));
      if (log != nullptr)
      {
        logs_received_->push_back(std::move(log));
      }
    }

    *is_export_completed_ = true;
    return ExportResult::kSuccess;
  }

  void Export(const opentelemetry::nostd::span<std::unique_ptr<Recordable>> &records,
              std::function<bool(opentelemetry::sdk::common::ExportResult)>
                  &&result_callback) noexcept override
  {
    // We should keep the order of test records
    auto result = Export(records);
    async_threads_.emplace_back(std::make_shared<std::thread>(
        [this,
         result](std::function<bool(opentelemetry::sdk::common::ExportResult)> &&result_callback) {
          for (int i = 0; i < callback_count_; i++)
          {
            result_callback(result);
          }
        },
        std::move(result_callback)));
  }

  // toggles the boolean flag marking this exporter as shut down
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override
  {
    while (!async_threads_.empty())
    {
      std::list<std::shared_ptr<std::thread>> async_threads;
      async_threads.swap(async_threads_);
      for (auto &async_thread : async_threads)
      {
        if (async_thread && async_thread->joinable())
        {
          async_thread->join();
        }
      }
    }
    *is_shutdown_ = true;
    return true;
  }

private:
  std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received_;
  std::shared_ptr<std::atomic<bool>> is_shutdown_;
  std::shared_ptr<std::atomic<bool>> is_export_completed_;
  const std::chrono::milliseconds export_delay_;
  std::list<std::shared_ptr<std::thread>> async_threads_;
  int callback_count_;
};

/**
 * A fixture class for testing the BatchLogProcessor class that uses the TestExporter defined above.
 */
class AsyncBatchLogProcessorTest : public testing::Test  // ::testing::Test
{
public:
  // returns a batch log processor that received a batch of log records, a shared pointer to a
  // is_shutdown flag, and the processor configuration options (default if unspecified)
  std::shared_ptr<LogProcessor> GetMockProcessor(
      std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received,
      std::shared_ptr<std::atomic<bool>> is_shutdown,
      std::shared_ptr<std::atomic<bool>> is_export_completed =
          std::shared_ptr<std::atomic<bool>>(new std::atomic<bool>(false)),
      const std::chrono::milliseconds export_delay           = std::chrono::milliseconds(0),
      const std::chrono::milliseconds scheduled_delay_millis = std::chrono::milliseconds(5000),
      const size_t max_queue_size                            = 2048,
      const size_t max_export_batch_size                     = 512,
      const size_t max_export_async                          = 8,
      int callback_count                                     = 1)
  {
    AsyncBatchLogProcessorOptions options;
    options.max_queue_size        = max_queue_size;
    options.schedule_delay_millis = scheduled_delay_millis;
    options.max_export_batch_size = max_export_batch_size;
    options.max_export_async              = max_export_async;
    return std::shared_ptr<LogProcessor>(new AsyncBatchLogProcessor(
        std::unique_ptr<LogExporter>(new MockLogExporter(
            logs_received, is_shutdown, is_export_completed, export_delay, callback_count)),
        options));
  }
};

TEST_F(AsyncBatchLogProcessorTest, TestAsyncShutdown)
{
  // initialize a batch log processor with the test exporter
  std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received(
      new std::vector<std::unique_ptr<LogRecord>>);
  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::atomic<bool>> is_export_completed(new std::atomic<bool>(false));

  const std::chrono::milliseconds export_delay(0);
  const std::chrono::milliseconds scheduled_delay_millis(5000);
  const size_t max_export_batch_size = 512;
  const size_t max_queue_size        = 2048;
  const size_t max_export_async      = 5;

  auto batch_processor = GetMockProcessor(logs_received, is_shutdown, is_export_completed,
                                          export_delay, scheduled_delay_millis, max_queue_size,
                                          max_export_batch_size, max_export_async);

  // Create a few test log records and send them to the processor
  const int num_logs = 2048;

  for (int i = 0; i < num_logs; ++i)
  {
    auto log = batch_processor->MakeRecordable();
    log->SetName("Log" + std::to_string(i));
    batch_processor->OnReceive(std::move(log));
  }

  // Test that shutting down the processor will first wait for the
  // current batch of logs to be sent to the log exporter
  // by checking the number of logs sent and the names of the logs sent
  EXPECT_EQ(true, batch_processor->Shutdown());
  // It's safe to shutdown again
  EXPECT_TRUE(batch_processor->Shutdown());

  EXPECT_EQ(num_logs, logs_received->size());

  // Assume logs are received by exporter in same order as sent by processor
  for (int i = 0; i < num_logs; ++i)
  {
    EXPECT_EQ("Log" + std::to_string(i), logs_received->at(i)->GetName());
  }

  // Also check that the processor is shut down at the end
  EXPECT_TRUE(is_shutdown->load());
}

TEST_F(AsyncBatchLogProcessorTest, TestAsyncShutdownNoCallback)
{
  // initialize a batch log processor with the test exporter
  std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received(
      new std::vector<std::unique_ptr<LogRecord>>);
  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::atomic<bool>> is_export_completed(new std::atomic<bool>(false));

  const std::chrono::milliseconds export_delay(0);
  const std::chrono::milliseconds scheduled_delay_millis(5000);
  const size_t max_export_batch_size = 512;
  const size_t max_queue_size        = 2048;
  const size_t max_export_async      = 5;

  auto batch_processor = GetMockProcessor(logs_received, is_shutdown, is_export_completed,
                                          export_delay, scheduled_delay_millis, max_queue_size,
                                          max_export_batch_size, max_export_async, 0);

  // Create a few test log records and send them to the processor
  const int num_logs = 2048;

  for (int i = 0; i < num_logs; ++i)
  {
    auto log = batch_processor->MakeRecordable();
    log->SetName("Log" + std::to_string(i));
    batch_processor->OnReceive(std::move(log));
  }

  EXPECT_EQ(true, batch_processor->Shutdown(std::chrono::milliseconds(5000)));
  // It's safe to shutdown again
  EXPECT_TRUE(batch_processor->Shutdown());

  // Also check that the processor is shut down at the end
  EXPECT_TRUE(is_shutdown->load());
}

TEST_F(AsyncBatchLogProcessorTest, TestAsyncForceFlush)
{
  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received(
      new std::vector<std::unique_ptr<LogRecord>>);
  std::shared_ptr<std::atomic<bool>> is_export_completed(new std::atomic<bool>(false));

  const std::chrono::milliseconds export_delay(0);
  const std::chrono::milliseconds scheduled_delay_millis(5000);
  const size_t max_export_batch_size = 512;
  const size_t max_queue_size        = 2048;

  auto batch_processor =
      GetMockProcessor(logs_received, is_shutdown, is_export_completed, export_delay,
                       scheduled_delay_millis, max_queue_size, max_export_batch_size);

  const int num_logs = 2048;

  for (int i = 0; i < num_logs; ++i)
  {
    auto log = batch_processor->MakeRecordable();
    log->SetName("Log" + std::to_string(i));
    batch_processor->OnReceive(std::move(log));
  }

  EXPECT_TRUE(batch_processor->ForceFlush());

  EXPECT_EQ(num_logs, logs_received->size());
  for (int i = 0; i < num_logs; ++i)
  {
    EXPECT_EQ("Log" + std::to_string(i), logs_received->at(i)->GetName());
  }

  // Create some more logs to make sure that the processor still works
  for (int i = 0; i < num_logs; ++i)
  {
    auto log = batch_processor->MakeRecordable();
    log->SetName("Log" + std::to_string(i));
    batch_processor->OnReceive(std::move(log));
  }

  EXPECT_TRUE(batch_processor->ForceFlush());

  EXPECT_EQ(num_logs * 2, logs_received->size());
  for (int i = 0; i < num_logs * 2; ++i)
  {
    EXPECT_EQ("Log" + std::to_string(i % num_logs), logs_received->at(i)->GetName());
  }
}

TEST_F(AsyncBatchLogProcessorTest, TestManyLogsLoss)
{
  /* Test that when exporting more than max_queue_size logs, some are most likely lost*/

  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received(
      new std::vector<std::unique_ptr<LogRecord>>);

  const int max_queue_size = 4096;

  auto batch_processor = GetMockProcessor(logs_received, is_shutdown);

  // Create max_queue_size log records
  for (int i = 0; i < max_queue_size; ++i)
  {
    auto log = batch_processor->MakeRecordable();
    log->SetName("Log" + std::to_string(i));
    batch_processor->OnReceive(std::move(log));
  }

  EXPECT_TRUE(batch_processor->ForceFlush());

  // Log should be exported by now
  EXPECT_GE(max_queue_size, logs_received->size());
}

TEST_F(AsyncBatchLogProcessorTest, TestManyLogsLossLess)
{
  /* Test that no logs are lost when sending max_queue_size logs */

  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received(
      new std::vector<std::unique_ptr<LogRecord>>);
  auto batch_processor = GetMockProcessor(logs_received, is_shutdown);

  const int num_logs = 2048;

  for (int i = 0; i < num_logs; ++i)
  {
    auto log = batch_processor->MakeRecordable();
    log->SetName("Log" + std::to_string(i));
    batch_processor->OnReceive(std::move(log));
  }

  EXPECT_TRUE(batch_processor->ForceFlush());

  EXPECT_EQ(num_logs, logs_received->size());
  for (int i = 0; i < num_logs; ++i)
  {
    EXPECT_EQ("Log" + std::to_string(i), logs_received->at(i)->GetName());
  }
}

TEST_F(AsyncBatchLogProcessorTest, TestScheduledDelayMillis)
{
  /* Test that max_export_batch_size logs are exported every scheduled_delay_millis
     seconds */

  std::shared_ptr<std::atomic<bool>> is_shutdown(new std::atomic<bool>(false));
  std::shared_ptr<std::atomic<bool>> is_export_completed(new std::atomic<bool>(false));
  std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received(
      new std::vector<std::unique_ptr<LogRecord>>);

  const std::chrono::milliseconds export_delay(0);
  const std::chrono::milliseconds scheduled_delay_millis(2000);
  const size_t max_export_batch_size = 512;

  auto batch_processor = GetMockProcessor(logs_received, is_shutdown, is_export_completed,
                                          export_delay, scheduled_delay_millis);

  for (std::size_t i = 0; i < max_export_batch_size; ++i)
  {
    auto log = batch_processor->MakeRecordable();
    log->SetName("Log" + std::to_string(i));
    batch_processor->OnReceive(std::move(log));
  }
  // Sleep for scheduled_delay_millis milliseconds
  std::this_thread::sleep_for(scheduled_delay_millis);

  // small delay to give time to export, which is being performed
  // asynchronously by the worker thread (this thread will not
  // forcibly join() the main thread unless processor's shutdown() is called).
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // Logs should be exported by now
  EXPECT_TRUE(is_export_completed->load());
  EXPECT_EQ(max_export_batch_size, logs_received->size());
  for (size_t i = 0; i < max_export_batch_size; ++i)
  {
    EXPECT_EQ("Log" + std::to_string(i), logs_received->at(i)->GetName());
  }
}
#  endif
#endif
