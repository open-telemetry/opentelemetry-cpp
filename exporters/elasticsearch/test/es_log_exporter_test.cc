#include "opentelemetry/exporters/elasticsearch/es_log_exporter.h"
#include "opentelemetry/ext/http/server/http_server.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/log_record.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/simple_log_processor.h"

#include <gtest/gtest.h>
#include <iostream>

namespace sdklogs       = opentelemetry::sdk::logs;
namespace logs_api      = opentelemetry::logs;
namespace nostd         = opentelemetry::nostd;
namespace logs_exporter = opentelemetry::exporter::logs;

// Attempt to write a log to an invalid host/port, test that the Export() returns failure
TEST(ElasticsearchLogsExporterTests, InvalidEndpoint)
{
  // Create invalid connection options for the elasticsearch exporter
  logs_exporter::ElasticsearchExporterOptions options("localhost", -1);

  // Create an elasticsearch exporter
  auto exporter =
      std::unique_ptr<sdklogs::LogExporter>(new logs_exporter::ElasticsearchLogExporter(options));

  // Create a log record and send to the exporter
  auto record = exporter->MakeRecordable();
  auto result = exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));

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
  auto record = exporter->MakeRecordable();
  auto result = exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));

  // Ensure the return value is failure
  ASSERT_EQ(result, sdklogs::ExportResult::kFailure);
}

// Test the elasticsearch recordable object
TEST(ElasticsearchLogsExporterTests, RecordableCreation)
{
  // Create an elasticsearch exporter
  auto exporter =
      std::unique_ptr<sdklogs::LogExporter>(new logs_exporter::ElasticsearchLogExporter);

  // Create a recordable
  auto record = exporter->MakeRecordable();
  record->SetName("Timeout Log");
  record->SetSeverity(logs_api::Severity::kFatal);
  record->SetTimestamp(std::chrono::system_clock::now());
  record->SetBody("Body of the log message");

  // Attributes and resource support different types
  record->SetAttribute("key0", false);
  record->SetAttribute("key1", "1");
  record->SetResource("key2", 2);
  record->SetResource("key3", 3.142);

  exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));
}