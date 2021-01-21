#include "opentelemetry/ext//http/client/curl//http_client_curl.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/ext/http/server/http_server.h"

#include <assert.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <memory>
#include <thread>
#include <vector>

#define HTTP_PORT 19000

#include <gtest/gtest.h>

namespace curl        = opentelemetry::ext::http::client::curl;
namespace http_client = opentelemetry::ext::http::client;

class CustomEventHandler : public http_client::EventHandler
{
public:
  virtual void OnResponse(http_client::Response &response) noexcept override{};
  virtual void OnEvent(http_client::SessionState state,
                       opentelemetry::nostd::string_view reason) noexcept override
  {}
  virtual void OnConnecting(const http_client::SSLCertificate &) noexcept {}
  virtual ~CustomEventHandler() = default;
  bool is_called_               = false;
};

class GetEventHandler : public CustomEventHandler
{
  void OnResponse(http_client::Response &response) noexcept override
  {
    ASSERT_EQ(200, response.GetStatusCode());
    ASSERT_EQ(response.GetBody().size(), 0);
    is_called_ = true;
  };
};

class PostEventHandler : public CustomEventHandler
{
  void OnResponse(http_client::Response &response) noexcept override
  {
    ASSERT_EQ(200, response.GetStatusCode());
    std::string body(response.GetBody().begin(), response.GetBody().end());
    ASSERT_EQ(body, "{'k1':'v1', 'k2':'v2', 'k3':'v3'}");
    is_called_ = true;
  }
};

class BasicCurlHttpTests : public ::testing::Test, public HTTP_SERVER_NS::HttpRequestCallback
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
  BasicCurlHttpTests() : is_setup_(false), is_running_(false){};

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
    if (request.uri == "/get/")
    {

      std::unique_lock<std::mutex> lk(mtx_requests);
      received_requests_.push_back(request);
      response.headers["Content-Type"] = "text/plain";
      return 200;
    }
    if (request.uri == "/post/")
    {
      std::unique_lock<std::mutex> lk(mtx_requests);
      received_requests_.push_back(request);
      response.headers["Content-Type"] = "application/json";
      response.body                    = "{'k1':'v1', 'k2':'v2', 'k3':'v3'}";
      return 200;
    }
    return 404;
  }

  bool waitForRequests(unsigned timeOutSec, unsigned expected_count = 1)
  {
    std::unique_lock<std::mutex> lk(mtx_requests);
    if (cv_got_events.wait_for(lk, std::chrono::milliseconds(1000 * timeOutSec),
                               [&] { return received_requests_.size() >= expected_count; }))
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
  std::multimap<std::string, std::string, curl::curl_ci> m1 = {
      {"name1", "value1_1"}, {"name1", "value1_2"}, {"name2", "value3"}, {"name3", "value3"}};
  res.headers_ = m1;

  const char *b          = "test-data";
  http_client::Body body = {b, b + strlen(b)};
  int count              = 0;
  res.ForEachHeader("name1", [&count](opentelemetry::nostd::string_view name,
                                      opentelemetry::nostd::string_view value) {
    if (name != "name1")
      return false;
    if (value != "value1_1" && value != "value1_2")
      return false;
    count++;
    return true;
  });
  ASSERT_EQ(count, 2);
  count = 0;
  res.ForEachHeader(
      [&count](opentelemetry::nostd::string_view name, opentelemetry::nostd::string_view value) {
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

  auto session = session_manager->CreateSession("127.0.0.1", HTTP_PORT);
  auto request = session->CreateRequest();
  request->SetUri("get/");
  GetEventHandler *handler = new GetEventHandler();
  session->SendRequest(*handler);
  ASSERT_TRUE(waitForRequests(1, 1));
  session->FinishSession();
  ASSERT_TRUE(handler->is_called_);
  delete handler;
}

TEST_F(BasicCurlHttpTests, SendPostRequest)
{
  received_requests_.clear();
  auto session_manager = http_client::HttpClientFactory::Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session = session_manager->CreateSession("127.0.0.1", HTTP_PORT);
  auto request = session->CreateRequest();
  request->SetUri("post/");
  request->SetMethod(http_client::Method::Post);

  const char *b          = "test-data";
  http_client::Body body = {b, b + strlen(b)};
  request->SetBody(body);
  request->AddHeader("Content-Type", "text/plain");
  PostEventHandler *handler = new PostEventHandler();
  session->SendRequest(*handler);
  ASSERT_TRUE(waitForRequests(1, 1));
  session->FinishSession();
  ASSERT_TRUE(handler->is_called_);

  session_manager->CancelAllSessions();
  session_manager->FinishAllSessions();

  delete handler;
}

TEST_F(BasicCurlHttpTests, RequestTimeout)
{
  received_requests_.clear();
  auto session_manager = http_client::HttpClientFactory::Create();
  EXPECT_TRUE(session_manager != nullptr);

  auto session =
      session_manager->CreateSession("222.222.222.200", HTTP_PORT);  // Non Existing address
  auto request = session->CreateRequest();
  request->SetUri("get/");
  GetEventHandler *handler = new GetEventHandler();
  session->SendRequest(*handler);
  session->FinishSession();
  ASSERT_FALSE(handler->is_called_);
  delete handler;
}

TEST_F(BasicCurlHttpTests, CurlHttpOperations)
{
  GetEventHandler *handler = new GetEventHandler();

  const char *b          = "test-data";
  http_client::Body body = {b, b + strlen(b)};

  std::multimap<std::string, std::string, curl::curl_ci> m1 = {
      {"name1", "value1_1"}, {"name1", "value1_2"}, {"name2", "value3"}, {"name3", "value3"}};
  curl::Headers headers = m1;
  curl::HttpOperation http_operations1(http_client::Method::Head, "/get", handler, headers, body,
                                       true);
  http_operations1.Send();

  curl::HttpOperation http_operations2(http_client::Method::Get, "/get", handler, headers, body,
                                       true);
  http_operations2.Send();

  curl::HttpOperation http_operations3(http_client::Method::Get, "/get", handler, headers, body,
                                       false);
  http_operations3.Send();
  delete handler;
}

TEST_F(BasicCurlHttpTests, GetBaseUri)
{
  curl::SessionManager session_manager;

  auto session = session_manager.CreateSession("127.0.0.1", 80);
  ASSERT_EQ(std::static_pointer_cast<curl::Session>(session)->GetBaseUri(), "http://127.0.0.1:80/");

  session = session_manager.CreateSession("https://127.0.0.1", 443);
  ASSERT_EQ(std::static_pointer_cast<curl::Session>(session)->GetBaseUri(),
            "https://127.0.0.1:443/");

  session = session_manager.CreateSession("http://127.0.0.1", 31339);
  ASSERT_EQ(std::static_pointer_cast<curl::Session>(session)->GetBaseUri(),
            "http://127.0.0.1:31339/");
}
