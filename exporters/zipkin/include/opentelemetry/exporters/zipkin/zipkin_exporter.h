/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

const char *kZipkinEndpointDefault = "http://localhost:9411/api/v2/spans";

static const std::string GetDefaultZipkinEndpoint()
{
  auto endpoint_from_env = std::getenv("OTEL_EXPORTER_ZIPKIN_ENDPOINT");
  return std::string{endpoint_from_env ? endpoint_from_env : kZipkinEndpointDefault};
}

/**
 * Struct to hold Zipkin  exporter options.
 */
struct ZipkinExporterOptions
{
  // The endpoint to export to. By default the OpenTelemetry Collector's default endpoint.
  std::string endpoint     = GetDefaultZipkinEndpoint();
  TransportFormat format   = TransportFormat::kJson;
  std::string service_name = "default-service";
  std::string ipv4;
  std::string ipv6;
};

namespace trace_sdk   = opentelemetry::sdk::trace;
namespace http_client = opentelemetry::ext::http::client;

/**
 * The Zipkin exporter exports span data in JSON format as expected by Zipkin
 */
class ZipkinExporter final : public trace_sdk::SpanExporter
{
public:
  /**
   * Create a ZipkinExporter using all default options.
   */
  ZipkinExporter();

  /**
   * Create a ZipkinExporter using the given options.
   */
  explicit ZipkinExporter(const ZipkinExporterOptions &options);

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
  }

private:
  void InitializeLocalEndpoint();

private:
  // The configuration options associated with this exporter.
  bool isShutdown_ = false;
  ZipkinExporterOptions options_;
  std::shared_ptr<http_client::HttpClientSync> http_client_;
  opentelemetry::ext::http::common::UrlParser url_parser_;
  nlohmann::json local_end_point_;
};
}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
