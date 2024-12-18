// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/ext//http/client/curl/http_client_curl.h"
#include "opentelemetry/ext/http/client/curl/http_operation_curl.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/ext/http/server/http_server.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/string_view.h"

#define HTTP_PORT 19000

namespace curl        = opentelemetry::ext::http::client::curl;
namespace http_client = opentelemetry::ext::http::client;
namespace nostd       = opentelemetry::nostd;

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

  ~CustomEventHandler() override = default;

  std::atomic<bool> is_called_;
  std::atomic<bool> got_response_;
};

class GetEventHandler : public CustomEventHandler
{
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

class BasicCurlHttpTests : public ::testing::Test, public HTTP_SERVER_NS::HttpRequestCallback
{
protected:
  HTTP_SERVER_NS::HttpServer server_;
  std::string server_address_;
  std::atomic<bool> is_setup_{false};
  std::atomic<bool> is_running_{false};
  std::vector<HTTP_SERVER_NS::HttpRequest> received_requests_;
  std::mutex cv_mtx_requests;
  std::mutex mtx_requests;
  std::condition_variable cv_got_events;
  std::mutex cv_m;

public:
  BasicCurlHttpTests() : is_setup_(false), is_running_(false) {}

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

TEST_F(BasicCurlHttpTests, DoNothing) {}

TEST_F(BasicCurlHttpTests, HttpRequest)
{
  curl::Request req;
  const char *b           = "test-data";
  http_client::Body body  = {b, b + strlen(b)};
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
  http_client::Body body = {b, b + strlen(b)};
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
  auto session_manager = http_client::HttpClientFactory::Create();
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
  auto session_manager = http_client::HttpClientFactory::Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session = session_manager->CreateSession("http://127.0.0.1:19000");
  auto request = session->CreateRequest();
  request->SetUri("post/");
  request->SetMethod(http_client::Method::Post);

  const char *b          = "test-data";
  http_client::Body body = {b, b + strlen(b)};
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
  auto session_manager = http_client::HttpClientFactory::Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session = session_manager->CreateSession("222.222.222.200:19000");  // Non Existing address
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
  http_client::Body body = {b, b + strlen(b)};

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

TEST_F(BasicCurlHttpTests, SendGetRequestSync)
{
  received_requests_.clear();
  curl::HttpClientSync http_client;

  http_client::Headers m1 = {};
  auto result             = http_client.GetNoSsl("http://127.0.0.1:19000/get/", m1);
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

TEST_F(BasicCurlHttpTests, SendGetRequestAsync)
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

#ifdef ENABLE_OTLP_COMPRESSION_PREVIEW
struct GzipEventHandler : public CustomEventHandler
{
  ~GzipEventHandler() override = default;

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
  auto session_manager = http_client::HttpClientFactory::Create();
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
  auto session_manager = http_client::HttpClientFactory::Create();
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
