#include "http_client_curl.h"
#include "opentelemetry/nostd/string_view.h"
#include <iostream>

namespace curl = opentelemetry::exporters::common::http::curl;
namespace http_sdk = opentelemetry::sdk::common::http;

class CustomEventHandler: public http_sdk::EventHandler
{
public:
  virtual void OnResponse(http_sdk::Response &response) noexcept override 
  {
      std::cout << "LALIT -> In main : " << response.GetStatusCode() << "\n";
  }

  virtual void OnEvent(http_sdk::SessionState state, opentelemetry::nostd::string_view reason) noexcept override 
  {
    std::cout  << reason ;
  }

  virtual void OnConnecting(const http_sdk::SSLCertificate &) noexcept {}

   virtual ~CustomEventHandler()  = default;
};

int main()
{
    curl::SessionManager sessionManager;

    auto session = sessionManager.CreateSession("127.0.0.1", 8080);
    auto request = session->CreateRequest(); 
    CustomEventHandler *handler = new CustomEventHandler();
    session->SendRequest(*handler);
    session->FinishSession();
    std::cout << " =====> LALIT :::===> deleting handler\n";
    delete handler;
}

