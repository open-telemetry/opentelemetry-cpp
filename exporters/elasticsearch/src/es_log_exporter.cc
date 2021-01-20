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
#include "opentelemetry/exporters/elasticsearch/es_log_recordable.h"

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
   * Creates a response handler, that by default doesn't display to console
   */
  ResponseHandler(bool console_debug = false) : console_debug_{console_debug} {}

  /**
   * Automatically called when the response is received, store the body into a string and notify any
   * threads blocked on this result
   */
  void OnResponse(http_client::Response &response) noexcept override
  {
    // Lock the private members so they can't be read while being modified
    {
      std::unique_lock<std::mutex> lk(mutex_);

      // Store the body of the request
      body_ = std::string(response.GetBody().begin(), response.GetBody().end());

      // Set the response_received_ flag to true and notify any threads waiting on this result
      response_received_ = true;
    }
    cv_.notify_all();
  }

  /**
   * A method the user calls to block their thread until the response is received. The longest
   * duration is the timeout of the request, set by SetTimeoutMs()
   */
  bool waitForResponse()
  {
    std::unique_lock<std::mutex> lk(mutex_);
    cv_.wait(lk);
    return response_received_;
  }

  /**
   * Returns the body of the response
   */
  std::string GetResponseBody()
  {
    // Lock so that body_ can't be written to while returning it
    std::unique_lock<std::mutex> lk(mutex_);
    return body_;
  }

  // Callback method when an http event occurs
  void OnEvent(http_client::SessionState state,
               opentelemetry::nostd::string_view reason) noexcept override
  {
    // If any failure event occurs, release the condition variable to unblock main thread
    switch (state)
    {
      case http_client::SessionState::ConnectFailed:
        if (console_debug_)
          std::cout << "Connection to elasticsearch failed\n";
        cv_.notify_all();
        break;
      case http_client::SessionState::SendFailed:
        if (console_debug_)
          std::cout << "Request failed to be sent to elasticsearch\n";
        cv_.notify_all();
        break;
      case http_client::SessionState::TimedOut:
        if (console_debug_)
          std::cout << "Request to elasticsearch timed out\n";
        cv_.notify_all();
        break;
      case http_client::SessionState::NetworkError:
        if (console_debug_)
          std::cout << "Network error to elasticsearch\n";
        cv_.notify_all();
        break;
    }
  }

private:
  // Define a condition variable and mutex
  std::condition_variable cv_;
  std::mutex mutex_;

  // Whether the response from Elasticsearch has been received
  bool response_received_ = false;

  // A string to store the response body
  std::string body_ = "";

  // Whether to print the results from the callback
  bool console_debug_ = false;
};

ElasticsearchLogExporter::ElasticsearchLogExporter()
    : options_{ElasticsearchExporterOptions()},
      session_manager_{new ext::http::client::curl::SessionManager()}
{}

ElasticsearchLogExporter::ElasticsearchLogExporter(const ElasticsearchExporterOptions &options)
    : options_{options}, session_manager_{new ext::http::client::curl::SessionManager()}
{}

std::unique_ptr<sdklogs::Recordable> ElasticsearchLogExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdklogs::Recordable>(new ElasticSearchRecordable);
}

sdklogs::ExportResult ElasticsearchLogExporter::Export(
    const nostd::span<std::unique_ptr<sdklogs::Recordable>> &records) noexcept
{
  // Return failure if this exporter has been shutdown
  if (is_shutdown_)
  {
    if (options_.console_debug_)
    {
      std::cout << "Export failed, exporter is shutdown" << std::endl;
    }

    return sdklogs::ExportResult::kFailure;
  }

  // Create a connection to the ElasticSearch instance
  auto session = session_manager_->CreateSession(options_.host_, options_.port_);
  auto request = session->CreateRequest();

  // Populate the request with headers and methods
  request->SetUri(options_.index_ + "/_bulk?pretty");
  request->SetMethod(http_client::Method::Post);
  request->AddHeader("Content-Type", "application/json");
  request->SetTimeoutMs(std::chrono::milliseconds(1000 * options_.response_timeout_));

  // Create the request body
  std::string body = "";
  for (auto &record : records)
  {
    // Append {"index":{}} before JSON body, which tells Elasticsearch to write to index specified
    // in URI
    body += "{\"index\" : {}}\n";

    // Add the context of the Recordable
    auto json_record = std::unique_ptr<ElasticSearchRecordable>(
        static_cast<ElasticSearchRecordable *>(record.release()));
    body += json_record->GetJSON().dump() + "\n";
  }
  std::vector<uint8_t> body_vec(body.begin(), body.end());
  request->SetBody(body_vec);

  // Send the request
  std::unique_ptr<ResponseHandler> handler(new ResponseHandler(options_.console_debug_));
  session->SendRequest(*handler);

  // Wait for the response to be received
  if (options_.console_debug_)
  {
    std::cout << "waiting for response from Elasticsearch (timeout = " << options_.response_timeout_
              << " seconds)" << std::endl;
  }
  bool write_successful = handler->waitForResponse();

  // End the session
  session->FinishSession();

  // If an error occurred with the HTTP request
  if (!write_successful)
  {
    // TODO: retry logic
    return sdklogs::ExportResult::kFailure;
  }

  // Parse the response output to determine if Elasticsearch consumed it correctly
  std::string responseBody = handler->GetResponseBody();
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
  is_shutdown_ = true;

  // Shutdown the session manager
  session_manager_->CancelAllSessions();
  session_manager_->FinishAllSessions();

  return true;
}
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
