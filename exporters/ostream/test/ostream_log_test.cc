// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include <array>
#  include "opentelemetry/exporters/ostream/log_exporter.h"
#  include "opentelemetry/logs/provider.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/sdk/logs/logger_provider.h"
#  include "opentelemetry/sdk/logs/simple_log_processor.h"

#  include <gtest/gtest.h>
#  include <iostream>

namespace sdklogs      = opentelemetry::sdk::logs;
namespace logs_api     = opentelemetry::logs;
namespace nostd        = opentelemetry::nostd;
namespace exporterlogs = opentelemetry::exporter::logs;
namespace common       = opentelemetry::common;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{

// Test that when OStream Log exporter is shutdown, no logs should be sent to stream
TEST(OStreamLogExporter, Shutdown)
{
  auto exporter = std::unique_ptr<sdklogs::LogExporter>(new exporterlogs::OStreamLogExporter);

  // Save cout's original buffer here
  std::streambuf *original = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::stringstream output;
  std::cout.rdbuf(output.rdbuf());

  EXPECT_TRUE(exporter->Shutdown());

  // After processor/exporter is shutdown, no logs should be sent to stream
  auto record = exporter->MakeRecordable();
  record->SetBody("Log record not empty");
  exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));

  // Restore original stringstream buffer
  std::cout.rdbuf(original);
  std::string err_message =
      "[Ostream Log Exporter] Exporting 1 log(s) failed, exporter is shutdown";
  EXPECT_TRUE(output.str().find(err_message) != std::string::npos);
}

// ---------------------------------- Print to cout -------------------------

// Testing what a default log record that has no values changed will print out
// This function tests MakeRecordable() as well as Export().
TEST(OstreamLogExporter, DefaultLogRecordToCout)
{
  auto exporter =
      std::unique_ptr<sdklogs::LogExporter>(new exporterlogs::OStreamLogExporter(std::cout));

  // Save cout's original buffer here
  std::streambuf *original = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::stringstream output;
  std::cout.rdbuf(output.rdbuf());

  // Pass a default recordable created by the exporter to be exported
  auto log_record = exporter->MakeRecordable();
  exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&log_record, 1));

  // Restore cout's original stringstream
  std::cout.rdbuf(original);

  std::vector<std::string> expected_output{
      "{\n"
      "  timestamp     : 0\n"
      "  severity_num  : 0\n"
      "  severity_text : INVALID\n"
      "  body          : \n",
      "  resource      : \n",
      "telemetry.sdk.version: " OPENTELEMETRY_VERSION "\n",
      "telemetry.sdk.name: opentelemetry\n",
      "telemetry.sdk.language: cpp\n",
      "  attributes    : \n"
      "  trace_id      : 00000000000000000000000000000000\n"
      "  span_id       : 0000000000000000\n"
      "  trace_flags   : 00\n"
      "}\n"};

  for (auto &expected : expected_output)
  {
    ASSERT_NE(output.str().find(expected), std::string::npos);
  }
}

// Testing what a log record with only the "timestamp", "severity", "name" and "message" fields set,
// will print out
TEST(OStreamLogExporter, SimpleLogToCout)
{
  // Initialize an Ostream exporter to std::cout
  auto exporter =
      std::unique_ptr<sdklogs::LogExporter>(new exporterlogs::OStreamLogExporter(std::cout));

  // Save original stream buffer, then redirect cout to our new stream buffer
  std::streambuf *original = std::cout.rdbuf();
  std::stringstream output;
  std::cout.rdbuf(output.rdbuf());

  // Pass a default recordable created by the exporter to be exported
  // Create a log record and manually timestamp, severity, name, message
  common::SystemTimestamp now(std::chrono::system_clock::now());

  auto record = std::unique_ptr<sdklogs::Recordable>(new sdklogs::LogRecord());
  record->SetTimestamp(now);
  record->SetSeverity(logs_api::Severity::kTrace);  // kTrace has enum value of 1
  record->SetBody("Message");

  // Log a record to cout
  exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));

  // Reset cout's original stringstream buffer
  std::cout.rdbuf(original);

  std::vector<std::string> expected_output{
      "{\n"
      "  timestamp     : " +
          std::to_string(now.time_since_epoch().count()) +
          "\n"
          "  severity_num  : 1\n"
          "  severity_text : TRACE\n"
          "  body          : Message\n",
      "  resource      : \n",
      "telemetry.sdk.version: " OPENTELEMETRY_VERSION "\n",
      "telemetry.sdk.name: opentelemetry\n",
      "telemetry.sdk.language: cpp\n",
      "  attributes    : \n"
      "  trace_id      : 00000000000000000000000000000000\n"
      "  span_id       : 0000000000000000\n"
      "  trace_flags   : 00\n"
      "}\n"};

  for (auto &expected : expected_output)
  {
    ASSERT_NE(output.str().find(expected), std::string::npos);
  }
}

// ---------------------------------- Print to cerr --------------------------

// Testing what a log record with only the "resource" and "attributes" fields
// (i.e. KeyValueIterable types) set with primitive types, will print out
TEST(OStreamLogExporter, LogWithStringAttributesToCerr)
{
  // Initialize an Ostream exporter to cerr
  auto exporter =
      std::unique_ptr<sdklogs::LogExporter>(new exporterlogs::OStreamLogExporter(std::cerr));

  // Save original stream buffer, then redirect cout to our new stream buffer
  std::streambuf *original = std::cerr.rdbuf();
  std::stringstream stdcerrOutput;
  std::cerr.rdbuf(stdcerrOutput.rdbuf());

  // Pass a recordable created by the exporter to be exported
  auto record = exporter->MakeRecordable();

  // Set resources for this log record only of type <string, string>
  auto resource = opentelemetry::sdk::resource::Resource::Create({{"key1", "val1"}});
  record->SetResource(resource);

  // Set attributes to this log record of type <string, AttributeValue>
  record->SetAttribute("a", true);

  // Log record to cerr
  exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));

  // Reset cerr's original stringstream buffer
  std::cerr.rdbuf(original);

  std::vector<std::string> expected_output{
      "{\n"
      "  timestamp     : 0\n"
      "  severity_num  : 0\n"
      "  severity_text : INVALID\n"
      "  body          : \n",
      "  resource      : \n",
      "telemetry.sdk.version: " OPENTELEMETRY_VERSION "\n",
      "telemetry.sdk.name: opentelemetry\n",
      "telemetry.sdk.language: cpp\n",
      "service.name: unknown_service\n",
      "key1: val1\n",
      "  attributes    : \n",
      "\ta: 1\n",
      "  trace_id      : 00000000000000000000000000000000\n"
      "  span_id       : 0000000000000000\n"
      "  trace_flags   : 00\n"
      "}\n"};

  for (auto &expected : expected_output)
  {
    ASSERT_NE(stdcerrOutput.str().find(expected), std::string::npos);
  }
}

// ---------------------------------- Print to clog -------------------------

// Testing what a log record with only the "resource", and "attributes" fields
// (i.e. KeyValueIterable types), set with 2D arrays as values, will print out
TEST(OStreamLogExporter, LogWithVariantTypesToClog)
{

  // Initialize an Ostream exporter to cerr
  auto exporter =
      std::unique_ptr<sdklogs::LogExporter>(new exporterlogs::OStreamLogExporter(std::clog));

  // Save original stream buffer, then redirect cout to our new stream buffer
  std::streambuf *original = std::clog.rdbuf();
  std::stringstream stdclogOutput;
  std::clog.rdbuf(stdclogOutput.rdbuf());

  // Pass a recordable created by the exporter to be exported
  auto record = exporter->MakeRecordable();

  // Set resources for this log record of only integer types as the value
  std::array<int, 3> array1 = {1, 2, 3};
  nostd::span<int> data1{array1.data(), array1.size()};

  auto resource = opentelemetry::sdk::resource::Resource::Create({{"res1", data1}});
  record->SetResource(resource);

  // Set resources for this log record of bool types as the value
  // e.g. key/value is a par of type <string, array of bools>
  std::array<bool, 3> array = {false, true, false};
  record->SetAttribute("attr1", nostd::span<bool>{array.data(), array.size()});

  // Log a record to clog
  exporter->Export(nostd::span<std::unique_ptr<sdklogs::Recordable>>(&record, 1));

  // Reset clog's original stringstream buffer
  std::clog.rdbuf(original);

  std::vector<std::string> expected_output{
      "{\n"
      "  timestamp     : 0\n"
      "  severity_num  : 0\n"
      "  severity_text : INVALID\n"
      "  body          : \n",
      "  resource      : \n",
      "service.name: unknown_service\n",
      "telemetry.sdk.version: " OPENTELEMETRY_VERSION "\n",
      "telemetry.sdk.name: opentelemetry\n",
      "telemetry.sdk.language: cpp\n",
      "res1: [1,2,3]\n",
      "attributes    : \n",
      "\tattr1: [0,1,0]\n"
      "  trace_id      : 00000000000000000000000000000000\n"
      "  span_id       : 0000000000000000\n"
      "  trace_flags   : 00\n"
      "}\n"};

  for (auto &expected : expected_output)
  {
    ASSERT_NE(stdclogOutput.str().find(expected), std::string::npos);
  }
}

// // ---------------------------------- Integration Tests -------------------------

// Test using the simple log processor and ostream exporter to cout
// and use the rest of the logging pipeline (Logger, LoggerProvider, Provider) as well
TEST(OStreamLogExporter, IntegrationTest)
{
  // Initialize a logger
  auto exporter    = std::unique_ptr<sdklogs::LogExporter>(new exporterlogs::OStreamLogExporter);
  auto sdkProvider = std::shared_ptr<sdklogs::LoggerProvider>(new sdklogs::LoggerProvider());
  sdkProvider->AddProcessor(
      std::unique_ptr<sdklogs::LogProcessor>(new sdklogs::SimpleLogProcessor(std::move(exporter))));
  auto apiProvider = nostd::shared_ptr<logs_api::LoggerProvider>(sdkProvider);
  auto provider    = nostd::shared_ptr<logs_api::LoggerProvider>(apiProvider);
  logs_api::Provider::SetLoggerProvider(provider);
  const std::string schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger = logs_api::Provider::GetLoggerProvider()->GetLogger(
      "Logger", "", "opentelelemtry_library", "", schema_url);

  // Back up cout's streambuf
  std::streambuf *original = std::cout.rdbuf();

  // Redirect cout to our string stream
  std::stringstream stdcoutOutput;
  std::cout.rdbuf(stdcoutOutput.rdbuf());

  // Write a log to ostream exporter
  common::SystemTimestamp now(std::chrono::system_clock::now());
  logger->Log(logs_api::Severity::kDebug, "Hello", {}, {}, {}, {}, now);

  // Restore cout's original streambuf
  std::cout.rdbuf(original);

  // Compare actual vs expected outputs
  std::vector<std::string> expected_output{
      "{\n"
      "  timestamp     : " +
          std::to_string(now.time_since_epoch().count()) +
          "\n"
          "  severity_num  : 5\n"
          "  severity_text : DEBUG\n"
          "  body          : Hello\n",
      "  resource      : \n",
      "telemetry.sdk.version: " OPENTELEMETRY_VERSION "\n",
      "service.name: unknown_service\n",
      "telemetry.sdk.name: opentelemetry\n",
      "telemetry.sdk.language: cpp\n",
      "  attributes    : \n"
      "  trace_id      : 00000000000000000000000000000000\n"
      "  span_id       : 0000000000000000\n"
      "  trace_flags   : 00\n"
      "}\n"};

  for (auto &expected : expected_output)
  {
    ASSERT_NE(stdcoutOutput.str().find(expected), std::string::npos);
  }
}

}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
