// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/simple_log_processor.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/sdk/logs/exporter.h"
#  include "opentelemetry/sdk/logs/log_record.h"

#  include <gtest/gtest.h>

#  include <chrono>
#  include <thread>

using namespace opentelemetry::sdk::logs;
using namespace opentelemetry::sdk::common;
namespace nostd = opentelemetry::nostd;

/*
 * A test exporter that can return a vector of all the records it has received,
 * and keep track of the number of times its Shutdown() function was called.
 */
class TestExporter final : public LogExporter
{
public:
  TestExporter(int *shutdown_counter,
               std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received,
               size_t *batch_size_received)
      : shutdown_counter_(shutdown_counter),
        logs_received_(logs_received),
        batch_size_received(batch_size_received)
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new LogRecord());
  }

  // Stores the names of the log records this exporter receives to an internal list
  ExportResult Export(const nostd::span<std::unique_ptr<Recordable>> &records) noexcept override
  {
    *batch_size_received = records.size();
    for (auto &record : records)
    {
      auto log_record = std::unique_ptr<LogRecord>(static_cast<LogRecord *>(record.release()));

      if (log_record != nullptr)
      {
        logs_received_->push_back(std::move(log_record));
      }
    }
    return ExportResult::kSuccess;
  }

  // Increment the shutdown counter everytime this method is called
  bool Shutdown(std::chrono::microseconds timeout) noexcept override
  {
    *shutdown_counter_ += 1;
    return true;
  }

private:
  int *shutdown_counter_;
  std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received_;
  size_t *batch_size_received;
};

// Tests whether the simple processor successfully creates a batch of size 1
// and whether the contents of the record is sent to the exporter correctly
TEST(SimpleLogProcessorTest, SendReceivedLogsToExporter)
{
  // Create a simple processor with a TestExporter attached
  std::shared_ptr<std::vector<std::unique_ptr<LogRecord>>> logs_received(
      new std::vector<std::unique_ptr<LogRecord>>);
  size_t batch_size_received = 0;

  std::unique_ptr<TestExporter> exporter(
      new TestExporter(nullptr, logs_received, &batch_size_received));

  SimpleLogProcessor processor(std::move(exporter));

  // Send some log records to the processor (which should then send to the TestExporter)
  const int num_logs = 5;
  for (int i = 0; i < num_logs; i++)
  {
    auto recordable = processor.MakeRecordable();
    recordable->SetBody("Log Body");
    processor.OnReceive(std::move(recordable));

    // Verify that the batch of 1 log record sent by processor matches what exporter received
    EXPECT_EQ(1, batch_size_received);
  }

  // Test whether the processor's log sent matches the log record received by the exporter
  EXPECT_EQ(logs_received->size(), num_logs);
  for (int i = 0; i < num_logs; i++)
  {
    EXPECT_EQ("Log Body", logs_received->at(i)->GetBody());
  }
}

// Tests behavior when calling the processor's ShutDown() multiple times
TEST(SimpleLogProcessorTest, ShutdownCalledOnce)
{
  // Create a TestExporter
  int num_shutdowns = 0;

  std::unique_ptr<TestExporter> exporter(new TestExporter(&num_shutdowns, nullptr, nullptr));

  // Create a processor with the previous test exporter
  SimpleLogProcessor processor(std::move(exporter));

  // The first time processor shutdown is called
  EXPECT_EQ(0, num_shutdowns);
  EXPECT_EQ(true, processor.Shutdown());
  EXPECT_EQ(1, num_shutdowns);

  EXPECT_EQ(true, processor.Shutdown());
  // Processor::ShutDown(), even if called more than once, should only shutdown exporter once
  EXPECT_EQ(1, num_shutdowns);
}

// A test exporter that always returns failure when shut down
class FailShutDownExporter final : public LogExporter
{
public:
  FailShutDownExporter() {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<Recordable>(new LogRecord());
  }

  ExportResult Export(const nostd::span<std::unique_ptr<Recordable>> &records) noexcept override
  {
    return ExportResult::kSuccess;
  }

  bool Shutdown(std::chrono::microseconds timeout) noexcept override { return false; }
};

// Tests for when when processor should fail to shutdown
TEST(SimpleLogProcessorTest, ShutDownFail)
{
  std::unique_ptr<FailShutDownExporter> exporter(new FailShutDownExporter());
  SimpleLogProcessor processor(std::move(exporter));

  // Expect failure result when exporter fails to shutdown
  EXPECT_EQ(false, processor.Shutdown());
}
#endif
