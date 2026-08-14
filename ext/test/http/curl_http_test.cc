// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <curl/curl.h>
#include <curl/curlver.h>
#include "gtest/gtest.h"

#ifdef ENABLE_OTLP_RETRY_PREVIEW
#  include "gmock/gmock.h"
#endif  // ENABLE_OTLP_RETRY_PREVIEW

#ifdef ENABLE_OTLP_COMPRESSION_PREVIEW
#  include <numeric>
#endif  // ENABLE_OTLP_COMPRESSION_PREVIEW

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

#include "opentelemetry/ext//http/client/curl/http_client_curl.h"
#include "opentelemetry/ext/http/client/curl/http_client_factory_curl.h"
#include "opentelemetry/ext/http/client/curl/http_operation_curl.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/server/http_server.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

constexpr int HTTP_PORT{19000};

namespace curl        = opentelemetry::ext::http::client::curl;
namespace http_client = opentelemetry::ext::http::client;
namespace nostd       = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace http
{
namespace client
{
namespace curl
{
// resetMultiHandle only runs when curl_multi_perform fails, which a test cannot provoke, so
// the case below reaches it directly. See #4389.
class HttpClientTestPeer
{
public:
  static void ResetMultiHandle(HttpClient &client) { client.resetMultiHandle(); }

  static bool RemoveSessions(HttpClient &client) { return client.doRemoveSessions(); }

  static bool AbortSessions(HttpClient &client) { return client.doAbortSessions(); }

  // What the multi handle holds is recorded when curl_multi_add_handle accepts a handle. The
  // cases below have no transfer to accept, so they say what it holds directly.
  static void NoteAttached(HttpClient &client, CURL *easy_handle)
  {
    client.attached_handles_.insert(easy_handle);
  }

  static std::size_t AttachedCount(const HttpClient &client)
  {
    return client.attached_handles_.size();
  }

  static std::size_t PendingRemovalCount(const HttpClient &client)
  {
    return client.pending_to_remove_session_handles_.size();
  }

  static std::size_t PendingRemovalOwnerCount(const HttpClient &client)
  {
    std::size_t owned = 0;
    for (const auto &pending : client.pending_to_remove_session_handles_)
    {
      if (pending.owner)
      {
        ++owned;
      }
    }
    return owned;
  }

  static std::size_t QuarantinedCount(const HttpClient &client)
  {
    return client.quarantined_handles_.size();
  }

  // A multi handle that failed to initialize refuses every removal, which is how the cases below
  // reach the path where libcurl will not take a handle back.
  static CURLM *ExchangeMultiHandle(HttpClient &client, CURLM *replacement)
  {
    CURLM *previous      = client.multi_handle_;
    client.multi_handle_ = replacement;
    return previous;
  }
};
}  // namespace curl
}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE

namespace
{

class CustomEventHandler : public http_client::EventHandler
{
public:
  void OnResponse(http_client::Response & /* response */) noexcept override
  {
    got_response_.store(true, std::memory_order_release);
  }
  void OnEvent(http_client::SessionState state, nostd::string_view /* reason */) noexcept override
  {
    switch (state)
    {
      case http_client::SessionState::ConnectFailed:
      case http_client::SessionState::SendFailed: {
        is_called_.store(true, std::memory_order_release);
        break;
      }
      default:
        break;
    }
  }

  CustomEventHandler() : is_called_(false), got_response_(false) {}

  std::atomic<bool> is_called_;
  std::atomic<bool> got_response_;
};

// Counts the terminal notifications one request produces. Set cancel_at_response_ to cancel from
// inside the Response event, which is the one moment both arms of the completion callback are
// eligible: DispatchEvent notifies the handler before it stores the new state, and the callback
// runs after both, so it sees an aborted operation that also has a response.
class TerminalCountingHandler : public CustomEventHandler
{
public:
  void OnResponse(http_client::Response & /* response */) noexcept override
  {
    terminal_count_.fetch_add(1, std::memory_order_release);
    got_response_.store(true, std::memory_order_release);
  }

  void OnEvent(http_client::SessionState state, nostd::string_view reason) noexcept override
  {
    // Counted on its own, and before the chain below rather than inside it: a case can cancel
    // from one of these, and two others pin terminal_count_ to an exact value.
    if (state == http_client::SessionState::ConnectFailed ||
        state == http_client::SessionState::SendFailed)
    {
      failed_count_.fetch_add(1, std::memory_order_release);
    }

    if (state == http_client::SessionState::Cancelled)
    {
      terminal_count_.fetch_add(1, std::memory_order_release);
      // Cleanup dispatches its own Cancelled carrying a curl message, and GetCurlErrorMessage
      // never yields an empty one, so an empty reason is the completion callback and only it.
      if (reason.empty())
      {
        cancelled_from_callback_.fetch_add(1, std::memory_order_release);
      }
    }
    else if (state == cancel_at_ && cancel_target_ != nullptr)
    {
      auto *session   = cancel_target_;
      cancel_target_  = nullptr;
      cancelled_from_ = std::this_thread::get_id();
      session->CancelSession();
    }
  }

  // cancel_at_ picks the event to cancel from and cancelled_from_ records the thread it ran
  // on, so a case can pin which side of the client it covers.
  http_client::Session *cancel_target_ = nullptr;
  http_client::SessionState cancel_at_ = http_client::SessionState::Response;
  std::thread::id cancelled_from_{};
  std::atomic<int> terminal_count_{0};
  std::atomic<int> failed_count_{0};
  std::atomic<int> cancelled_from_callback_{0};
};

class GetEventHandler : public CustomEventHandler
{
public:
  void OnResponse(http_client::Response &response) noexcept override
  {
    ASSERT_EQ(200, response.GetStatusCode());
    ASSERT_EQ(response.GetBody().size(), 0);
    is_called_.store(true, std::memory_order_release);
    got_response_.store(true, std::memory_order_release);
  }
};

class PostEventHandler : public CustomEventHandler
{
public:
  void OnResponse(http_client::Response &response) noexcept override
  {
    ASSERT_EQ(200, response.GetStatusCode());
    std::string body(response.GetBody().begin(), response.GetBody().end());
    ASSERT_EQ(body, "{'k1':'v1', 'k2':'v2', 'k3':'v3'}");
    is_called_.store(true, std::memory_order_release);
    got_response_.store(true, std::memory_order_release);
  }
};

class FinishInCallbackHandler : public CustomEventHandler
{
public:
  FinishInCallbackHandler(std::shared_ptr<http_client::Session> session)
      : session_(std::move(session))
  {}

  void OnResponse(http_client::Response &response) noexcept override
  {
    ASSERT_EQ(200, response.GetStatusCode());
    ASSERT_EQ(response.GetBody().size(), 0);
    is_called_.store(true, std::memory_order_release);
    got_response_.store(true, std::memory_order_release);

    if (session_)
    {
      session_->FinishSession();
      session_.reset();
    }
  }

private:
  std::shared_ptr<http_client::Session> session_;
};

class RetryEventHandler : public CustomEventHandler
{
public:
  void OnResponse(http_client::Response &response) noexcept override
  {
    ASSERT_EQ(429, response.GetStatusCode());
    ASSERT_EQ(response.GetBody().size(), 0);
    is_called_.store(true, std::memory_order_release);
    got_response_.store(true, std::memory_order_release);
  }
};

class BasicCurlHttpTests : public ::testing::Test, public HTTP_SERVER_NS::HttpRequestCallback
{
protected:
  HTTP_SERVER_NS::HttpServer server_;
  std::string server_address_;
  std::atomic<bool> is_setup_{false};
  std::atomic<bool> is_running_{false};
  std::vector<HTTP_SERVER_NS::HttpRequest> received_requests_;
  std::atomic<unsigned> close_requests_{0};
  std::mutex cv_mtx_requests;
  std::mutex mtx_requests;
  std::condition_variable cv_got_events;
  std::mutex cv_m;

public:
  BasicCurlHttpTests() : is_setup_(false), is_running_(false) {}

protected:
  void SetUp() override
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
    server_.addHandler("/retry/", *this);
    server_.addHandler("/close/", *this);
    server_.start();
    is_running_ = true;
  }

  void TearDown() override
  {
    if (!is_setup_.exchange(false))
      return;
    server_.stop();
    is_running_ = false;
  }

public:
  int onHttpRequest(HTTP_SERVER_NS::HttpRequest const &request,
                    HTTP_SERVER_NS::HttpResponse &response) override
  {
    int response_status = 404;
    if (request.uri == "/get/")
    {
      std::unique_lock<std::mutex> lk1(mtx_requests);
      received_requests_.push_back(request);
      response.headers["Content-Type"] = "text/plain";
      response_status                  = 200;
    }
    else if (request.uri == "/post/")
    {
      std::unique_lock<std::mutex> lk1(mtx_requests);
      received_requests_.push_back(request);
      response.headers["Content-Type"] = "application/json";
      response.body                    = "{'k1':'v1', 'k2':'v2', 'k3':'v3'}";
      response_status                  = 200;
    }
    else if (request.uri == "/retry/")
    {
      std::unique_lock<std::mutex> lk1(mtx_requests);
      received_requests_.push_back(request);
      response.headers["Content-Type"] = "text/plain";
      response_status                  = 429;
    }
    else if (request.uri == "/close/")
    {
      // -1 is the documented way for a handler to ask the server to terminate the
      // connection immediately without sending a response.
      close_requests_.fetch_add(1, std::memory_order_relaxed);
      response_status = -1;
    }

    cv_got_events.notify_one();

    return response_status;
  }

  bool waitForRequests(unsigned timeOutSec, unsigned expected_count = 1)
  {
    std::unique_lock<std::mutex> lk(cv_mtx_requests);
    if (cv_got_events.wait_for(lk, std::chrono::milliseconds(1000 * timeOutSec), [&] {
          std::unique_lock<std::mutex> lk1(mtx_requests);
          return received_requests_.size() >= expected_count;
        }))
    {
      return true;
    }
    return false;
  }
};

class DISABLED_BasicCurlHttpTests : public BasicCurlHttpTests
{};

TEST_F(BasicCurlHttpTests, DoNothing) {}

TEST_F(BasicCurlHttpTests, HttpRequest)
{
  curl::Request req;
  const char *b           = "test-data";
  http_client::Body body  = {b, b + std::strlen(b)};
  http_client::Body body1 = body;
  req.SetBody(body);
  ASSERT_EQ(req.body_, body1);
  req.AddHeader("name1", "value1");
  req.AddHeader("name2", "value2");
  ASSERT_TRUE(req.headers_.find("name1")->second == "value1");
  ASSERT_TRUE(req.headers_.find("name2")->second == "value2");

  req.ReplaceHeader("name1", "value3");
  ASSERT_EQ(req.headers_.find("name1")->second, "value3");

  req.SetTimeoutMs(std::chrono::duration<int>(2000));
  ASSERT_EQ(req.timeout_ms_, std::chrono::duration<int>(2000));
}

TEST_F(BasicCurlHttpTests, HttpResponse)
{
  curl::Response res;
  http_client::Headers m1 = {
      {"name1", "value1_1"}, {"name1", "value1_2"}, {"name2", "value3"}, {"name3", "value3"}};
  res.headers_ = m1;

  const char *b          = "test-data";
  http_client::Body body = {b, b + std::strlen(b)};
  int count              = 0;
  res.ForEachHeader("name1", [&count](nostd::string_view name, nostd::string_view value) {
    if (name != "name1")
      return false;
    if (value != "value1_1" && value != "value1_2")
      return false;
    count++;
    return true;
  });
  ASSERT_EQ(count, 2);
  count = 0;
  res.ForEachHeader([&count](nostd::string_view name, nostd::string_view value) {
    if (name != "name1" && name != "name2" && name != "name3")
      return false;
    if (value != "value1_1" && value != "value1_2" && value != "value2" && value != "value3")
      return false;
    count++;
    return true;
  });
  ASSERT_EQ(count, 4);
}

TEST_F(BasicCurlHttpTests, SendGetRequest)
{
  received_requests_.clear();
  auto session_manager = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session = session_manager->CreateSession("http://127.0.0.1:19000");
  auto request = session->CreateRequest();
  request->SetUri("get/");
  auto handler = std::make_shared<GetEventHandler>();
  session->SendRequest(handler);
  ASSERT_TRUE(waitForRequests(30, 1));
  session->FinishSession();
  ASSERT_TRUE(handler->is_called_.load(std::memory_order_acquire));
  ASSERT_TRUE(handler->got_response_.load(std::memory_order_acquire));
}

TEST_F(BasicCurlHttpTests, SendPostRequest)
{
  received_requests_.clear();
  auto session_manager = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session = session_manager->CreateSession("http://127.0.0.1:19000");
  auto request = session->CreateRequest();
  request->SetUri("post/");
  request->SetMethod(http_client::Method::Post);

  const char *b          = "test-data";
  http_client::Body body = {b, b + std::strlen(b)};
  request->SetBody(body);
  request->AddHeader("Content-Type", "text/plain");
  auto handler = std::make_shared<PostEventHandler>();
  session->SendRequest(handler);
  ASSERT_TRUE(waitForRequests(30, 1));
  session->FinishSession();
  ASSERT_TRUE(handler->is_called_.load(std::memory_order_acquire));
  ASSERT_TRUE(handler->got_response_.load(std::memory_order_acquire));

  session_manager->CancelAllSessions();
  session_manager->FinishAllSessions();
}

TEST_F(BasicCurlHttpTests, RequestTimeout)
{
  received_requests_.clear();
  auto session_manager = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session = session_manager->CreateSession("192.0.2.0:19000");  // RFC 5737 TEST-NET-1
  auto request = session->CreateRequest();
  request->SetUri("get/");
  auto handler = std::make_shared<GetEventHandler>();
  session->SendRequest(handler);
  session->FinishSession();
  ASSERT_TRUE(handler->is_called_.load(std::memory_order_acquire));
  ASSERT_FALSE(handler->got_response_.load(std::memory_order_acquire));
}

TEST_F(BasicCurlHttpTests, CurlHttpOperations)
{
  http_client::HttpSslOptions no_ssl;

  GetEventHandler *handler = new GetEventHandler();

  const char *b          = "test-data";
  http_client::Body body = {b, b + std::strlen(b)};

  http_client::Headers headers = {
      {"name1", "value1_1"}, {"name1", "value1_2"}, {"name2", "value3"}, {"name3", "value3"}};

  http_client::Compression compression = http_client::Compression::kNone;

  curl::HttpOperation http_operations1(http_client::Method::Head, "/get", no_ssl, handler, headers,
                                       body, compression, true);
  http_operations1.Send();

  curl::HttpOperation http_operations2(http_client::Method::Get, "/get", no_ssl, handler, headers,
                                       body, compression, true);
  http_operations2.Send();

  curl::HttpOperation http_operations3(http_client::Method::Get, "/get", no_ssl, handler, headers,
                                       body, compression, false);
  http_operations3.Send();
  delete handler;
}

#ifdef ENABLE_OTLP_RETRY_PREVIEW
TEST_F(BasicCurlHttpTests, RetryPolicyEnabled)
{
  RetryEventHandler handler;
  http_client::HttpSslOptions no_ssl;
  http_client::Body body;
  http_client::Headers headers;
  http_client::Compression compression  = http_client::Compression::kNone;
  http_client::RetryPolicy retry_policy = {5, std::chrono::duration<float>{1.0f},
                                           std::chrono::duration<float>{5.0f}, 1.5f};

  curl::HttpOperation operation(http_client::Method::Post, "http://127.0.0.1:19000/retry/", no_ssl,
                                &handler, headers, body, compression, false,
                                curl::kDefaultHttpConnTimeout, false, false, retry_policy);

  ASSERT_EQ(CURLE_OK, operation.Send());
  ASSERT_TRUE(operation.IsRetryable());
}

TEST_F(BasicCurlHttpTests, RetryPolicyDisabled)
{
  RetryEventHandler handler;
  http_client::HttpSslOptions no_ssl;
  http_client::Body body;
  http_client::Headers headers;
  http_client::Compression compression     = http_client::Compression::kNone;
  http_client::RetryPolicy no_retry_policy = {0, std::chrono::duration<float>::zero(),
                                              std::chrono::duration<float>::zero(), 0.0f};

  curl::HttpOperation operation(http_client::Method::Post, "http://127.0.0.1:19000/retry/", no_ssl,
                                &handler, headers, body, compression, false,
                                curl::kDefaultHttpConnTimeout, false, false, no_retry_policy);

  ASSERT_EQ(CURLE_OK, operation.Send());
  ASSERT_FALSE(operation.IsRetryable());
}

TEST_F(BasicCurlHttpTests, ExponentialBackoffRetry)
{
  using ::testing::AllOf;
  using ::testing::Gt;
  using ::testing::Lt;

  RetryEventHandler handler;
  http_client::HttpSslOptions no_ssl;
  http_client::Body body;
  http_client::Headers headers;
  http_client::Compression compression  = http_client::Compression::kNone;
  http_client::RetryPolicy retry_policy = {4, std::chrono::duration<float>{1.0f},
                                           std::chrono::duration<float>{5.0f}, 2.0f};

  curl::HttpOperation operation(http_client::Method::Post, "http://127.0.0.1:19000/retry/", no_ssl,
                                &handler, headers, body, compression, false,
                                curl::kDefaultHttpConnTimeout, false, false, retry_policy);

  auto first_attempt_time = std::chrono::system_clock::now();
  ASSERT_EQ(CURLE_OK, operation.Send());
  ASSERT_TRUE(operation.IsRetryable());
  ASSERT_THAT(
      operation.NextRetryTime().time_since_epoch().count(),
      AllOf(Gt((first_attempt_time + std::chrono::milliseconds{750}).time_since_epoch().count()),
            Lt((first_attempt_time + std::chrono::milliseconds{1250}).time_since_epoch().count())));

  auto second_attempt_time = std::chrono::system_clock::now();
  ASSERT_EQ(CURLE_OK, operation.Send());
  ASSERT_TRUE(operation.IsRetryable());
  ASSERT_THAT(
      operation.NextRetryTime().time_since_epoch().count(),
      AllOf(
          Gt((second_attempt_time + std::chrono::milliseconds{1550}).time_since_epoch().count()),
          Lt((second_attempt_time + std::chrono::milliseconds{2450}).time_since_epoch().count())));

  auto third_attempt_time = std::chrono::system_clock::now();
  ASSERT_EQ(CURLE_OK, operation.Send());
  ASSERT_TRUE(operation.IsRetryable());
  ASSERT_THAT(
      operation.NextRetryTime().time_since_epoch().count(),
      AllOf(Gt((third_attempt_time + std::chrono::milliseconds{3150}).time_since_epoch().count()),
            Lt((third_attempt_time + std::chrono::milliseconds{4850}).time_since_epoch().count())));

  ASSERT_EQ(CURLE_OK, operation.Send());
  ASSERT_FALSE(operation.IsRetryable());
}
#endif  // ENABLE_OTLP_RETRY_PREVIEW

// A cancel that arrives once the server has answered used to deliver Cancelled and the response,
// so a handler treating either as terminal saw one request finish twice.
TEST_F(BasicCurlHttpTests, ACancelAfterTheResponseReportsOneOutcome)
{
  received_requests_.clear();
  auto session_manager = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session = session_manager->CreateSession("http://127.0.0.1:19000");
  auto request = session->CreateRequest();
  request->SetUri("get/");

  auto handler            = std::make_shared<TerminalCountingHandler>();
  handler->cancel_target_ = session.get();
  handler->cancel_at_     = http_client::SessionState::Response;

  session->SendRequest(handler);
  ASSERT_TRUE(waitForRequests(30, 1));
  session->FinishSession();

  EXPECT_TRUE(handler->got_response_.load(std::memory_order_acquire));
  EXPECT_EQ(1, handler->terminal_count_.load(std::memory_order_acquire));

  session_manager->FinishAllSessions();
}

// The other arm of the same callback, which nothing exercised. Nothing listens on 19937, so the
// connection fails and PerformCurlMessage dispatches ConnectFailed from the IO thread. Cancelling
// there leaves the state short of Response with the abort flag raised, which is what the arm
// needs, and keeps Abort() on the thread that owns the easy handle.
TEST_F(BasicCurlHttpTests, ACancelBeforeTheResponseReportsCancelled)
{
  received_requests_.clear();
  auto session_manager = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session = session_manager->CreateSession("http://127.0.0.1:19937");
  auto request = session->CreateRequest();
  request->SetUri("get/");

  auto handler            = std::make_shared<TerminalCountingHandler>();
  handler->cancel_target_ = session.get();
  handler->cancel_at_     = http_client::SessionState::ConnectFailed;

  session->SendRequest(handler);
  session->FinishSession();

  EXPECT_FALSE(handler->got_response_.load(std::memory_order_acquire));
  // Counted by its empty reason so the assertion holds the arm this covers rather than whatever
  // else reports a cancel.
  EXPECT_EQ(1, handler->cancelled_from_callback_.load(std::memory_order_acquire));
  EXPECT_NE(handler->cancelled_from_, std::this_thread::get_id())
      << "this case cancels from an event the IO thread dispatches";

  session_manager->FinishAllSessions();
}

// NextRetryTime draws the backoff jitter from an engine that used to be a shared static, so
// two clients retrying at the same time wrote the same std::mt19937. The case passes either
// way, since a data race is not a functional failure. It is here for the sanitizer builds.
TEST_F(BasicCurlHttpTests, RetryJitterIsNotSharedAcrossThreads)
{
  opentelemetry::ext::http::client::HttpSslOptions ssl_options;
  opentelemetry::ext::http::client::Headers request_headers;
  opentelemetry::ext::http::client::Body request_body;

  http_client::curl::HttpOperation first(http_client::Method::Get, "http://127.0.0.1:19000/",
                                         ssl_options, nullptr, request_headers, request_body);
  http_client::curl::HttpOperation second(http_client::Method::Get, "http://127.0.0.1:19000/",
                                          ssl_options, nullptr, request_headers, request_body);

  std::thread drawing_first([&first] {
    for (int i = 0; i < 200; ++i)
    {
      (void)first.NextRetryTime();
    }
  });
  std::thread drawing_second([&second] {
    for (int i = 0; i < 200; ++i)
    {
      (void)second.NextRetryTime();
    }
  });

  drawing_first.join();
  drawing_second.join();
}

// resetMultiHandle used to hold sessions_m_ across CancelSession and doRemoveSessions, which
// take it again on the same thread. One registered session is enough to reach both.
TEST_F(BasicCurlHttpTests, ResetMultiHandleWithASessionDoesNotDeadlock)
{
  auto client = std::make_shared<http_client::curl::HttpClient>();

  auto session = client->CreateSession("http://127.0.0.1:19000");
  ASSERT_TRUE(session != nullptr);

  http_client::curl::HttpClientTestPeer::ResetMultiHandle(*client);

  client->FinishAllSessions();
}

// The record queued for removal is the only thing naming the easy handle, the header list it
// points at and the session, so a removal libcurl refuses has to keep all three. The message
// loop reads that session back out of CURLOPT_PRIVATE, which makes letting go of it as much a
// live pointer left behind as a handle.
TEST_F(BasicCurlHttpTests, ARefusedRemovalKeepsTheHandleAndTheSessionItNames)
{
  curl::HttpClient client;

  auto session = client.CreateSession("http://127.0.0.1:19000");
  ASSERT_TRUE(session != nullptr);
  const auto session_id = std::static_pointer_cast<curl::Session>(session)->GetSessionId();

  CURL *easy_handle = curl_easy_init();
  ASSERT_TRUE(easy_handle != nullptr);
  curl_slist *headers_chunk = curl_slist_append(nullptr, "X-Test: keep");
  ASSERT_TRUE(headers_chunk != nullptr);

  http_client::curl::HttpClientTestPeer::NoteAttached(client, easy_handle);
  client.ScheduleRemoveSession(session_id, {easy_handle, headers_chunk});

  // From here the client holds the only reference, so what survives the removal is what the
  // removal chose to keep.
  std::weak_ptr<http_client::Session> watch = session;
  session.reset();

  CURLM *multi_handle = http_client::curl::HttpClientTestPeer::ExchangeMultiHandle(client, nullptr);
  http_client::curl::HttpClientTestPeer::RemoveSessions(client);
  http_client::curl::HttpClientTestPeer::ExchangeMultiHandle(client, multi_handle);

  EXPECT_EQ(1U, http_client::curl::HttpClientTestPeer::QuarantinedCount(client));
  EXPECT_FALSE(watch.expired());
}

// A queued handle still names its session through CURLOPT_PRIVATE, and the message loop reads
// that back. The session is taken when the record is made rather than found when the record is
// drained, because between those two the calling thread returns from FinishSession and takes the
// session out of sessions_, and by then the queue has been swapped away.
TEST_F(BasicCurlHttpTests, AQueuedHandleTakesTheSessionItNamesWithIt)
{
  curl::HttpClient client;

  auto session = client.CreateSession("http://127.0.0.1:19000");
  ASSERT_TRUE(session != nullptr);
  const auto session_id = std::static_pointer_cast<curl::Session>(session)->GetSessionId();

  CURL *easy_handle = curl_easy_init();
  ASSERT_TRUE(easy_handle != nullptr);
  curl_slist *headers_chunk = curl_slist_append(nullptr, "X-Test: owner");
  ASSERT_TRUE(headers_chunk != nullptr);

  client.ScheduleRemoveSession(session_id, {easy_handle, headers_chunk});

  EXPECT_EQ(1U, http_client::curl::HttpClientTestPeer::PendingRemovalOwnerCount(client));

  // What the calling thread does next takes nothing away, because there is nothing left for it
  // to decide.
  std::weak_ptr<http_client::Session> watch = session;
  session.reset();
  client.CleanupSession(session_id);

  EXPECT_FALSE(watch.expired());

  http_client::curl::HttpClientTestPeer::RemoveSessions(client);
  EXPECT_TRUE(watch.expired());
}

// Scheduling an abort takes the session out of sessions_, so a handle queued after that finds
// nothing to take and the abort hands the session over itself. Both steps are the background
// thread's, one after the other, and no request is sent here so there is no background thread to
// run them out of order.
TEST_F(BasicCurlHttpTests, AnAbortedSessionIsGivenToTheHandleItQueued)
{
  curl::HttpClient client;

  auto session = client.CreateSession("http://127.0.0.1:19000");
  ASSERT_TRUE(session != nullptr);
  const auto session_id = std::static_pointer_cast<curl::Session>(session)->GetSessionId();

  client.ScheduleAbortSession(session_id);

  CURL *easy_handle = curl_easy_init();
  ASSERT_TRUE(easy_handle != nullptr);
  curl_slist *headers_chunk = curl_slist_append(nullptr, "X-Test: aborted");
  ASSERT_TRUE(headers_chunk != nullptr);

  client.ScheduleRemoveSession(session_id, {easy_handle, headers_chunk});
  EXPECT_EQ(0U, http_client::curl::HttpClientTestPeer::PendingRemovalOwnerCount(client));

  http_client::curl::HttpClientTestPeer::AbortSessions(client);
  EXPECT_EQ(1U, http_client::curl::HttpClientTestPeer::PendingRemovalOwnerCount(client));

  std::weak_ptr<http_client::Session> watch = session;
  session.reset();
  EXPECT_FALSE(watch.expired());

  http_client::curl::HttpClientTestPeer::RemoveSessions(client);
  EXPECT_TRUE(watch.expired());
}

// A multi handle that was never given a handle has nothing to give back, and asking it is not a
// way to find that out: libcurl 8.10 and 8.11 refuse, and a multi handle that failed to
// initialize refuses on every version. Reading either refusal as ownership strands the handle.
TEST_F(BasicCurlHttpTests, AHandleTheMultiHandleNeverTookIsFreedNotStranded)
{
  curl::HttpClient client;

  CURL *easy_handle = curl_easy_init();
  ASSERT_TRUE(easy_handle != nullptr);
  curl_slist *headers_chunk = curl_slist_append(nullptr, "X-Test: free");
  ASSERT_TRUE(headers_chunk != nullptr);

  client.ScheduleRemoveSession(4405U, {easy_handle, headers_chunk});

  CURLM *multi_handle = http_client::curl::HttpClientTestPeer::ExchangeMultiHandle(client, nullptr);
  http_client::curl::HttpClientTestPeer::RemoveSessions(client);
  http_client::curl::HttpClientTestPeer::ExchangeMultiHandle(client, multi_handle);

  EXPECT_EQ(0U, http_client::curl::HttpClientTestPeer::QuarantinedCount(client));
  EXPECT_EQ(0U, http_client::curl::HttpClientTestPeer::PendingRemovalCount(client));
}

// A handle handed over after the background thread has stopped, or before it ever started, has
// nobody left to drain it, so the client frees it on the way out. Nothing here can watch that
// happen, which leaves this case checking that the record reaches the queue and the sanitizer
// builds checking what becomes of it.
TEST_F(BasicCurlHttpTests, AHandleQueuedWithNoOneLeftToDrainItIsFreedWithTheClient)
{
  curl::HttpClient client;

  CURL *easy_handle = curl_easy_init();
  ASSERT_TRUE(easy_handle != nullptr);
  curl_slist *headers_chunk = curl_slist_append(nullptr, "X-Test: undrained");
  ASSERT_TRUE(headers_chunk != nullptr);

  client.ScheduleRemoveSession(4405U, {easy_handle, headers_chunk});

  EXPECT_EQ(1U, http_client::curl::HttpClientTestPeer::PendingRemovalCount(client));
}

// curl_multi_cleanup detaches whatever the old multi handle held, so a handle recorded against
// it is not attached to anything once it has been replaced, and nothing may be handed back to
// the replacement. Emptying the ledger in the same step is what says so. The replacement is made
// to refuse every removal here, so an offer to it would leave the handle kept back instead of
// freed.
TEST_F(BasicCurlHttpTests, AHandleFromTheReplacedMultiHandleIsNotOfferedToItsSuccessor)
{
  curl::HttpClient client;

  CURL *easy_handle = curl_easy_init();
  ASSERT_TRUE(easy_handle != nullptr);
  curl_slist *headers_chunk = curl_slist_append(nullptr, "X-Test: generation");
  ASSERT_TRUE(headers_chunk != nullptr);

  http_client::curl::HttpClientTestPeer::NoteAttached(client, easy_handle);
  EXPECT_EQ(1U, http_client::curl::HttpClientTestPeer::AttachedCount(client));

  http_client::curl::HttpClientTestPeer::ResetMultiHandle(client);
  EXPECT_EQ(0U, http_client::curl::HttpClientTestPeer::AttachedCount(client));

  client.ScheduleRemoveSession(4405U, {easy_handle, headers_chunk});

  CURLM *multi_handle = http_client::curl::HttpClientTestPeer::ExchangeMultiHandle(client, nullptr);
  http_client::curl::HttpClientTestPeer::RemoveSessions(client);
  http_client::curl::HttpClientTestPeer::ExchangeMultiHandle(client, multi_handle);

  EXPECT_EQ(0U, http_client::curl::HttpClientTestPeer::QuarantinedCount(client));
  EXPECT_EQ(0U, http_client::curl::HttpClientTestPeer::PendingRemovalCount(client));
}

// One record per easy handle. A session that starts another request hands over a second handle
// while the first is still queued, and keying the queue by session would lose the first one.
TEST_F(BasicCurlHttpTests, ASecondHandleFromTheSameSessionDoesNotDisplaceTheFirst)
{
  curl::HttpClient client;

  CURL *first = curl_easy_init();
  ASSERT_TRUE(first != nullptr);
  CURL *second = curl_easy_init();
  ASSERT_TRUE(second != nullptr);
  curl_slist *first_headers = curl_slist_append(nullptr, "X-Test: first");
  ASSERT_TRUE(first_headers != nullptr);
  curl_slist *second_headers = curl_slist_append(nullptr, "X-Test: second");
  ASSERT_TRUE(second_headers != nullptr);

  client.ScheduleRemoveSession(4405U, {first, first_headers});
  client.ScheduleAddSession(4405U);
  client.ScheduleRemoveSession(4405U, {second, second_headers});

  EXPECT_EQ(2U, http_client::curl::HttpClientTestPeer::PendingRemovalCount(client));

  http_client::curl::HttpClientTestPeer::RemoveSessions(client);

  EXPECT_EQ(0U, http_client::curl::HttpClientTestPeer::PendingRemovalCount(client));
  EXPECT_EQ(0U, http_client::curl::HttpClientTestPeer::QuarantinedCount(client));
}

// The same ledger after a real request. The background thread is the only one that writes it and
// it only stops once nothing is running, so what a case can look at is the state it leaves
// behind: an entry left there would send a later handle allocated at the same address through a
// removal it never needed, and nothing should still be waiting to be given back.
TEST_F(BasicCurlHttpTests, TheAttachmentLedgerIsEmptyOnceARequestFinishes)
{
  received_requests_.clear();
  curl::HttpClient client;

  auto session = client.CreateSession("http://127.0.0.1:19000/get/");
  auto request = session->CreateRequest();
  request->SetUri("get/");
  auto handler = std::make_shared<CustomEventHandler>();

  session->SendRequest(handler);
  ASSERT_TRUE(waitForRequests(30, 1));
  session->FinishSession();
  client.WaitBackgroundThreadExit();

  EXPECT_TRUE(handler->got_response_.load(std::memory_order_acquire));
  EXPECT_EQ(0U, http_client::curl::HttpClientTestPeer::AttachedCount(client));
  EXPECT_EQ(0U, http_client::curl::HttpClientTestPeer::QuarantinedCount(client));
}

// The caller-thread side of the same cancel. The server handler takes mtx_requests before it
// answers, so holding it keeps a response from racing the cancel and the abort lands while the
// IO thread is still driving the easy handle. That pairing is what #4369 caught.
TEST_F(BasicCurlHttpTests, ACancelFromTheCallerThreadReportsCancelled)
{
  received_requests_.clear();
  auto session_manager = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session = session_manager->CreateSession("http://127.0.0.1:19000");
  auto request = session->CreateRequest();
  request->SetUri("get/");

  auto handler = std::make_shared<TerminalCountingHandler>();

  {
    std::unique_lock<std::mutex> lock_requests(mtx_requests);
    session->SendRequest(handler);
    session->CancelSession();
    session->FinishSession();
  }

  EXPECT_FALSE(handler->got_response_.load(std::memory_order_acquire));
  EXPECT_EQ(1, handler->cancelled_from_callback_.load(std::memory_order_acquire));

  session_manager->FinishAllSessions();
}

// The same cancel again, repeated, because #4369 is a race and one attempt proves little.
// Nothing listens on 19937, so every attempt fails to connect and the IO thread reaches Cleanup
// while the caller is still inside CancelSession, which is the overlap the race needs. Under a
// thread sanitizer this reports against the unfixed client on every run.
TEST_F(BasicCurlHttpTests, RepeatedCallerThreadCancelsAreClean)
{
  for (int i = 0; i < 20; ++i)
  {
    auto session_manager = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
    ASSERT_TRUE(session_manager != nullptr);

    auto session = session_manager->CreateSession("http://127.0.0.1:19937");
    auto request = session->CreateRequest();
    request->SetUri("get/");

    auto handler = std::make_shared<TerminalCountingHandler>();
    session->SendRequest(handler);
    session->CancelSession();
    session->FinishSession();
    session_manager->FinishAllSessions();

    EXPECT_FALSE(handler->got_response_.load(std::memory_order_acquire));

    // Whichever side wins, the attempt ends somewhere the handler is told about: the cancel
    // lands first, or the connection to a closed port fails first and the cancel is then
    // correctly a no-op. How many cancels arrive is not what this case decides, #4360 tracks
    // that, and neither is which of the two wins.
    EXPECT_GT(handler->terminal_count_.load(std::memory_order_acquire) +
                  handler->failed_count_.load(std::memory_order_acquire),
              0);
  }
}

TEST_F(BasicCurlHttpTests, SendGetRequestSync)
{
  received_requests_.clear();
  curl::HttpClientSync http_client;

  http_client::Headers m1 = {};
  auto result             = http_client.GetNoSsl("http://127.0.0.1:19000/get/", m1);
  EXPECT_EQ(result, true);
  EXPECT_EQ(result.GetSessionState(), http_client::SessionState::Response);
}

TEST_F(BasicCurlHttpTests, HandlerRequestedCloseKeepsServerUsable)
{
  curl::HttpClientSync http_client;
  http_client::Headers m1 = {};

  // A handler returning -1 closes the connection without a response. The server used to keep
  // reading and writing the Connection it had just erased, which AddressSanitizer reports as
  // a use-after-free on this request.
  auto closed = http_client.GetNoSsl("http://127.0.0.1:19000/close/", m1);
  ASSERT_EQ(closed, false);

  // Prove the request actually reached the handler. Without this, an unregistered route or a
  // typo would leave the server answering 404 and the assertion above would be measuring the
  // wrong thing.
  EXPECT_EQ(close_requests_.load(std::memory_order_relaxed), 1U);

  // The server has to still be serving afterwards.
  auto result = http_client.GetNoSsl("http://127.0.0.1:19000/get/", m1);
  EXPECT_EQ(result, true);
  EXPECT_EQ(result.GetSessionState(), http_client::SessionState::Response);
}

TEST_F(BasicCurlHttpTests, SendGetRequestSyncTimeout)
{
  received_requests_.clear();
  curl::HttpClientSync http_client;

  http_client::Headers m1 = {};
  auto result             = http_client.GetNoSsl("https://192.0.2.0:19000/get/", m1);
  EXPECT_EQ(result, false);

  // When network is under proxy, it may connect success but closed by peer when send data
  EXPECT_TRUE(result.GetSessionState() == http_client::SessionState::ConnectFailed ||
              result.GetSessionState() == http_client::SessionState::SendFailed);
}

TEST_F(BasicCurlHttpTests, SendPostRequestSync)
{
  received_requests_.clear();
  curl::HttpClientSync http_client;

  http_client::Headers m1 = {};
  http_client::Body body  = {};
  auto result             = http_client.PostNoSsl("http://127.0.0.1:19000/post/", body, m1);
  EXPECT_EQ(result, true);
  EXPECT_EQ(result.GetSessionState(), http_client::SessionState::Response);
}

TEST_F(BasicCurlHttpTests, GetBaseUri)
{
  curl::HttpClient session_manager;

  auto session = session_manager.CreateSession("127.0.0.1:80");
  ASSERT_EQ(std::static_pointer_cast<curl::Session>(session)->GetBaseUri(), "http://127.0.0.1:80/");

  session = session_manager.CreateSession("https://127.0.0.1:443");
  ASSERT_EQ(std::static_pointer_cast<curl::Session>(session)->GetBaseUri(),
            "https://127.0.0.1:443/");

  session = session_manager.CreateSession("http://127.0.0.1:31339");
  ASSERT_EQ(std::static_pointer_cast<curl::Session>(session)->GetBaseUri(),
            "http://127.0.0.1:31339/");
}

// DISABLED, see https://github.com/open-telemetry/opentelemetry-cpp/issues/3535
TEST_F(DISABLED_BasicCurlHttpTests, SendGetRequestAsync)
{
  curl::HttpClient http_client;

  for (int round = 0; round < 2; ++round)
  {
    received_requests_.clear();
    static constexpr const unsigned batch_count = 5;
    std::shared_ptr<http_client::Session> sessions[batch_count];
    std::shared_ptr<GetEventHandler> handlers[batch_count];
    for (unsigned i = 0; i < batch_count; ++i)
    {
      sessions[i]  = http_client.CreateSession("http://127.0.0.1:19000/get/");
      auto request = sessions[i]->CreateRequest();
      request->SetMethod(http_client::Method::Get);
      request->SetUri("get/");

      handlers[i] = std::make_shared<GetEventHandler>();

      // Lock mtx_requests to prevent response, we will check IsSessionActive() in the end
      std::unique_lock<std::mutex> lock_requests(mtx_requests);
      sessions[i]->SendRequest(handlers[i]);
      ASSERT_TRUE(sessions[i]->IsSessionActive());
    }

    ASSERT_TRUE(waitForRequests(30, batch_count));

    for (unsigned i = 0; i < batch_count; ++i)
    {
      sessions[i]->FinishSession();
      ASSERT_FALSE(sessions[i]->IsSessionActive());

      ASSERT_TRUE(handlers[i]->is_called_.load(std::memory_order_acquire));

      // TODO: Spurious test failures here.
      ASSERT_TRUE(handlers[i]->got_response_.load(std::memory_order_acquire));
    }

    http_client.WaitBackgroundThreadExit();
  }
}

TEST_F(BasicCurlHttpTests, SendGetRequestAsyncTimeout)
{
  received_requests_.clear();
  curl::HttpClient http_client;

  static constexpr const unsigned batch_count = 5;
  std::shared_ptr<http_client::Session> sessions[batch_count];
  std::shared_ptr<GetEventHandler> handlers[batch_count];
  for (unsigned i = 0; i < batch_count; ++i)
  {
    sessions[i]  = http_client.CreateSession("https://192.0.2.0:19000/get/");
    auto request = sessions[i]->CreateRequest();
    request->SetMethod(http_client::Method::Get);
    request->SetUri("get/");
    request->SetTimeoutMs(std::chrono::milliseconds(256));

    handlers[i] = std::make_shared<GetEventHandler>();

    // Lock mtx_requests to prevent response, we will check IsSessionActive() in the end
    std::unique_lock<std::mutex> lock_requests(mtx_requests);
    sessions[i]->SendRequest(handlers[i]);
    ASSERT_TRUE(sessions[i]->IsSessionActive() ||
                handlers[i]->is_called_.load(std::memory_order_acquire));
  }

  for (unsigned i = 0; i < batch_count; ++i)
  {
    sessions[i]->FinishSession();
    ASSERT_FALSE(sessions[i]->IsSessionActive());

    ASSERT_TRUE(handlers[i]->is_called_.load(std::memory_order_acquire));
    ASSERT_FALSE(handlers[i]->got_response_.load(std::memory_order_acquire));
  }
}

TEST_F(BasicCurlHttpTests, SendPostRequestAsync)
{
  curl::HttpClient http_client;

  for (int round = 0; round < 2; ++round)
  {
    received_requests_.clear();
    auto handler = std::make_shared<PostEventHandler>();

    static constexpr const unsigned batch_count = 5;
    std::shared_ptr<http_client::Session> sessions[batch_count];
    for (auto &session : sessions)
    {
      session      = http_client.CreateSession("http://127.0.0.1:19000/post/");
      auto request = session->CreateRequest();
      request->SetMethod(http_client::Method::Post);
      request->SetUri("post/");

      // Lock mtx_requests to prevent response, we will check IsSessionActive() in the end
      std::unique_lock<std::mutex> lock_requests(mtx_requests);
      session->SendRequest(handler);
      ASSERT_TRUE(session->IsSessionActive());
    }

    ASSERT_TRUE(waitForRequests(30, batch_count));

    for (auto &session : sessions)
    {
      session->FinishSession();
      ASSERT_FALSE(session->IsSessionActive());
    }

    ASSERT_TRUE(handler->is_called_.load(std::memory_order_acquire));
    ASSERT_TRUE(handler->got_response_.load(std::memory_order_acquire));

    http_client.WaitBackgroundThreadExit();
  }
}

TEST_F(BasicCurlHttpTests, FinishInAsyncCallback)
{
  curl::HttpClient http_client;

  for (int round = 0; round < 2; ++round)
  {
    received_requests_.clear();
    static constexpr const unsigned batch_count = 5;
    std::shared_ptr<http_client::Session> sessions[batch_count];
    std::shared_ptr<FinishInCallbackHandler> handlers[batch_count];
    for (unsigned i = 0; i < batch_count; ++i)
    {
      sessions[i]  = http_client.CreateSession("http://127.0.0.1:19000/get/");
      auto request = sessions[i]->CreateRequest();
      request->SetMethod(http_client::Method::Get);
      request->SetUri("get/");

      handlers[i] = std::make_shared<FinishInCallbackHandler>(sessions[i]);

      // Lock mtx_requests to prevent response, we will check IsSessionActive() in the end
      std::unique_lock<std::mutex> lock_requests(mtx_requests);
      sessions[i]->SendRequest(handlers[i]);
      ASSERT_TRUE(sessions[i]->IsSessionActive());
    }

    http_client.WaitBackgroundThreadExit();
    ASSERT_TRUE(waitForRequests(300, batch_count));

    for (unsigned i = 0; i < batch_count; ++i)
    {
      ASSERT_FALSE(sessions[i]->IsSessionActive());

      ASSERT_TRUE(handlers[i]->is_called_.load(std::memory_order_acquire));
      ASSERT_TRUE(handlers[i]->got_response_.load(std::memory_order_acquire));
    }
  }
}

TEST_F(BasicCurlHttpTests, ElegantQuitQuick)
{
  auto http_client = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  std::static_pointer_cast<curl::HttpClient>(http_client)->MaybeSpawnBackgroundThread();
  // start background first, then test it could wakeup
  auto session = http_client->CreateSession("http://127.0.0.1:19000/get/");
  auto request = session->CreateRequest();
  request->SetUri("get/");
  auto handler = std::make_shared<GetEventHandler>();
  session->SendRequest(handler);
  std::this_thread::sleep_for(std::chrono::milliseconds{10});  // let it enter poll state
  auto beg = std::chrono::system_clock::now();
  http_client->FinishAllSessions();
  http_client.reset();
  // when background_thread_wait_for_ is used, it should have no side effect on elegant quit
  // wait should be less than scheduled_delay_milliseconds_
  // Due to load on CI hosts (some take 10ms), we assert it is less than 20ms
  auto cost = std::chrono::system_clock::now() - beg;
  ASSERT_TRUE(cost < std::chrono::milliseconds{20})
      << "cost ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(cost).count()
      << " libcurl version: 0x" << std::hex << LIBCURL_VERSION_NUM;
  ASSERT_TRUE(handler->is_called_);
  ASSERT_TRUE(handler->got_response_);
}

TEST_F(BasicCurlHttpTests, BackgroundThreadWaitMore)
{
  {
    curl::HttpClient http_client;
    http_client.MaybeSpawnBackgroundThread();
    std::this_thread::sleep_for(std::chrono::milliseconds{10});
#if LIBCURL_VERSION_NUM >= 0x074200
    ASSERT_FALSE(http_client.MaybeSpawnBackgroundThread());
#else
    // low version curl do not support delay quit, so old background would quit
    ASSERT_TRUE(http_client.MaybeSpawnBackgroundThread());
#endif
  }
  {
    curl::HttpClient http_client;
    http_client.SetBackgroundWaitFor(std::chrono::milliseconds::zero());
    http_client.MaybeSpawnBackgroundThread();
    std::this_thread::sleep_for(std::chrono::milliseconds{10});
    // we can disable delay quit by set wait for 0
    ASSERT_TRUE(http_client.MaybeSpawnBackgroundThread());
  }
}

#ifdef ENABLE_OTLP_COMPRESSION_PREVIEW
struct GzipEventHandler : public CustomEventHandler
{
  void OnResponse(http_client::Response & /* response */) noexcept override {}

  void OnEvent(http_client::SessionState state, nostd::string_view reason) noexcept override
  {
    is_called_ = true;
    state_     = state;
    reason_    = std::string{reason};
  }

  bool is_called_                  = false;
  http_client::SessionState state_ = static_cast<http_client::SessionState>(-1);
  std::string reason_;
};

TEST_F(BasicCurlHttpTests, GzipCompressibleData)
{
  received_requests_.clear();
  auto session_manager = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session = session_manager->CreateSession("http://127.0.0.1:19000");
  auto request = session->CreateRequest();
  request->SetUri("post/");
  request->SetMethod(http_client::Method::Post);

  const auto original_size = 500UL;
  http_client::Body body(original_size);
  std::iota(body.begin(), body.end(), 0);
  request->SetBody(body);
  request->AddHeader("Content-Type", "text/plain");
  request->SetCompression(opentelemetry::ext::http::client::Compression::kGzip);
  auto handler = std::make_shared<GzipEventHandler>();
  session->SendRequest(handler);
  ASSERT_TRUE(waitForRequests(30, 1));
  session->FinishSession();
  ASSERT_TRUE(handler->is_called_);
  ASSERT_EQ(handler->state_, http_client::SessionState::Response);
  ASSERT_TRUE(handler->reason_.empty());

  auto http_request =
      dynamic_cast<opentelemetry::ext::http::client::curl::Request *>(request.get());
  ASSERT_TRUE(http_request != nullptr);
  ASSERT_LT(http_request->body_.size(), original_size);

  session_manager->CancelAllSessions();
  session_manager->FinishAllSessions();
}

TEST_F(BasicCurlHttpTests, GzipIncompressibleData)
{
  received_requests_.clear();
  auto session_manager = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session = session_manager->CreateSession("http://127.0.0.1:19000");
  auto request = session->CreateRequest();
  request->SetUri("post/");
  request->SetMethod(http_client::Method::Post);

  // Random data generated using code snippet below.
  //   const auto original_size = 500UL;
  //   http_client::Body body(original_size);
  //   std::random_device rd;
  //   std::mt19937 gen(rd());
  //   std::uniform_int_distribution<> uid(1, 255);
  //   std::generate(body.begin(), body.end(), [&]() { return uid(gen); });

  // The input values are fixed to make the test repeatable in the event that some distributions
  // might yield results that are, in fact, compressible.
  http_client::Body body = {
      140, 198, 12,  56,  165, 185, 173, 20,  13,  83,  127, 223, 77,  38,  224, 43,  236, 10,  178,
      75,  169, 157, 136, 199, 74,  30,  148, 195, 51,  30,  225, 21,  121, 219, 7,   155, 198, 121,
      205, 102, 80,  38,  132, 202, 45,  229, 206, 90,  150, 202, 53,  221, 54,  37,  172, 90,  238,
      248, 191, 240, 109, 227, 248, 41,  251, 121, 35,  226, 107, 122, 15,  242, 203, 45,  64,  195,
      186, 23,  1,   158, 61,  196, 182, 26,  201, 47,  211, 241, 251, 209, 255, 170, 181, 192, 89,
      133, 176, 60,  178, 97,  168, 223, 152, 9,   118, 98,  169, 240, 170, 15,  13,  161, 24,  57,
      123, 117, 230, 30,  244, 117, 238, 255, 198, 232, 95,  148, 37,  61,  67,  103, 31,  240, 52,
      21,  145, 175, 201, 86,  19,  61,  228, 76,  131, 185, 111, 149, 203, 143, 16,  142, 95,  173,
      42,  106, 39,  203, 116, 235, 20,  162, 112, 173, 112, 70,  126, 191, 210, 219, 90,  145, 126,
      118, 43,  241, 101, 66,  175, 179, 5,   233, 208, 164, 180, 83,  214, 194, 173, 29,  179, 149,
      75,  202, 17,  152, 139, 130, 94,  247, 142, 249, 159, 224, 205, 131, 93,  82,  186, 226, 210,
      84,  17,  212, 155, 61,  226, 103, 152, 37,  3,   193, 216, 219, 203, 101, 99,  33,  59,  38,
      106, 62,  232, 127, 44,  125, 90,  169, 148, 238, 34,  106, 12,  221, 90,  173, 67,  122, 232,
      161, 89,  198, 43,  241, 195, 248, 219, 35,  47,  200, 11,  227, 168, 246, 243, 103, 38,  17,
      203, 237, 203, 158, 204, 89,  231, 19,  24,  25,  199, 160, 233, 43,  117, 144, 196, 117, 152,
      42,  121, 189, 217, 202, 221, 250, 157, 237, 47,  29,  64,  32,  10,  32,  243, 28,  114, 158,
      228, 102, 36,  191, 139, 217, 161, 162, 186, 19,  141, 212, 49,  1,   239, 153, 107, 249, 31,
      235, 138, 73,  80,  58,  152, 15,  149, 50,  42,  84,  75,  95,  82,  56,  86,  143, 45,  214,
      11,  184, 164, 181, 249, 74,  184, 26,  207, 165, 162, 240, 154, 90,  56,  175, 72,  4,   166,
      188, 78,  232, 87,  243, 50,  59,  62,  175, 213, 210, 182, 31,  123, 91,  118, 98,  249, 23,
      170, 240, 228, 236, 121, 87,  132, 129, 250, 41,  227, 204, 250, 147, 145, 109, 149, 210, 21,
      174, 165, 127, 234, 64,  211, 52,  93,  126, 117, 231, 216, 210, 15,  16,  2,   167, 215, 178,
      104, 245, 119, 211, 235, 120, 135, 202, 117, 150, 101, 94,  201, 136, 179, 205, 167, 212, 236,
      7,   178, 132, 228, 65,  230, 90,  171, 109, 31,  83,  31,  210, 123, 136, 76,  186, 81,  205,
      63,  35,  21,  121, 152, 22,  242, 199, 106, 217, 199, 211, 206, 165, 88,  77,  112, 108, 193,
      122, 8,   193, 74,  91,  50,  6,   156, 185, 165, 15,  92,  116, 3,   18,  244, 165, 191, 2,
      183, 9,   164, 116, 75,  127};
  const auto original_size = body.size();

  request->SetBody(body);
  request->AddHeader("Content-Type", "text/plain");
  request->SetCompression(opentelemetry::ext::http::client::Compression::kGzip);
  auto handler = std::make_shared<GzipEventHandler>();
  session->SendRequest(handler);
  ASSERT_TRUE(waitForRequests(30, 1));
  session->FinishSession();
  ASSERT_TRUE(handler->is_called_);
  ASSERT_EQ(handler->state_, http_client::SessionState::Response);
  ASSERT_TRUE(handler->reason_.empty());

  auto http_request =
      dynamic_cast<opentelemetry::ext::http::client::curl::Request *>(request.get());
  ASSERT_TRUE(http_request != nullptr);
  ASSERT_EQ(http_request->body_.size(), original_size);

  session_manager->CancelAllSessions();
  session_manager->FinishAllSessions();
}
#endif  // ENABLE_OTLP_COMPRESSION_PREVIEW

}  // namespace
