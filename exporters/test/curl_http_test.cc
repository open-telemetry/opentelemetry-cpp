#include "opentelemetry/exporters/common/http/http_client_curl.h"
#include "opentelemetry/ext/http/server/http_server.h"

#include <fstream>
#include <atomic>
#include <assert.h>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <vector>

#define HTTP_PORT       19000

#include <gtest/gtest.h>

namespace curl = opentelemetry::exporters::common::http::curl;
namespace http_sdk = opentelemetry::sdk::common::http;

class CustomEventHandler: public http_sdk::EventHandler
{
public:
  virtual void OnResponse(http_sdk::Response &response) noexcept override {};
  virtual void OnEvent(http_sdk::SessionState state, opentelemetry::nostd::string_view reason) noexcept override {}
  virtual void OnConnecting(const http_sdk::SSLCertificate &) noexcept {}
   virtual ~CustomEventHandler()  = default;
   bool is_called_ = false;
};

class GetEventHandler: public CustomEventHandler
{
    void OnResponse(http_sdk::Response &response) noexcept override
    {
        ASSERT_EQ(200, response.GetStatusCode());
        ASSERT_EQ(response.GetBody().size(), 0);
        is_called_ = true;
    };
};

class PostEventHandler: public CustomEventHandler
{
    void OnResponse(http_sdk::Response &response) noexcept override
    {
        ASSERT_EQ(200, response.GetStatusCode());
        std::string body(response.GetBody().begin(),response.GetBody().end());
        ASSERT_EQ(body, "{'k1':'v1', 'k2':'v2', 'k3':'v3'}");
        is_called_ = true;
    }
};

class BasicCurlHttpTests : public ::testing::Test,
    public HTTP_SERVER_NS::HttpRequestCallback
{
    protected:
    HTTP_SERVER_NS::HttpServer server_;
    std::string server_address_;
    std::atomic<bool> is_setup_;
    std::atomic<bool> is_running_;
    std::vector<HTTP_SERVER_NS::HttpRequest> received_requests_;
    std::mutex                       mtx_requests;
    std::condition_variable cv_got_events;
    std::mutex cv_m;


    public:
        BasicCurlHttpTests() :
        is_setup_(false) ,
        is_running_(false)
    {};

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
        is_running_  = true;
    }

    virtual void TearDown() override
    {
        if (!is_setup_.exchange(false))
            return;
        server_.stop();
        is_running_ = false;        
    }

    virtual int onHttpRequest(HTTP_SERVER_NS::HttpRequest const &request, HTTP_SERVER_NS::HttpResponse &response) override
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
                response.body = "{'k1':'v1', 'k2':'v2', 'k3':'v3'}";
                return 200;
            }
            return 404;

    }

    bool waitForRequests(unsigned timeOutSec, unsigned expected_count = 1)
    {
        std::unique_lock<std::mutex> lk(cv_m);
        if (cv_got_events.wait_for(lk, std::chrono::milliseconds(1000 * timeOutSec), [&] { return received_requests_.size() >= expected_count; }))
        {
            return true;
        }
        return false;
    }
};

TEST_F(BasicCurlHttpTests, DoNothing)
{
}

TEST_F(BasicCurlHttpTests, SendGetRequest)
{
    received_requests_.clear();
    curl::SessionManager session_manager;

    auto session = session_manager.CreateSession("127.0.0.1", HTTP_PORT);
    auto request = session->CreateRequest();
    request->SetUri("get/");
    GetEventHandler *handler = new GetEventHandler();
    session->SendRequest(*handler);
    ASSERT_TRUE(waitForRequests(1,1));  
    session->FinishSession();
    ASSERT_TRUE(handler->is_called_);
    delete handler;
}

TEST_F(BasicCurlHttpTests, SendPostRequest)
{
    received_requests_.clear();
    curl::SessionManager session_manager;

    auto session = session_manager.CreateSession("127.0.0.1", HTTP_PORT);
    auto request = session->CreateRequest();
    request->SetUri("post/");
    request->SetMethod(http_sdk::Method::Post);
    PostEventHandler *handler = new PostEventHandler();
    session->SendRequest(*handler);
    ASSERT_TRUE(waitForRequests(1,1));
    session->FinishSession();
    ASSERT_TRUE(handler->is_called_);
    delete handler;
}



