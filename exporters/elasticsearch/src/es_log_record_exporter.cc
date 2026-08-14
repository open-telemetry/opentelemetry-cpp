// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <map>     // IWYU pragma: keep
#include <memory>  // IWYU pragma: keep
#include <mutex>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/exporters/elasticsearch/es_log_record_exporter.h"
#include "opentelemetry/exporters/elasticsearch/es_log_recordable.h"
#include "opentelemetry/ext/http/client/detail/default_factory.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/version.h"

// Half this file only exists under ENABLE_ASYNC_EXPORT, and include-what-you-use asks for these
// in the configurations that build it and asks for them to go in the ones that do not.
#ifdef ENABLE_ASYNC_EXPORT
#  include <cstddef>
#  include <functional>
#  include <set>

#  include "opentelemetry/common/timestamp.h"
#endif

namespace sdklogs     = opentelemetry::sdk::logs;
namespace http_client = opentelemetry::ext::http::client;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{
namespace
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

  std::string BuildResponseLogMessage(http_client::Response &response,
                                      const std::string &body) noexcept
  {
    std::stringstream ss;
    ss << "Status:" << response.GetStatusCode() << ", Header:";
    response.ForEachHeader([&ss](opentelemetry::nostd::string_view header_name,
                                 opentelemetry::nostd::string_view header_value) {
      ss << "\t" << header_name << ": " << header_value << ",";
      return true;
    });
    ss << "Body:" << body;

    return ss.str();
  }

  /**
   * Automatically called when the response is received, store the body into a string and notify any
   * threads blocked on this result
   */
  void OnResponse(http_client::Response &response) noexcept override
  {
    std::string log_message;

    // Lock the private members so they can't be read while being modified
    {
      std::unique_lock<std::mutex> lk(mutex_);

      // Store the body of the request
      body_ = std::string(response.GetBody().begin(), response.GetBody().end());

      if (!(response.GetStatusCode() >= 200 && response.GetStatusCode() <= 299))
      {
        log_message = BuildResponseLogMessage(response, body_);

        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Export failed, " << log_message);
      }

      if (console_debug_)
      {
        if (log_message.empty())
        {
          log_message = BuildResponseLogMessage(response, body_);
        }

        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Got response from Elasticsearch, "
                                << log_message);
      }

      // Record the outcome and notify any threads waiting on this result
      recordCompletionLocked(CompletionState::Success);
    }
    cv_.notify_all();
  }

  /**
   * A method the user calls to block their thread until the request has either produced a
   * response or failed. The longest duration is the timeout of the request, set by
   * SetTimeoutMs(), which arrives here as a TimedOut session event.
   */
  bool waitForResponse()
  {
    std::unique_lock<std::mutex> lk(mutex_);
    // Waiting on a predicate rather than bare: the completion may already have been recorded
    // before this thread got here, in which case there is no notification left to receive.
    cv_.wait(lk, [this] { return completion_ != CompletionState::Pending; });
    return completion_ == CompletionState::Success;
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
  void OnEvent(http_client::SessionState state, nostd::string_view /* reason */) noexcept override
  {
    // If any failure event occurs, release the condition variable to unblock main thread
    switch (state)
    {
      case http_client::SessionState::CreateFailed:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Failed to create session");
        recordCompletion(CompletionState::Failure);
        break;
      case http_client::SessionState::Created:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Session created");
        break;
      case http_client::SessionState::Destroyed:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Session destroyed");
        // Nothing else will arrive after this. If no outcome was recorded, the session ended
        // without a response, so release the waiter rather than leaving it blocked forever.
        recordCompletion(CompletionState::Failure);
        break;
      case http_client::SessionState::Connecting:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Connecting to peer");
        break;
      case http_client::SessionState::ConnectFailed:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Failed to connect to peer");
        recordCompletion(CompletionState::Failure);
        break;
      case http_client::SessionState::Connected:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Connected to peer");
        break;
      case http_client::SessionState::Sending:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Sending request");
        break;
      case http_client::SessionState::SendFailed:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Failed to send request");
        recordCompletion(CompletionState::Failure);
        break;
      case http_client::SessionState::Response:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Received response");
        break;
      case http_client::SessionState::SSLHandshakeFailed:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Failed SSL Handshake");
        recordCompletion(CompletionState::Failure);
        break;
      case http_client::SessionState::TimedOut:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Request timed out");
        recordCompletion(CompletionState::Failure);
        break;
      case http_client::SessionState::NetworkError:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Network error");
        recordCompletion(CompletionState::Failure);
        break;
      case http_client::SessionState::ReadError:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Read error");
        break;
      case http_client::SessionState::WriteError:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Write error");
        break;
      case http_client::SessionState::Cancelled:
        OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] (manually) cancelled");
        recordCompletion(CompletionState::Failure);
        break;
    }
  }

private:
  enum class CompletionState : std::uint8_t
  {
    Pending,
    Success,
    Failure
  };

  /**
   * Record the outcome of the request, first writer wins, then release any waiter. Keeping the
   * first outcome means a session destroyed after a successful response does not overwrite it.
   */
  void recordCompletion(CompletionState state)
  {
    {
      std::unique_lock<std::mutex> lk(mutex_);
      recordCompletionLocked(state);
    }
    cv_.notify_all();
  }

  /// As recordCompletion(), for callers that already hold mutex_ and notify themselves.
  void recordCompletionLocked(CompletionState state)
  {
    if (completion_ == CompletionState::Pending)
    {
      completion_ = state;
    }
  }

  // Define a condition variable and mutex
  std::condition_variable cv_;
  std::mutex mutex_;

  // Whether the request has completed, and how
  CompletionState completion_ = CompletionState::Pending;

  // A string to store the response body
  std::string body_ = "";

  // Whether to print the results from the callback
  bool console_debug_ = false;
};

#ifdef ENABLE_ASYNC_EXPORT
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
      : session_{std::move(session)},
        result_callback_{std::move(result_callback)},
        console_debug_{console_debug}
  {}

  AsyncResponseHandler(const AsyncResponseHandler &)            = delete;
  AsyncResponseHandler &operator=(const AsyncResponseHandler &) = delete;
  AsyncResponseHandler(AsyncResponseHandler &&)                 = delete;
  AsyncResponseHandler &operator=(AsyncResponseHandler &&)      = delete;

  /**
   * Cleans up the session in the destructor.
   */
  ~AsyncResponseHandler() override
  {
    // A handler that goes away without an outcome would leave ForceFlush() waiting on a session
    // that can no longer finish. Report before tearing the session down, since FinishSession()
    // can block.
    CompleteOnce(sdk::common::ExportResult::kFailure);
    session_->FinishSession();
  }

  /**
   * Report the outcome of this export, at most once. The HTTP client can deliver both a response
   * and a terminal session event for one request, and the exporter counts one finished session
   * per export, so only the first outcome is reported.
   * @return whether this call is the one that reported.
   */
  bool CompleteOnce(sdk::common::ExportResult result) noexcept
  {
    bool expected = false;
    if (!completed_.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
    {
      return false;
    }
    result_callback_(result);
    return true;
  }

  /**
   * Automatically called when the response is received
   */
  void OnResponse(http_client::Response &response) noexcept override
  {

    const std::string body(response.GetBody().begin(), response.GetBody().end());
    const bool written = body.find("\"failed\" : 0") != std::string::npos;

    // Reported before anything is logged. CompleteOnce() retires the session and wakes
    // ForceFlush() before it returns, and the log handler is replaceable, so one that calls
    // ForceFlush() would otherwise wait for the session this call has not let go of. A response
    // that loses the exchange says nothing either, since the outcome it would describe is not the
    // one the caller was given.
    if (!CompleteOnce(written ? sdk::common::ExportResult::kSuccess
                              : sdk::common::ExportResult::kFailure))
    {
      return;
    }

    if (console_debug_)
    {
      OTEL_INTERNAL_LOG_DEBUG(
          "[ES Log Exporter] Got response from Elasticsearch,  response body: " << body);
    }
    if (!written)
    {
      OTEL_INTERNAL_LOG_ERROR(
          "[ES Log Exporter] Logs were not written to Elasticsearch correctly, response body: "
          << body);
    }
  }

  // Callback method when an http event occurs
  void OnEvent(http_client::SessionState state, nostd::string_view /* reason */) noexcept override
  {
    // Every state is listed so that -Wswitch reports a new one rather than it being swallowed by
    // a default label and silently leaving the session uncounted.
    const char *failure = nullptr;
    switch (state)
    {
      case http_client::SessionState::CreateFailed:
        failure = "[ES Log Exporter] Create request to elasticsearch failed";
        break;
      case http_client::SessionState::Created:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Session created");
        break;
      case http_client::SessionState::Destroyed:
        failure = "[ES Log Exporter] Session to elasticsearch destroyed before a response";
        break;
      case http_client::SessionState::Connecting:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Connecting to elasticsearch");
        break;
      case http_client::SessionState::ConnectFailed:
        failure = "[ES Log Exporter] Connection to elasticsearch failed";
        break;
      case http_client::SessionState::Connected:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Connected to elasticsearch");
        break;
      case http_client::SessionState::Sending:
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Sending request to elasticsearch");
        break;
      case http_client::SessionState::SendFailed:
        failure = "[ES Log Exporter] Request failed to be sent to elasticsearch";
        break;
      case http_client::SessionState::Response:
        // The body arrives through OnResponse(), which is what reports the outcome.
        OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Response received from elasticsearch");
        break;
      case http_client::SessionState::SSLHandshakeFailed:
        failure = "[ES Log Exporter] SSL handshake to elasticsearch failed";
        break;
      case http_client::SessionState::TimedOut:
        failure = "[ES Log Exporter] Request to elasticsearch timed out";
        break;
      case http_client::SessionState::NetworkError:
        failure = "[ES Log Exporter] Network error to elasticsearch";
        break;
      case http_client::SessionState::ReadError:
        failure = "[ES Log Exporter] Read error";
        break;
      case http_client::SessionState::WriteError:
        failure = "[ES Log Exporter] Write error";
        break;
      case http_client::SessionState::Cancelled:
        failure = "[ES Log Exporter] Request to elasticsearch cancelled";
        break;
    }

    // Reported only when this event is the outcome. Any of these can arrive after a response has
    // already been reported, and an error line there would describe a failure the exporter never
    // told the caller about.
    if (failure != nullptr && CompleteOnce(sdk::common::ExportResult::kFailure))
    {
      OTEL_INTERNAL_LOG_ERROR(failure);
    }
  }

private:
  // Stores the session object for the request
  std::shared_ptr<ext::http::client::Session> session_;
  // Callback to call to on receiving events
  std::function<bool(opentelemetry::sdk::common::ExportResult)> result_callback_;

  // Whether the outcome has already been reported
  std::atomic<bool> completed_{false};

  // Whether to print the results from the callback
  bool console_debug_ = false;
};
#endif
}  // namespace

ElasticsearchLogRecordExporter::ElasticsearchLogRecordExporter()
    : ElasticsearchLogRecordExporter(ElasticsearchExporterOptions())
{}

ElasticsearchLogRecordExporter::ElasticsearchLogRecordExporter(
    const ElasticsearchExporterOptions &options)
    : ElasticsearchLogRecordExporter(options,
                                     ext::http::client::detail::GetDefaultHttpClientFactory())
{}

ElasticsearchLogRecordExporter::ElasticsearchLogRecordExporter(
    const ElasticsearchExporterOptions &options,
    const std::shared_ptr<ext::http::client::HttpClientFactory> &factory)
    : ElasticsearchLogRecordExporter(options, factory->Create())
{}

ElasticsearchLogRecordExporter::ElasticsearchLogRecordExporter(
    const ElasticsearchExporterOptions &options,
    std::shared_ptr<ext::http::client::HttpClient> http_client)
    : options_{options},
      http_client_{std::move(http_client)}
#ifdef ENABLE_ASYNC_EXPORT
      ,
      synchronization_data_(new SynchronizationData())
#endif
{}

std::unique_ptr<sdklogs::Recordable> ElasticsearchLogRecordExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdklogs::Recordable>(new ElasticSearchRecordable());
}

sdk::common::ExportResult ElasticsearchLogRecordExporter::Export(
    const nostd::span<std::unique_ptr<sdklogs::Recordable>> &records) noexcept
{
#ifdef ENABLE_ASYNC_EXPORT
  // Registered before anything that can return, so a flush asked from the moment these records
  // arrive waits for them, and every exit below reports through the guard.
  const std::size_t span_count = records.size();
  auto synchronization_data    = synchronization_data_;

  std::uint64_t session_id = 0;
  {
    std::lock_guard<std::mutex> lock(synchronization_data_->force_flush_cv_m);
    session_id = synchronization_data_->next_session_id++;
    synchronization_data_->running_sessions.insert(session_id);
  }

  // Retiring the export is separate from describing what happened to it, because the refusal
  // below has to retire without having an outcome to report through the completion.
  auto retire = [session_id, synchronization_data]() noexcept {
    bool retired = false;
    {
      // Published under the mutex ForceFlush() waits on. A waiter that has evaluated its
      // predicate but not yet parked would otherwise not see this until the next wakeup.
      std::lock_guard<std::mutex> lock(synchronization_data->force_flush_cv_m);
      retired = synchronization_data->running_sessions.erase(session_id) == 1;
    }
    synchronization_data->force_flush_cv.notify_all();
    return retired;
  };

  using Completion    = std::function<bool(opentelemetry::sdk::common::ExportResult)>;
  Completion complete = [span_count,
                         retire](opentelemetry::sdk::common::ExportResult result) noexcept {
    retire();

    // Logged after the session is retired. The log handler is replaceable, and one that calls
    // ForceFlush() would otherwise wait for the very session this call has not let go of yet.
    //
    // That is the whole of what the ordering buys, and it is worth being exact about the rest.
    // It does not make a log handler safe to re-enter the exporter from in general: one that
    // calls ForceFlush() without a deadline from a progress event blocks the Export() that has
    // not handed its request to the client yet, and one that flushes from any callback the HTTP
    // client dispatches can wait on work only that client thread can advance. Neither is new
    // here and neither is fixed here.
    if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
    {
      OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] ERROR: Export "
                              << span_count
                              << " log record(s) error: " << static_cast<int>(result));
    }
    else
    {
      OTEL_INTERNAL_LOG_DEBUG("[ES Log Exporter] Export " << span_count
                                                          << " log record(s) success");
    }
    return true;
  };

  // A return added between here and SendRequest() would otherwise strand a waiter on a session
  // that can never finish. Reporting through the same completion leaves a session one way out,
  // and says so in the log rather than dropping the batch silently.
  struct GiveUpGuard
  {
    Completion *report = nullptr;

    GiveUpGuard()                               = default;
    GiveUpGuard(const GiveUpGuard &)            = delete;
    GiveUpGuard &operator=(const GiveUpGuard &) = delete;
    GiveUpGuard(GiveUpGuard &&)                 = delete;
    GiveUpGuard &operator=(GiveUpGuard &&)      = delete;

    ~GiveUpGuard()
    {
      if (report != nullptr)
      {
        (*report)(opentelemetry::sdk::common::ExportResult::kFailure);
      }
    }
  } guard;
  guard.report = &complete;
#endif

  // Return failure if this exporter has been shutdown
  if (isShutdown())
  {
#ifdef ENABLE_ASYNC_EXPORT
    // Retired before anything replaceable runs, and reported here rather than through the guard,
    // so a log handler that flushes does not wait for this Export() and one refusal reads as one.
    guard.report = nullptr;
    retire();
#endif
    OTEL_INTERNAL_LOG_ERROR("[ES Log Exporter] Exporting "
                            << records.size() << " log(s) failed, exporter is shutdown");
    return sdk::common::ExportResult::kFailure;
  }

  // Create a connection to the ElasticSearch instance
  auto session = http_client_->CreateSession(options_.host_ + ":" + std::to_string(options_.port_));
  auto request = session->CreateRequest();

  // Populate the request with headers and methods
  request->SetUri(options_.index_ + "/_bulk?pretty");
  request->SetMethod(http_client::Method::Post);
  request->AddHeader("Content-Type", "application/json");

  // Add options headers
  for (auto it = options_.http_headers_.cbegin(); it != options_.http_headers_.cend(); ++it)
  {
    request->AddHeader(it->first, it->second);
  }

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

#ifdef ENABLE_ASYNC_EXPORT
  // Send the request
  auto handler = std::make_shared<AsyncResponseHandler>(session, Completion(complete),
                                                        options_.console_debug_);
  session->SendRequest(handler);
  guard.report = nullptr;  // the handler reports this session from here on
  return sdk::common::ExportResult::kSuccess;
#else
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
#endif
}

bool ElasticsearchLogRecordExporter::ForceFlush(std::chrono::microseconds timeout
                                                OPENTELEMETRY_MAYBE_UNUSED) noexcept
{
#ifdef ENABLE_ASYNC_EXPORT
  // ASAN will report chrono: runtime error: signed integer overflow: A + B cannot be represented
  //   in type 'long int' here. So we reset timeout to meet signed long int limit here. Zero is
  //   what that returns for a timeout there is no point waiting against, which is also how a
  //   caller asks for no deadline.
  timeout = opentelemetry::common::DurationUtil::AdjustWaitForTimeout(
      timeout, std::chrono::microseconds::zero());

  std::unique_lock<std::mutex> lock(synchronization_data_->force_flush_cv_m);

  // The snapshot is the next id, not a count: a session started after it takes a larger id and
  // cannot stand in for one of these. Ids are issued in order, so the smallest one still running
  // decides. Callers are not serialised, so two deadlines never queue behind one another.
  const std::uint64_t watermark = synchronization_data_->next_session_id;
  ++synchronization_data_->watermarks_taken;
  const auto flushed = [this, watermark]() {
    const auto &running = synchronization_data_->running_sessions;
    return running.empty() || *running.begin() >= watermark;
  };

  if (timeout <= std::chrono::microseconds::zero())
  {
    // wait() only returns once the predicate holds, so the flush has completed.
    synchronization_data_->force_flush_cv.wait(lock, flushed);
    return true;
  }

  // One deadline for the call, so a wakeup that is not a completion resumes against what is left
  // rather than starting the wait again. wait_until() returns the predicate, so a flush that ran
  // out of time cannot report success.
  const std::chrono::steady_clock::time_point deadline =
      std::chrono::steady_clock::now() +
      std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout);

  return synchronization_data_->force_flush_cv.wait_until(lock, deadline, flushed);
#else
  return true;
#endif
}

bool ElasticsearchLogRecordExporter::Shutdown(std::chrono::microseconds /* timeout */) noexcept
{
  is_shutdown_ = true;

  // Shutdown the session manager
  http_client_->CancelAllSessions();
  http_client_->FinishAllSessions();

  return true;
}

bool ElasticsearchLogRecordExporter::isShutdown() const noexcept
{
  return is_shutdown_;
}
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
