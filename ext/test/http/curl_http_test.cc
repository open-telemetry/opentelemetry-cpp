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
#include <cstdlib>
#include <cstring>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "opentelemetry/ext//http/client/curl/http_client_curl.h"
#include "opentelemetry/ext/http/client/curl/http_client_factory_curl.h"
#include "opentelemetry/ext/http/client/curl/http_operation_curl.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/server/http_server.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/common/thread_instrumentation.h"
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

// curl_global_init_mem is the only way to reach the failure returns of curl_slist_append and
// curl_multi_init. It must be called before libcurl is initialised: afterwards it returns
// CURLE_OK and changes nothing. The switches are thread local, so arming one from a test cannot
// disturb a client's background thread.
extern "C" {
static std::atomic<bool> g_curl_hooks_ran{false};
static thread_local bool g_fail_curl_malloc = false;
static thread_local bool g_fail_curl_calloc = false;

// NOLINTBEGIN(cppcoreguidelines-no-malloc,hicpp-no-malloc): these are the allocator libcurl
// is given, so reaching for the C allocation functions is the point of them.
// A curl_slist node is two pointers, and a curl easy handle is thousands of bytes on every
// libcurl, so the bound aims the failure at the list append. Which call consumes the first
// failing allocation is otherwise a property of the libcurl in use rather than of this test.
static const size_t kCurlSmallAllocation = 64;

static void *CurlTestMalloc(size_t size)
{
  g_curl_hooks_ran.store(true, std::memory_order_relaxed);
  if (g_fail_curl_malloc && size <= kCurlSmallAllocation)
  {
    return nullptr;
  }
  return std::malloc(size);
}

static void CurlTestFree(void *ptr)
{
  std::free(ptr);
}

static void *CurlTestRealloc(void *ptr, size_t size)
{
  return std::realloc(ptr, size);
}

// Not routed through CurlTestMalloc on purpose, so that a failing malloc cannot reach the
// copies libcurl makes of the caller's strings and land somewhere other than the list node.
static char *CurlTestStrdup(const char *str)
{
  const size_t length = std::strlen(str) + 1;
  char *copy          = static_cast<char *>(std::malloc(length));
  if (copy != nullptr)
  {
    std::memcpy(copy, str, length);
  }
  return copy;
}

// Aimed at curl_multi_init, which allocates with calloc on the libcurl this was measured
// against. That is not part of libcurl's contract, so the cases assert on what actually failed.
// Not thread local, unlike the switches above: the failure has to reach the IO thread.
static std::atomic<bool> g_fail_curl_calloc_everywhere{false};

// Counts what the process wide switch refused, which is one per curl_multi_init the IO thread
// tried while it had no handle. That is the retry rate, measured the same way everywhere.
static std::atomic<int> g_curl_calloc_failures{0};

// Exempts one thread from the process wide switch. A test that has to build a request while the
// IO thread cannot create a handle needs its own allocations to keep working.
static thread_local bool g_curl_calloc_exempt = false;

static void *CurlTestCalloc(size_t count, size_t size)
{
  g_curl_hooks_ran.store(true, std::memory_order_relaxed);
  if (g_fail_curl_calloc_everywhere.load(std::memory_order_relaxed) && !g_curl_calloc_exempt)
  {
    g_curl_calloc_failures.fetch_add(1, std::memory_order_relaxed);
    return nullptr;
  }
  if (g_fail_curl_calloc)
  {
    return nullptr;
  }
  return std::calloc(count, size);
}
// NOLINTEND(cppcoreguidelines-no-malloc,hicpp-no-malloc)
}  // extern "C"

namespace
{
bool g_curl_hooks_installed = false;

struct FailCurlMalloc
{
  FailCurlMalloc() { g_fail_curl_malloc = true; }
  ~FailCurlMalloc() { g_fail_curl_malloc = false; }
  FailCurlMalloc(const FailCurlMalloc &)            = delete;
  FailCurlMalloc(FailCurlMalloc &&)                 = delete;
  FailCurlMalloc &operator=(const FailCurlMalloc &) = delete;
  FailCurlMalloc &operator=(FailCurlMalloc &&)      = delete;
};

struct FailCurlCalloc
{
  FailCurlCalloc() { g_fail_curl_calloc = true; }
  ~FailCurlCalloc() { g_fail_curl_calloc = false; }
  FailCurlCalloc(const FailCurlCalloc &)            = delete;
  FailCurlCalloc(FailCurlCalloc &&)                 = delete;
  FailCurlCalloc &operator=(const FailCurlCalloc &) = delete;
  FailCurlCalloc &operator=(FailCurlCalloc &&)      = delete;
};

// Counts terminal outcomes without caring which one, since a client whose multi handle could
// not be created may still recover and answer, and the case below is about the caller being
// told either way rather than about which answer it gets.
// Counts internal log lines, so a case can hold that a run of failures is reported a bounded
// number of times rather than once per pass of the IO loop.
class CountingLogHandler : public opentelemetry::sdk::common::internal_log::LogHandler
{
public:
  void Handle(opentelemetry::sdk::common::internal_log::LogLevel /* level */,
              const char * /* file */,
              int /* line */,
              const char * /* msg */,
              const opentelemetry::sdk::common::AttributeMap & /* attributes */) noexcept override
  {
    count_.fetch_add(1, std::memory_order_relaxed);
  }

  std::atomic<int> count_{0};
};

class MultiHandleOutcomeHandler : public http_client::EventHandler
{
public:
  void OnResponse(http_client::Response & /* response */) noexcept override
  {
    responses_.fetch_add(1, std::memory_order_release);
    terminal_.fetch_add(1, std::memory_order_release);
  }

  void OnEvent(http_client::SessionState state, nostd::string_view /* reason */) noexcept override
  {
    switch (state)
    {
      case http_client::SessionState::Cancelled:
        cancels_.fetch_add(1, std::memory_order_release);
        terminal_.fetch_add(1, std::memory_order_release);
        break;
      case http_client::SessionState::CreateFailed:
        create_failed_.fetch_add(1, std::memory_order_release);
        terminal_.fetch_add(1, std::memory_order_release);
        break;
      case http_client::SessionState::ConnectFailed:
        connect_failed_.fetch_add(1, std::memory_order_release);
        terminal_.fetch_add(1, std::memory_order_release);
        break;
      case http_client::SessionState::SendFailed:
        terminal_.fetch_add(1, std::memory_order_release);
        break;
      default:
        break;
    }
  }

  std::atomic<int> terminal_{0};
  std::atomic<int> responses_{0};
  std::atomic<int> cancels_{0};
  std::atomic<int> create_failed_{0};
  std::atomic<int> connect_failed_{0};
};

class CapturingLogHandler : public opentelemetry::sdk::common::internal_log::LogHandler
{
public:
  void Handle(opentelemetry::sdk::common::internal_log::LogLevel,
              const char *,
              int,
              const char *msg,
              const opentelemetry::sdk::common::AttributeMap &) noexcept override
  {
    if (msg == nullptr)
    {
      return;
    }
    std::lock_guard<std::mutex> lock(messages_m_);
    messages_.append(msg).append("\n");
  }

  std::string Text()
  {
    std::lock_guard<std::mutex> lock(messages_m_);
    return messages_;
  }

private:
  std::mutex messages_m_;
  std::string messages_;
};

class ReportedStateHandler : public CustomEventHandler
{
public:
  std::atomic<bool> create_failed_{false};
  std::atomic<int> terminal_count_{0};

  void OnResponse(http_client::Response &) noexcept override
  {
    terminal_count_.fetch_add(1, std::memory_order_acq_rel);
  }

  void OnEvent(http_client::SessionState state, nostd::string_view reason) noexcept override
  {
    switch (state)
    {
      case http_client::SessionState::CreateFailed:
      case http_client::SessionState::ConnectFailed:
      case http_client::SessionState::SendFailed:
      case http_client::SessionState::SSLHandshakeFailed:
      case http_client::SessionState::TimedOut:
      case http_client::SessionState::NetworkError:
      case http_client::SessionState::Cancelled:
        terminal_count_.fetch_add(1, std::memory_order_acq_rel);
        break;
      default:
        break;
    }

    if (state != http_client::SessionState::CreateFailed)
    {
      return;
    }
    {
      std::lock_guard<std::mutex> lock(reason_m_);
      reason_.assign(reason.data(), reason.size());
    }
    create_failed_.store(true, std::memory_order_release);
  }

  std::string Reason()
  {
    std::lock_guard<std::mutex> lock(reason_m_);
    return reason_;
  }

private:
  std::mutex reason_m_;
  std::string reason_;
};
}  // namespace

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

  // Runs once before the first case, which is the only point still ahead of the first
  // HttpClient and therefore ahead of curl_global_init.
  static void SetUpTestSuite()
  {
    g_curl_hooks_installed =
        (CURLE_OK == curl_global_init_mem(CURL_GLOBAL_ALL, CurlTestMalloc, CurlTestFree,
                                          CurlTestRealloc, CurlTestStrdup, CurlTestCalloc));
  }

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
  int terminal_total = 0;

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
    terminal_total += handler->terminal_count_.load(std::memory_order_acquire);
  }

  // A lower bound, not a count: #4360 tracks the same cancel arriving twice, and how many
  // arrive is not what this case decides.
  EXPECT_GE(terminal_total, 20);
}

// Without this the two cases below would fail for the wrong reason if the hooks ever stopped
// being installed early enough, and the message would not say so.
TEST_F(BasicCurlHttpTests, CurlAllocationHooksAreInstalled)
{
  EXPECT_TRUE(g_curl_hooks_installed) << "curl_global_init_mem did not return CURLE_OK";

  auto session_manager = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  ASSERT_TRUE(session_manager != nullptr);
  session_manager->FinishAllSessions();

  EXPECT_TRUE(g_curl_hooks_ran.load(std::memory_order_relaxed))
      << "libcurl allocated without calling the hooks, so they were installed too late";
}

// A header list that cannot be built has to end the operation. Reporting it is what stops the
// request going out with none of the caller's headers, which for an OTLP export means no
// Content-Type and a receiver that rejects it.
TEST_F(BasicCurlHttpTests, AFailedHeaderAllocationIsReported)
{
  ASSERT_TRUE(g_curl_hooks_installed);

  received_requests_.clear();

  auto session_manager = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  auto session         = session_manager->CreateSession("http://127.0.0.1:19000");
  auto request         = session->CreateRequest();
  request->SetUri("get/");
  request->AddHeader("X-Test", "1");

  auto handler = std::make_shared<ReportedStateHandler>();
  {
    // The operation is constructed on this thread inside SendRequest, so the switch reaches
    // only its allocations.
    FailCurlMalloc fail;
    session->SendRequest(handler);
  }

  session->FinishSession();
  session_manager->FinishAllSessions();

  size_t requests_seen = 0;
  {
    std::unique_lock<std::mutex> lock_requests(mtx_requests);
    requests_seen = received_requests_.size();
  }

  EXPECT_TRUE(handler->create_failed_.load(std::memory_order_acquire))
      << "a header list that could not be built was not reported";
  // Reporting it is only half. The easy handle is still valid here and Setup() skips
  // CURLOPT_HTTPHEADER when the list is null, so without a construction result the request goes
  // out anyway, carrying none of the caller's headers.
  EXPECT_EQ(static_cast<size_t>(0), requests_seen)
      << "the request reached the server after the failure was reported";

  // Everything past here is specific to the header list having been the allocation that failed.
  // A libcurl that took the failure somewhere else reports its own message, and says so rather
  // than asserting against a path it did not take.
  const std::string reason = handler->Reason();
  if (std::string::npos == reason.find("Out of memory"))
  {
    GTEST_SKIP() << "this libcurl consumed the failing allocation before the header list, "
                 << "reported as: " << reason;
  }

  EXPECT_EQ(1, handler->terminal_count_.load(std::memory_order_acquire))
      << "expected exactly one terminal outcome";
}

// A client whose multi handle is null accepts sessions, adds none of them, and completes none
// of them, so the failure has to be visible somewhere.
TEST_F(BasicCurlHttpTests, AFailedMultiHandleAllocationIsReported)
{
  ASSERT_TRUE(g_curl_hooks_installed);

  // One ordinary client first, so the global curl initializer already exists and the switch
  // below can only reach curl_multi_init.
  {
    auto warmup = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
    ASSERT_TRUE(warmup != nullptr);
    warmup->FinishAllSessions();
  }

  auto *capture = new CapturingLogHandler();
  auto previous = opentelemetry::sdk::common::internal_log::GlobalLogHandler::GetLogHandler();
  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler(
      nostd::shared_ptr<opentelemetry::sdk::common::internal_log::LogHandler>(capture));

  {
    FailCurlCalloc fail;
    auto client = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
    ASSERT_TRUE(client != nullptr);
    client->FinishAllSessions();
  }

  const std::string text = capture->Text();
  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler(previous);

  EXPECT_NE(std::string::npos, text.find("curl_multi_init failed"))
      << "a multi handle that could not be created was not reported, captured: " << text;
}

// SendAsync refuses a request whose header list could not be built, and Send has to refuse it
// the same way, or a synchronous caller puts one on the wire carrying none of its headers.
TEST_F(BasicCurlHttpTests, ASynchronousSendRefusesAFailedHeaderAllocation)
{
  ASSERT_TRUE(g_curl_hooks_installed);
  received_requests_.clear();

  // The operation keeps references to these, so they outlive it.
  const http_client::HttpSslOptions no_ssl;
  const http_client::Body body;
  const http_client::Headers headers         = {{"X-Test", "1"}};
  const http_client::Compression compression = http_client::Compression::kNone;

  std::unique_ptr<curl::HttpOperation> operation;
  {
    // Only the constructor allocates under the switch. Send needs its own allocations to work.
    FailCurlMalloc fail;
    operation.reset(new curl::HttpOperation(http_client::Method::Get, "http://127.0.0.1:19000/get/",
                                            no_ssl, nullptr, headers, body, compression));
  }

  const CURLcode result = operation->Send();

  size_t requests_seen = 0;
  {
    std::unique_lock<std::mutex> lock_requests(mtx_requests);
    requests_seen = received_requests_.size();
  }

  EXPECT_EQ(static_cast<size_t>(0), requests_seen)
      << "a synchronous request went out after its setup had failed";

  if (CURLE_OUT_OF_MEMORY != result)
  {
    GTEST_SKIP() << "this libcurl consumed the failing allocation before the header list, "
                 << "reported as: " << curl_easy_strerror(result);
  }
  EXPECT_EQ(CURLE_OUT_OF_MEMORY, operation->GetLastResultCode())
      << "the refusal was not recorded as the last result";
}

// Both constructors reach the multi handle through the same helper, so the overload that takes
// thread instrumentation reports a failed one too. A null instrumentation is enough to pick it.
TEST_F(BasicCurlHttpTests, AFailedMultiHandleIsReportedForAnInstrumentedClient)
{
  ASSERT_TRUE(g_curl_hooks_installed);

  {
    auto warmup = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
    ASSERT_TRUE(warmup != nullptr);
    warmup->FinishAllSessions();
  }

  auto *capture = new CapturingLogHandler();
  auto previous = opentelemetry::sdk::common::internal_log::GlobalLogHandler::GetLogHandler();
  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler(
      nostd::shared_ptr<opentelemetry::sdk::common::internal_log::LogHandler>(capture));

  {
    FailCurlCalloc fail;
    auto client = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create(
        std::shared_ptr<opentelemetry::sdk::common::ThreadInstrumentation>{});
    ASSERT_TRUE(client != nullptr);
    client->FinishAllSessions();
  }

  const std::string text = capture->Text();
  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler(previous);

  EXPECT_NE(std::string::npos, text.find("curl_multi_init failed"))
      << "the instrumented constructor did not report a multi handle it could not create, "
      << "captured: " << text;
}

// Reporting the failure does not by itself stop the client taking requests, so this holds what
// taking one leads to. The IO loop creates a new multi handle when curl_multi_perform rejects
// the one it has, so a client built on a null handle usually recovers and answers, and reports a
// failure instead when the allocation is still failing by then. Either is fine. Neither is not,
// and that is what this checks, so it does not assert which one arrives.
TEST_F(BasicCurlHttpTests, AClientWithoutAMultiHandleReachesATerminalOutcome)
{
  ASSERT_TRUE(g_curl_hooks_installed);
  received_requests_.clear();

  {
    auto warmup = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
    ASSERT_TRUE(warmup != nullptr);
    warmup->FinishAllSessions();
  }

  std::shared_ptr<http_client::HttpClient> client;
  {
    FailCurlCalloc fail;
    client = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  }
  ASSERT_TRUE(client != nullptr);

  auto session = client->CreateSession("http://127.0.0.1:19000");
  auto request = session->CreateRequest();
  request->SetUri("get/");

  auto handler = std::make_shared<MultiHandleOutcomeHandler>();
  session->SendRequest(handler);

  for (int i = 0; i < 300 && 0 == handler->terminal_.load(std::memory_order_acquire); ++i)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  EXPECT_GE(handler->terminal_.load(std::memory_order_acquire), 1)
      << "the request was accepted and never reached an outcome";
  EXPECT_FALSE(session->IsSessionActive()) << "the session stayed active with nothing running it";

  // Cancel before finishing. If either assertion above failed then nothing is going to complete
  // this operation, so the case would hang rather
  // than fail. Cancelling a session that already answered does nothing.
  session->CancelSession();
  session->FinishSession();
  client->FinishAllSessions();
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

// A client whose multi handle can never be created has nothing to run. The IO loop reports a run
// of failures once and waits between attempts, so a client left alive in that state costs neither
// a core nor a log line per pass, and this holds both.
// The phases the loop gates on a multi handle move sessions between queues. Ungated, a session
// queued while the handle is missing leaves the pending queue for a multi function that cannot
// take it, and the next reset cancels it, so the caller is told a request was cancelled that
// nothing cancelled.
TEST_F(BasicCurlHttpTests, AQueuedRequestSurvivesAMissingMultiHandle)
{
  ASSERT_TRUE(g_curl_hooks_installed);
  received_requests_.clear();

  auto client = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  ASSERT_TRUE(client != nullptr);
  auto *concrete = static_cast<http_client::curl::HttpClient *>(client.get());

  // The idle grace is a minute by default, but only from libcurl 7.68: the assignment that
  // gives it that value is behind a version check, and older libcurl leaves it at zero, where
  // the IO thread reaches the retirement check on its first idle pass. CMake asks for no
  // minimum libcurl, so both are supported and this asks for the shorter one, to run the same
  // way everywhere rather than the way whichever libcurl the job has happens to allow.
  concrete->SetBackgroundWaitFor(std::chrono::milliseconds::zero());

  // One completed request, so the IO thread exists and is running the loop under test.
  {
    auto warm         = client->CreateSession("http://127.0.0.1:19000");
    auto warm_request = warm->CreateRequest();
    warm_request->SetUri("get/");
    auto warm_handler = std::make_shared<MultiHandleOutcomeHandler>();
    warm->SendRequest(warm_handler);
    ASSERT_TRUE(waitForRequests(30, 1));
    warm->FinishSession();
    ASSERT_GE(warm_handler->responses_.load(std::memory_order_acquire), 1);
  }
  received_requests_.clear();

  // Join the IO thread before taking its multi handle away. resetMultiHandle destroys the one
  // it finds, and a handle another thread is inside is not one to destroy.
  concrete->WaitBackgroundThreadExit();

  g_curl_calloc_failures.store(0, std::memory_order_relaxed);
  g_fail_curl_calloc_everywhere.store(true, std::memory_order_relaxed);
  http_client::curl::HttpClientTestPeer::ResetMultiHandle(*concrete);

  // curl_easy_init allocates with calloc too, so without this the request below could not be
  // built and the case would test the refusal rather than the queue.
  g_curl_calloc_exempt = true;

  auto session = client->CreateSession("http://127.0.0.1:19000");
  auto request = session->CreateRequest();
  request->SetUri("get/");
  auto handler = std::make_shared<MultiHandleOutcomeHandler>();
  session->SendRequest(handler);

  // Wait for the IO thread to go round several times with no handle, so the gated phases have
  // had every chance to consume the queued session.
  for (int i = 0; i < 200 && g_curl_calloc_failures.load(std::memory_order_relaxed) < 5; ++i)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  ASSERT_GE(g_curl_calloc_failures.load(std::memory_order_relaxed), 1)
      << "the IO thread never ran without a handle, so nothing was tested";

  g_fail_curl_calloc_everywhere.store(false, std::memory_order_relaxed);
  g_curl_calloc_exempt = false;

  for (int i = 0; i < 300 && 0 == handler->terminal_.load(std::memory_order_acquire); ++i)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  EXPECT_EQ(0, handler->cancels_.load(std::memory_order_acquire))
      << "the queued request was cancelled although nothing cancelled it";
  EXPECT_GE(handler->responses_.load(std::memory_order_acquire), 1)
      << "the request queued while the handle was missing never reached the wire";

  session->CancelSession();
  session->FinishSession();
  client->FinishAllSessions();
}

// curl_easy_init can fail as well, and the operation then holds no handle. It has to be refused
// like a failed header list, so one request produces one kind of failure rather than a create
// failure from the constructor followed by a connect failure from the null handle.
TEST_F(BasicCurlHttpTests, AFailedEasyHandleIsReportedOnce)
{
  ASSERT_TRUE(g_curl_hooks_installed);
  received_requests_.clear();

  auto client = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  ASSERT_TRUE(client != nullptr);

  auto session = client->CreateSession("http://127.0.0.1:19000");
  auto request = session->CreateRequest();
  request->SetUri("get/");
  auto handler = std::make_shared<MultiHandleOutcomeHandler>();

  // Armed here so it reaches the curl_easy_init inside SendRequest and nothing before it.
  g_fail_curl_calloc_everywhere.store(true, std::memory_order_relaxed);
  session->SendRequest(handler);
  g_fail_curl_calloc_everywhere.store(false, std::memory_order_relaxed);

  EXPECT_EQ(1, handler->create_failed_.load(std::memory_order_acquire))
      << "one failed handle was not described exactly once";
  EXPECT_EQ(1, handler->terminal_.load(std::memory_order_acquire))
      << "one request produced more than one terminal outcome";
  EXPECT_EQ(0, handler->connect_failed_.load(std::memory_order_acquire))
      << "a request with no easy handle still reported a connect failure";
  EXPECT_EQ(0, handler->responses_.load(std::memory_order_acquire));
  EXPECT_FALSE(session->IsSessionActive()) << "the session stayed active with nothing running it";
  EXPECT_EQ(0U, received_requests_.size()) << "a request with no easy handle reached the server";

  session->FinishSession();
  client->FinishAllSessions();
}

TEST_F(BasicCurlHttpTests, APersistentMultiHandleFailureDoesNotSpin)
{
  ASSERT_TRUE(g_curl_hooks_installed);
  received_requests_.clear();

  auto client = std::make_shared<http_client::curl::HttpCurlClientFactory>()->Create();
  ASSERT_TRUE(client != nullptr);

  // One completed request, so the IO thread exists and the loop below is the one under test.
  {
    auto warm         = client->CreateSession("http://127.0.0.1:19000");
    auto warm_request = warm->CreateRequest();
    warm_request->SetUri("get/");
    auto warm_handler = std::make_shared<MultiHandleOutcomeHandler>();
    warm->SendRequest(warm_handler);
    ASSERT_TRUE(waitForRequests(30, 1));
    warm->FinishSession();
    ASSERT_GE(warm_handler->terminal_.load(std::memory_order_acquire), 1);
  }

  auto *capture = new CountingLogHandler();
  auto previous = opentelemetry::sdk::common::internal_log::GlobalLogHandler::GetLogHandler();
  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler(
      nostd::shared_ptr<opentelemetry::sdk::common::internal_log::LogHandler>(capture));

  int attempts = 0;
  {
    // The hooks serve libcurl only, so this fails curl_multi_init on the IO thread without
    // touching the allocations the rest of the binary makes.
    g_curl_calloc_failures.store(0, std::memory_order_relaxed);
    g_fail_curl_calloc_everywhere.store(true, std::memory_order_relaxed);
    auto *concrete = static_cast<http_client::curl::HttpClient *>(client.get());
    http_client::curl::HttpClientTestPeer::ResetMultiHandle(*concrete);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    attempts = g_curl_calloc_failures.load(std::memory_order_relaxed);
    g_fail_curl_calloc_everywhere.store(false, std::memory_order_relaxed);
  }
  const int log_lines = capture->count_.load(std::memory_order_relaxed);
  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler(previous);

  // Counting what the allocator refused says how often the IO thread tried, which is what a spin
  // is, and says it the same way on every platform. The lower bounds matter as much as the upper
  // ones: without them an injection that stopped working reads as a pass.
  EXPECT_GE(attempts, 1) << "the IO thread never tried to create a handle, so nothing was tested";
  EXPECT_LE(attempts, 64) << "the IO thread tried " << attempts
                          << " times in a second, which is a spin rather than a wait";
  EXPECT_GE(log_lines, 1) << "the failure was never reported";
  EXPECT_LE(log_lines, 8) << "the same failure was reported " << log_lines << " times";

  // Recovery from a handle that could not be created is held by
  // AClientWithoutAMultiHandleReachesATerminalOutcome. What this case is for is the state in
  // between, which nothing else reaches.
  client->FinishAllSessions();
}

}  // namespace
