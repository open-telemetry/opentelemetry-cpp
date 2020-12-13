/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "opentelemetry/exporters/elasticsearch/es_log_exporter.h"

namespace nostd       = opentelemetry::nostd;
namespace sdklogs     = opentelemetry::sdk::logs;
namespace http_client = opentelemetry::ext::http::client;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{
/**
 * This class handles the response message from the Elasticsearch request
 */
class ResponseHandler : public http_client::EventHandler
{
public:
  /**
   * Automatically called when the response is received, store the body into a string and notify any
   * threads blocked on this result
   */
  void OnResponse(http_client::Response &response) noexcept override
  {
    // Store the body of the request
    body_ = std::string(response.GetBody().begin(), response.GetBody().end());

    // Set the response_received_ flag to true and notify any threads waiting on this result
    response_received_ = true;
    cv_.notify_all();
  }

  /**
   * A method the user calls to block their thread until the response is received, or the timeout is
   * exceeded.
   */
  bool waitForResponse(unsigned int timeoutSec = 1)
  {
    std::mutex mutex_;
    std::unique_lock<std::mutex> lk(mutex_);
    cv_.wait_for(lk, std::chrono::milliseconds(1000 * timeoutSec));
    return response_received_;
  }

  /**
   * Returns the body of the response
   */
  std::string GetResponseBody()
  {
    if (!response_received_)
      return "No response";

    return body_;
  }

  // Virtual method definition that isn't used
  void OnEvent(http_client::SessionState state,
               opentelemetry::nostd::string_view reason) noexcept override
  {}

private:
  // Define a mutex and condition variable
  std::condition_variable cv_;

  // Whether the response from Elasticsearch has been received
  bool response_received_ = false;

  // A string to store the response body
  std::string body_ = "";
};

ElasticsearchLogExporter::ElasticsearchLogExporter() : options_(ElasticsearchExporterOptions()) {}

ElasticsearchLogExporter::ElasticsearchLogExporter(const ElasticsearchExporterOptions &options)
    : options_{options}
{}

sdklogs::ExportResult ElasticsearchLogExporter::Export(
    const nostd::span<std::unique_ptr<opentelemetry::logs::LogRecord>> &records) noexcept
{
  // Return failure if this exporter has been shutdown
  if (isShutdown_)
  {
    if (options_.console_debug_)
      std::cout << "Export failed, exporter is shutdown" << std::endl;

    return sdklogs::ExportResult::kFailure;
  }

  // Create a json array to store all the JSON log records
  json logs = json::array();

  for (auto &record : records)
  {
    // Convert the log record to a JSON object, and store in json array
    logs.emplace_back(RecordToJSON(std::move(record)));
  }

  // Create a connection to the ElasticSearch instance
  opentelemetry::ext::http::client::curl::SessionManager session_manager;
  auto session = session_manager.CreateSession(options_.host_, options_.port_);
  auto request = session->CreateRequest();

  // Populate the request with headers and methods
  request->SetUri(options_.index_ + "/_bulk?pretty");
  request->SetMethod(http_client::Method::Post);
  request->AddHeader("Content-Type", "application/json");
  request->SetTimeoutMs(std::chrono::milliseconds(1000 * options_.response_timeout_));

  // Add the request body
  std::string body = "";
  for (int i = 0; i < logs.size(); i++)
  {
    // Append {"index":{}} before JSON body, which tells Elasticsearch to write to index specified
    // in URI
    body += "{\"index\" : {}}\n";

    // Add the context of the Log Record
    body += logs[i].dump() + "\n";
  }
  std::vector<uint8_t> body_vec(body.begin(), body.end());
  request->SetBody(body_vec);

  // Send the request
  std::unique_ptr<ResponseHandler> handler(new ResponseHandler());
  session->SendRequest(*handler);

  // Wait for the response to be received
  if (options_.console_debug_)
    std::cout << "waiting for response from Elasticsearch (timeout = " << options_.response_timeout_
              << " seconds)" << std::endl;
  bool receivedResponse = handler->waitForResponse(options_.response_timeout_);

  // End the session
  session->FinishSession();
  session_manager.CancelAllSessions();
  session_manager.FinishAllSessions();

  // If the response was never received
  if (!receivedResponse)
  {
    // TODO: retry logic

    if (options_.console_debug_)
      std::cout << "Request exceeded timeout, aborting..." << std::endl;

    return sdklogs::ExportResult::kFailure;
  }

  // Parse the response output to determine if the request wasen't successful
  std::string responseBody = handler->GetResponseBody();
  std::cout << responseBody << std::endl;
  if (responseBody.find("\"failed\" : 0") == std::string::npos)
  {
    if (options_.console_debug_)
    {
      std::cout << "Logs were not written to Elasticsearch correctly, response body:" << std::endl;
      std::cout << responseBody << std::endl;
    }

    // TODO: Retry logic
    return sdklogs::ExportResult::kFailure;
  }

  return sdklogs::ExportResult::kSuccess;
}

bool ElasticsearchLogExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  isShutdown_ = true;

  return true;
}

json ElasticsearchLogExporter::RecordToJSON(std::unique_ptr<opentelemetry::logs::LogRecord> record)
{
  // Create a json object to store the LogRecord information in
  json log;

  // Write the simple fields
  log["timestamp"] = record->timestamp.time_since_epoch().count();
  log["name"]      = record->name;
  log["body"]      = record->body;

  // Write the severity by converting it from its enum to a string
  static opentelemetry::nostd::string_view severityStringMap_[25] = {
      "kInvalid", "kTrace",  "kTrace2", "kTrace3", "kTrace4", "kDebug",  "kDebug2",
      "kDebug3",  "kDebug4", "kInfo",   "kInfo2",  "kInfo3",  "kInfo4",  "kWarn",
      "kWarn2",   "kWarn3",  "kWarn4",  "kError",  "kError2", "kError3", "kError4",
      "kFatal",   "kFatal2", "kFatal3", "kFatal4"};
  log["severity"] = severityStringMap_[static_cast<int>(record->severity)];

  // Convert the resources into a JSON object
  json resource;
  if (record->resource != nullptr)
  {
    record->resource->ForEachKeyValue([&](nostd::string_view key,
                                          common::AttributeValue value) noexcept {
      resource[key.data()] = ValueToString(value);
      return true;
    });

    // Push the attributes JSON object into the main JSON under the "resource" key
    log.push_back({"resource", resource});
  }

  // Convert the attributes into a JSON object
  json attributes;
  if (record->attributes != nullptr)
  {
    record->attributes->ForEachKeyValue([&](nostd::string_view key,
                                            common::AttributeValue value) noexcept {
      attributes[key.data()] = ValueToString(value);
      return true;
    });

    // Push the attributes JSON object into the main JSON under the "attributes" key
    log.push_back({"attributes", attributes});
  }

  // Convert traceid, spanid, and traceflags into strings
  char trace_buf[32];
  record->trace_id.ToLowerBase16(trace_buf);
  log["traceid"] = std::string(trace_buf, sizeof(trace_buf));

  char span_buf[16];
  record->span_id.ToLowerBase16(span_buf);
  log["spanid"] = std::string(span_buf, sizeof(span_buf));

  char flag_buf[2];
  record->trace_flags.ToLowerBase16(flag_buf);
  log["traceflags"] = std::string(flag_buf, sizeof(flag_buf));

  return log;
}

}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
