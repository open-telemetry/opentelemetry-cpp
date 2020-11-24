#include "opentelemetry/sdk/logs/simple_log_processor.h"
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

using namespace opentelemetry::sdk::logs;
using opentelemetry::logs::LogRecord;

/*
 * A test exporter that can return a vector of all the records it has received,
 * and keep track of the number of times its Shutdown() function was called.
 */
class TestExporter final : public LogExporter
{
public:
  TestExporter(int *shutdown_counter,
               std::shared_ptr<std::vector<std::string>> logs_received,
               int *batch_size_received)
      : shutdown_counter_(shutdown_counter),
        logs_received_(logs_received),
        batch_size_received(batch_size_received)
  {}

  // Stores the names of the log records this exporter receives to an internal list
  ExportResult Export(const std::vector<std::unique_ptr<LogRecord>> &records) noexcept override
  {
    *batch_size_received = records.size();
    for (auto &record : records)
    {
      logs_received_->push_back(record->name.data());
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
  std::shared_ptr<std::vector<std::string>> logs_received_;
  int *batch_size_received;
};

// Tests whether the simple processor successfully creates a batch of size 1
// and whether the contents of the record is sent to the exporter correctly
TEST(SimpleLogProcessorTest, SendReceivedLogsToExporter)
{
  // Create a simple processor with a TestExporter attached
  std::shared_ptr<std::vector<std::string>> logs_received(new std::vector<std::string>);
  int batch_size_received = 0;

  std::unique_ptr<TestExporter> exporter(
      new TestExporter(nullptr, logs_received, &batch_size_received));

  SimpleLogProcessor processor(std::move(exporter));

  // Send some log records to the processor (which should then send to the TestExporter)
  const int num_logs = 5;
  for (int i = 0; i < num_logs; i++)
  {
    auto record = std::unique_ptr<LogRecord>(new LogRecord());
    std::string s("Log name");
    s += std::to_string(i);
    record->name = s;

    processor.OnReceive(std::move(record));

    // Verify that the batch of 1 log record sent by processor matches what exporter received
    EXPECT_EQ(1, batch_size_received);
  }

  // Test whether the processor's log sent matches the log record received by the exporter
  EXPECT_EQ(logs_received->size(), num_logs);
  for (int i = 0; i < num_logs; i++)
  {
    std::string s("Log name");
    s += std::to_string(i);
    EXPECT_EQ(s, logs_received->at(i));
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

  // The second time processor shutdown is called
  EXPECT_EQ(false, processor.Shutdown());
  // Processor::ShutDown(), even if called more than once, should only shutdown exporter once
  EXPECT_EQ(1, num_shutdowns);
}

// A test exporter that always returns failure when shut down
class FailShutDownExporter final : public LogExporter
{
public:
  FailShutDownExporter() {}

  ExportResult Export(const std::vector<std::unique_ptr<LogRecord>> &records) noexcept override
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

  // Expect failure result when processor given a negative timeout allowed to shutdown
  EXPECT_EQ(false, processor.Shutdown(std::chrono::microseconds(-1)));
}
