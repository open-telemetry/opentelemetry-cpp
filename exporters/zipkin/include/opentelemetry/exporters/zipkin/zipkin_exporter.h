#pragma once

#include "opentelemetry/exporters/zipkin/recordable.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/ext/http/common/url_parser.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include "nlohmann/json.hpp"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

std::string ZIPKIN_ENDPOINT_DEFAULT = "http://localhost:9411/api/v2/spans";

/**
 * Struct to hold Zipkin  exporter options.
 */
struct ZipkinExporterOptions
{
  // The endpoint to export to. By default the OpenTelemetry Collector's default endpoint.
  std::string endpoint     = ZIPKIN_ENDPOINT_DEFAULT;
  TransportFormat format   = TransportFormat::JSON;
  std::string service_name = "default-service";
  std::string ipv4;
  std::string ipv6;
};

namespace trace_sdk   = opentelemetry::sdk::trace;
namespace http_client = opentelemetry::ext::http::client;

/**
 * The Zipkin exporter exports span data in JSON format as expected by Zipkin
 */
class ZipkinExporter final : public trace_sdk::SpanExporter, public http_client::EventHandler
{
public:
  /**
   * Create an ZipkinExporter using all default options.
   */
  ZipkinExporter();

  /**
   * Create an ZipkinExporter using the given options.
   */
  ZipkinExporter(const ZipkinExporterOptions &options);

  /**
   * Create a span recordable.
   * @return a newly initialized Recordable object
   */
  std::unique_ptr<trace_sdk::Recordable> MakeRecordable() noexcept override;

  /**
   * Export a batch of span recordables in JSON format.
   * @param spans a span of unique pointers to span recordables
   */
  trace_sdk::ExportResult Export(
      const nostd::span<std::unique_ptr<trace_sdk::Recordable>> &spans) noexcept override;

  /**
   * Shut down the exporter.
   * @param timeout an optional timeout, default to max.
   */
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override
  {
    return true;
  };

  void OnResponse(http_client::Response &response) noexcept override
  { /*Not required */
  }

  virtual void OnEvent(http_client::SessionState state,
                       nostd::string_view msg) noexcept override{/* Not required */};

  virtual void OnConnecting(const http_client::SSLCertificate &) noexcept override{
      /* Not required */};

private:
  void InitializeLocalEndpoint();

private:
  // The configuration options associated with this exporter.
  bool isShutdown_ = false;
  std::shared_ptr<http_client::SessionManager> http_session_manager_;
  nlohmann::json local_end_point_;
  ZipkinExporterOptions options_;
  ext::http::common::UrlParser url_parser_;
};
}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
