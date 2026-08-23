// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>
#include <chrono>
#include <memory>

#include "nlohmann/json.hpp"

#include "opentelemetry/exporters/zipkin/zipkin_exporter_options.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/ext/http/common/url_parser.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

/**
 * The Zipkin exporter exports span data in JSON format as expected by Zipkin
 */
class ZipkinExporter final : public opentelemetry::sdk::trace::SpanExporter
{
public:
  /**
   * Create a ZipkinExporter using all default options.
   * Uses the default HTTP client factory (curl when available).
   */
  ZipkinExporter();

  /**
   * Create a ZipkinExporter using the given options.
   * Uses the default HTTP client factory (curl when available).
   */
  explicit ZipkinExporter(const ZipkinExporterOptions &options);

  /**
   * Create a ZipkinExporter using the given options and HTTP client factory.
   * @param options the exporter options
   * @param factory the HTTP client factory used to create the underlying HTTP client
   */
  ZipkinExporter(
      const ZipkinExporterOptions &options,
      const std::shared_ptr<opentelemetry::ext::http::client::HttpClientFactory> &factory);

  /**
   * Create a ZipkinExporter using the given options and HTTP client.
   * @param options the exporter options
   * @param http_client the HTTP client to be used for exporting
   */
  ZipkinExporter(const ZipkinExporterOptions &options,
                 std::shared_ptr<opentelemetry::ext::http::client::HttpClientSync> http_client);

  /**
   * Create a span recordable.
   * @return a newly initialized Recordable object
   */
  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override;

  /**
   * Export a batch of span recordables in JSON format.
   * @param spans a span of unique pointers to span recordables
   */
  sdk::common::ExportResult Export(
      const nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> &spans) noexcept
      override;

  /**
   * Force flush the exporter.
   * @param timeout an option timeout, default to max.
   * @return return true when all data are exported, and false when timeout
   */
  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

  /**
   * Shut down the exporter.
   * @param timeout an optional timeout, default to max.
   */
  bool Shutdown(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

private:
  void InitializeLocalEndpoint();

private:
  // The configuration options associated with this exporter.
  std::atomic<bool> is_shutdown_{false};
  ZipkinExporterOptions options_;
  std::shared_ptr<opentelemetry::ext::http::client::HttpClientSync> http_client_;
  opentelemetry::ext::http::common::UrlParser url_parser_;
  nlohmann::json local_end_point_;

  // For testing
  friend class ZipkinExporterTestPeer;
  /**
   * Create an ZipkinExporter using the specified http client.
   * Only tests can call this constructor directly.
   * @param http_client the http client to be used for exporting
   */
  ZipkinExporter(std::shared_ptr<opentelemetry::ext::http::client::HttpClientSync> http_client);

  bool isShutdown() const noexcept;
};
}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
