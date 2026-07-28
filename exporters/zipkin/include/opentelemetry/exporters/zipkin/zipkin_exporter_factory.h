// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/exporters/zipkin/zipkin_exporter_options.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

/**
 * Factory class for ZipkinExporter.
 */
class ZipkinExporterFactory
{
public:
  /**
   * Create a ZipkinExporter using all default options.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create();

  /**
   * Create a ZipkinExporter using the given options.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create(
      const ZipkinExporterOptions &options);

  /**
   * Create a ZipkinExporter using the given options and HTTP client factory.
   * @param options the exporter options
   * @param factory the HTTP client factory used to create the underlying HTTP client
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create(
      const ZipkinExporterOptions &options,
      const std::shared_ptr<opentelemetry::ext::http::client::HttpClientFactory> &factory);

  /**
   * Create a ZipkinExporter using the given options and HTTP client.
   * @param options the exporter options
   * @param http_client the HTTP client to be used for exporting
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create(
      const ZipkinExporterOptions &options,
      std::shared_ptr<opentelemetry::ext::http::client::HttpClientSync> http_client);
};

}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
