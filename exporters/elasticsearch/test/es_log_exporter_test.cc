#include "opentelemetry/exporters/elasticsearch/es_log_exporter.h"
#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/simple_log_processor.h"
#include "opentelemetry/ext/http/server/http_server.h"

#include <gtest/gtest.h>
#include <iostream>

namespace sdklogs  = opentelemetry::sdk::logs;
namespace logs_api = opentelemetry::logs;
namespace nostd    = opentelemetry::nostd;
namespace logs_exporter = opentelemetry::exporter::logs;

#define HTTP_PORT 19000

/**
 * Create a class that mocks an elasticsearch instance
 */ 
class ElasticsearchLogsExporterTests : public ::testing::Test, public HTTP_SERVER_NS::HttpRequestCallback
{
protected:
  HTTP_SERVER_NS::HttpServer server_;
  std::string server_address_;
  std::atomic<bool> is_setup_;
  std::atomic<bool> is_running_;
  std::vector<HTTP_SERVER_NS::HttpRequest> received_requests_;
  std::mutex mtx_requests;
  std::condition_variable cv_got_events;
  std::mutex cv_m;

public:
  ElasticsearchLogsExporterTests() : is_setup_(false), is_running_(false){};

  virtual void SetUp() override
  {
    if (is_setup_.exchange(true))
    {
      return;
    }
    int port = server_.addListeningPort(HTTP_PORT);
    std::ostringstream os;
    os << "localhost:" << port;
    server_address_ = "http://" + os.str() + "/simple/";
    server_.setServerName(os.str());
    server_.setKeepalive(false);
    server_.addHandler("/simple/", *this);
    server_.addHandler("/get/", *this);
    server_.addHandler("/post/", *this);
    server_.start();
    is_running_ = true;
  }

  virtual void TearDown() override
  {
    if (!is_setup_.exchange(false))
      return;
    server_.stop();
    is_running_ = false;
  }

  virtual int onHttpRequest(HTTP_SERVER_NS::HttpRequest const &request,
                            HTTP_SERVER_NS::HttpResponse &response) override
  {
    if (request.uri == "/logs/")
    {
      std::unique_lock<std::mutex> lk(mtx_requests);
      received_requests_.push_back(request);
      response.headers["Content-Type"] = "application/json";
      response.body                    = "{'k1':'v1', 'k2':'v2', 'k3':'v3'}";
      return 200;
    }
    return 404;
  }
};

// Attempt to write a log to an invalid host/port, test that the timeout works properly
TEST(ElasticsearchLogsExporterTests, InvalidEndpoint) {
    // Create options for the elasticsearch exporter
    logs_exporter::ElasticsearchExporterOptions options("invalidhost", -1);
    options.response_timeout_ = 10; // Wait 10 seconds to receive a response

    // Create an elasticsearch exporter
    auto exporter =
        std::unique_ptr<sdklogs::LogExporter>(new logs_exporter::ElasticsearchLogExporter(options));

    // Create a log record
    auto record  = std::unique_ptr<logs_api::LogRecord>(new logs_api::LogRecord());
    record->name = "Timeout Log";

    // Write the log record to the exporter, and time the duration
    nostd::span<std::unique_ptr<opentelemetry::logs::LogRecord>> batch(&record, 1);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto result = exporter->Export(batch);
    auto t2 = std::chrono::high_resolution_clock::now();

    // Ensure the timeout is within the range of the timeout specified ([10, 10 + 1] seconds)
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    ASSERT_TRUE((duration >= options.response_timeout_) && (duration < options.response_timeout_ + 1));

    // Ensure the return value is failure
    ASSERT_EQ(result, sdklogs::ExportResult::kFailure);
}

// Test that when the exporter is shutdown, any call to Export should return failure
TEST(ElasticsearchLogsExporterTests, Shutdown) {
    // Create an elasticsearch exporter and immediately shut it down
    auto exporter =
        std::unique_ptr<sdklogs::LogExporter>(new logs_exporter::ElasticsearchLogExporter);
    bool shutdownResult = exporter->Shutdown();
    ASSERT_TRUE(shutdownResult);

    // Write a log to the shutdown exporter
    auto record  = std::unique_ptr<logs_api::LogRecord>(new logs_api::LogRecord());
    nostd::span<std::unique_ptr<opentelemetry::logs::LogRecord>> batch(&record, 1);
    auto result = exporter->Export(batch);

    // Ensure the return value is failure
    ASSERT_EQ(result, sdklogs::ExportResult::kFailure);
}

// Write a log to the mock server, but gets a failure response
TEST(ElasticsearchLogsExporterTests, FailureResponseCode) {
    // Create an elasticsearch exporter with config options that specify the endpoint
    //     - host    = localhost
    //     - port    = HTTP_PORT
    //     - index   = logs
    //     - timeout = 5 seconds
    logs_exporter::ElasticsearchExporterOptions options("localhost", HTTP_PORT, "logs", 5);
    auto exporter =
        std::unique_ptr<sdklogs::LogExporter>(new logs_exporter::ElasticsearchLogExporter(options));

    // Create a log record with a name that the mock server is programmed to reject
    auto record = std::unique_ptr<logs_api::LogRecord>(new logs_api::LogRecord());
    record->name = "Bad Record!";
    nostd::span<std::unique_ptr<opentelemetry::logs::LogRecord>> batch(&record, 1);

    // Send the record to the mock server
    auto result = exporter->Export(batch);

    // Ensure the return value is failure
    ASSERT_EQ(result, sdklogs::ExportResult::kFailure);
}

