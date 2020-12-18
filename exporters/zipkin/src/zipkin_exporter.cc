#include "opentelemetry/exporters/zipkin/zipkin_exporter.h"
#include "opentelemetry/exporters/zipkin/recordable.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/ext/http/common/url_parser.h"
#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

// -------------------------------- Contructors --------------------------------

ZipkinExporter::ZipkinExporter(const ZipkinExporterOptions &options)
    : options_(options), url_parser_(options_.endpoint)
{
  http_session_manager_ = ext::http::client::HttpClientFactory::Create();
  InitializeLocalEndpoint();
  
}

ZipkinExporter::ZipkinExporter()
    : options_(ZipkinExporterOptions()),
      url_parser_(options_.endpoint)
{  
  http_session_manager_ = ext::http::client::HttpClientFactory::Create();
  std::cout<< "URL: "<< options_.endpoint << " " << url_parser_.host_ << " " << url_parser_.port_ << "\n";
  InitializeLocalEndpoint();
}

// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<sdk::trace::Recordable> ZipkinExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdk::trace::Recordable>(new Recordable);
}

sdk::trace::ExportResult ZipkinExporter::Export(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept
{
  if (isShutdown_)
  {
    return sdk::trace::ExportResult::kFailure;
  }
  auto session = http_session_manager_->CreateSession(url_parser_.host_, url_parser_.port_);
  auto request = session->CreateRequest();
  request->SetUri(url_parser_.path_);
  exporter::zipkin::ZipkinSpan json_spans = {};
  for (auto &recordable : spans)
  {
    auto rec = std::unique_ptr<Recordable>(static_cast<Recordable *>(recordable.release()));
    if (rec != nullptr)
    {
      auto json_span = rec->span();
      // add localEndPoint
      json_span["localEndpoint"]  = local_end_point_;
      json_spans.push_back(json_span);
    }
  }
  auto body_s = json_spans.dump();
  std::cout << "FINAL JSON:" << body_s << "\n";
  http_client::Body body_v(body_s.begin(), body_s.end());
  request->SetMethod(opentelemetry::ext::http::client::Method::Post);
  request->SetBody(body_v);
  request->AddHeader("Content-Type", "application/json");
  http_client::SessionState session_state;
  auto response = session->SendRequestSync(session_state);
  if (response){
    
    std::cout << "Response Code." << response->GetStatusCode() << "\n";
    auto body = response->GetBody();
    std::cout << "\nBody:" ;
    for (auto &c : body)
    {
      std::cout << static_cast<char>(c) ;
    }
    std::cout << "\n";
  }
  if (response && ( response->GetStatusCode() == 200  || response->GetStatusCode() == 202))
  {
    std::cout << " SUCCESS\n";
    return sdk::trace::ExportResult::kSuccess;
  }
  else {
    std::cout << " FAILURE\n";
    if (session_state == http_client::SessionState::ConnectFailed) { std::cout << "Connect failed\n";}
    //TBD -> Handle error / retries
    return sdk::trace::ExportResult::kFailure;
  }
}
 
void ZipkinExporter::InitializeLocalEndpoint()
{
    if (options_.service_name.length()) {
      local_end_point_["serviceName"] = options_.service_name;
    }
    if (options_.ipv4.length()) {
      local_end_point_["ipv4"] = options_.ipv4;
    }
    if (options_.ipv6.length()) {
      local_end_point_["ipv6"] = options_.ipv6;
    }
    local_end_point_["port"] = url_parser_.port_;
}

}
}
OPENTELEMETRY_END_NAMESPACE