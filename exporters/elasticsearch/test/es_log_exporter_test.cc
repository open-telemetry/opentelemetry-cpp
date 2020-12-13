#include "opentelemetry/exporters/elasticsearch/es_log_exporter.h"
#include "opentelemetry/ext/http/server/http_server.h"
#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/simple_log_processor.h"

#include <gtest/gtest.h>
#include <iostream>

namespace sdklogs       = opentelemetry::sdk::logs;
namespace logs_api      = opentelemetry::logs;
namespace nostd         = opentelemetry::nostd;
namespace logs_exporter = opentelemetry::exporter::logs;

// Attempt to write a log to an invalid host/port, test that the timeout works properly
TEST(ElasticsearchLogsExporterTests, InvalidEndpoint)
{
  // Create options for the elasticsearch exporter
  logs_exporter::ElasticsearchExporterOptions options("localhost", -1, "logs", 5, true);
  options.response_timeout_ = 10;  // Wait 10 seconds to receive a response

  // Create an elasticsearch exporter
  auto exporter =
      std::unique_ptr<sdklogs::LogExporter>(new logs_exporter::ElasticsearchLogExporter(options));

  // Create a log record
  auto record  = std::unique_ptr<logs_api::LogRecord>(new logs_api::LogRecord());
  record->name = "Timeout Log";
  std::map<std::string, std::string> m = {{"key1", "value1"}, {"key2", "value2"}};

  // Write the log record to the exporter, and time the duration
  nostd::span<std::unique_ptr<opentelemetry::logs::LogRecord>> batch(&record, 1);
  auto t1     = std::chrono::high_resolution_clock::now();
  auto result = exporter->Export(batch);
  auto t2     = std::chrono::high_resolution_clock::now();

  // Ensure the timeout is within the range of the timeout specified ([10, 10 + 1] seconds)
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
  ASSERT_TRUE((duration >= options.response_timeout_) &&
              (duration < options.response_timeout_ + 1));

  // Ensure the return value is failure
  ASSERT_EQ(result, sdklogs::ExportResult::kFailure);
}

// Test that when the exporter is shutdown, any call to Export should return failure
TEST(ElasticsearchLogsExporterTests, Shutdown)
{
  // Create an elasticsearch exporter and immediately shut it down
  auto exporter =
      std::unique_ptr<sdklogs::LogExporter>(new logs_exporter::ElasticsearchLogExporter);
  bool shutdownResult = exporter->Shutdown();
  ASSERT_TRUE(shutdownResult);

  // Write a log to the shutdown exporter
  auto record = std::unique_ptr<logs_api::LogRecord>(new logs_api::LogRecord());
  nostd::span<std::unique_ptr<opentelemetry::logs::LogRecord>> batch(&record, 1);
  auto result = exporter->Export(batch);

  // Ensure the return value is failure
  ASSERT_EQ(result, sdklogs::ExportResult::kFailure);
}
