// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/exporters/zipkin/zipkin_exporter_options.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/ext/http/common/url_parser.h"
#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include "nlohmann/json.hpp"

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
  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override;

  /**
   * Export a batch of span recordables in JSON format.
   * @param spans a span of unique pointers to span recordables
   */
  sdk::common::ExportResult Export(
      const nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> &spans) noexcept
      override;

  /**
   * Shut down the exporter.
   * @param timeout an optional timeout, default to max.
   */
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

private:
  void InitializeLocalEndpoint();

private:
  // The configuration options associated with this exporter.
  bool is_shutdown_ = false;
  ZipkinExporterOptions options_;
  std::shared_ptr<opentelemetry::ext::http::client::HttpClientSync> http_client_;
  opentelemetry::ext::http::common::UrlParser url_parser_;
  nlohmann::json local_end_point_;

  // For testing
  friend class ZipkinExporterTestPeer;
  /**
   * Create an ZipkinExporter using the specified thrift sender.
   * Only tests can call this constructor directly.
   * @param http_client the http client to be used for exporting
   */
  ZipkinExporter(std::shared_ptr<opentelemetry::ext::http::client::HttpClientSync> http_client);

  mutable opentelemetry::common::SpinLockMutex lock_;
  bool isShutdown() const noexcept;
};
}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
