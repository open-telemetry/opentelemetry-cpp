// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include <sstream>  // std::stringstream

#  include <mutex>
#  include "opentelemetry/exporters/elasticsearch/es_log_exporter.h"
#  include "opentelemetry/exporters/elasticsearch/es_log_recordable.h"
#  include "opentelemetry/sdk_config.h"

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

      if (console_debug_)
      {
        OTEL_INTERNAL_LOG_DEBUG(
            "[ES Log Exporter] Got response from Elasticsearch,  response body: " << body_);
      }

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
  void OnEvent(http_client::SessionState state, nostd::string_view reason) noexcept override
  {
    // If any failure event occurs, release the condition variable to unblock main thread
    switch (state)
    {
      case http_client::SessionState::ConnectFailed:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Connection to elasticsearch failed");
        cv_.notify_all();
        break;
      case http_client::SessionState::SendFailed:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Request failed to be sent to elasticsearch");
        cv_.notify_all();
        break;
      case http_client::SessionState::TimedOut:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Request to elasticsearch timed out");
        cv_.notify_all();
        break;
      case http_client::SessionState::NetworkError:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Network error to elasticsearch");
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

#  ifdef ENABLE_ASYNC_EXPORT
/**
 * This class handles the async response message from the Elasticsearch request
 */
class AsyncResponseHandler : public http_client::EventHandler
{
public:
  /**
   * Creates a response handler, that by default doesn't display to console
   */
  AsyncResponseHandler(
      std::shared_ptr<ext::http::client::Session> session,
      std::function<bool(opentelemetry::sdk::common::ExportResult)> &&result_callback,
      bool console_debug = false)
      : console_debug_{console_debug},
        session_{std::move(session)},
        result_callback_{std::move(result_callback)}
  {}

  /**
   * Cleans up the session in the destructor.
   */
  ~AsyncResponseHandler() { session_->FinishSession(); }

  /**
   * Automatically called when the response is received
   */
  void OnResponse(http_client::Response &response) noexcept override
  {

    // Store the body of the response
    body_ = std::string(response.GetBody().begin(), response.GetBody().end());
    if (console_debug_)
    {
      OTEL_INTERNAL_LOG_DEBUG(
          "[ES Log Exporter] Got response from Elasticsearch,  response body: " << body_);
    }
    if (body_.find("\"failed\" : 0") == std::string::npos)
    {
      OTEL_INTERNAL_LOG_ERROR(
          "[ES Log Exporter] Logs were not written to Elasticsearch correctly, response body: "
          << body_);
      result_callback_(sdk::common::ExportResult::kFailure);
    }
    else
    {
      result_callback_(sdk::common::ExportResult::kSuccess);
    }
  }

  // Callback method when an http event occurs
  void OnEvent(http_client::SessionState state, nostd::string_view reason) noexcept override
  {
    bool need_stop = false;
    switch (state)
    {
      case http_client::SessionState::CreateFailed:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Create request to elasticsearch failed");
        need_stop = true;
        break;
      case http_client::SessionState::ConnectFailed:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Connection to elasticsearch failed");
        need_stop = true;
        break;
      case http_client::SessionState::SendFailed:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Request failed to be sent to elasticsearch");
        need_stop = true;
        break;
      case http_client::SessionState::SSLHandshakeFailed:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] SSL handshake to elasticsearch failed");
        need_stop = true;
        break;
      case http_client::SessionState::TimedOut:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Request to elasticsearch timed out");
        need_stop = true;
        break;
      case http_client::SessionState::NetworkError:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Network error to elasticsearch");
        need_stop = true;
        break;
      case http_client::SessionState::Cancelled:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Request to elasticsearch cancelled");
        need_stop = true;
        break;
      default:
        break;
    }
    if (need_stop)
    {
      result_callback_(sdk::common::ExportResult::kFailure);
    }
  }

private:
  // Stores the session object for the request
  std::shared_ptr<ext::http::client::Session> session_;
  // Callback to call to on receiving events
  std::function<bool(opentelemetry::sdk::common::ExportResult)> result_callback_;

  // A string to store the response body
  std::string body_ = "";

  // Whether to print the results from the callback
  bool console_debug_ = false;
};
#  endif

ElasticsearchLogExporter::ElasticsearchLogExporter()
    : options_{ElasticsearchExporterOptions()},
      http_client_{new ext::http::client::curl::HttpClient()}
{}

ElasticsearchLogExporter::ElasticsearchLogExporter(const ElasticsearchExporterOptions &options)
    : options_{options}, http_client_{new ext::http::client::curl::HttpClient()}
{}

std::unique_ptr<sdklogs::Recordable> ElasticsearchLogExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdklogs::Recordable>(new ElasticSearchRecordable);
}

sdk::common::ExportResult ElasticsearchLogExporter::Export(
    const nostd::span<std::unique_ptr<sdklogs::Recordable>> &records) noexcept
{
  // Return failure if this exporter has been shutdown
  if (isShutdown())
  {
    OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Exporting "
                            << records.size() << " log(s) failed, exporter is shutdown");
    return sdk::common::ExportResult::kFailure;
  }

  // Create a connection to the ElasticSearch instance
  auto session = http_client_->CreateSession(options_.host_ + std::to_string(options_.port_));
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

#  ifdef ENABLE_ASYNC_EXPORT
  // Send the request
  std::size_t span_count = records.size();
  auto handler           = std::make_shared<AsyncResponseHandler>(
      session,
      [span_count](opentelemetry::sdk::common::ExportResult result) {
        if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
        {
          OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] ERROR: Export "
                                  << span_count
                                  << " trace span(s) error: " << static_cast<int>(result));
        }
        else
        {
          OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] DEBUG: Export " << span_count
                                                                     << " trace span(s) success");
        }
        return true;
      },
      options_.console_debug_);
  session->SendRequest(handler);
  return sdk::common::ExportResult::kSuccess;
#  else
  // Send the request
  auto handler = std::make_shared<ResponseHandler>(options_.console_debug_);
  session->SendRequest(handler);

  // Wait for the response to be received
  if (options_.console_debug_)
  {
    OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] waiting for response from Elasticsearch (timeout = "
                            << options_.response_timeout_ << " seconds)");
  }
  bool write_successful = handler->waitForResponse();

  // End the session
  session->FinishSession();

  // If an error occurred with the HTTP request
  if (!write_successful)
  {
    // TODO: retry logic
    return sdk::common::ExportResult::kFailure;
  }

  // Parse the response output to determine if Elasticsearch consumed it correctly
  std::string responseBody = handler->GetResponseBody();
  if (responseBody.find("\"failed\" : 0") == std::string::npos)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[ES Log Exporter] Logs were not written to Elasticsearch correctly, response body: "
        << responseBody);
    // TODO: Retry logic
    return sdk::common::ExportResult::kFailure;
  }

  return sdk::common::ExportResult::kSuccess;
#  endif
}

bool ElasticsearchLogExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_shutdown_ = true;

  // Shutdown the session manager
  http_client_->CancelAllSessions();
  http_client_->FinishAllSessions();

  return true;
}

bool ElasticsearchLogExporter::isShutdown() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return is_shutdown_;
}
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
