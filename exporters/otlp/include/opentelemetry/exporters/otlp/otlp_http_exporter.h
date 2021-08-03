// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

// We need include exporter.h first, which will include Windows.h with NOMINMAX on Windows
#include "opentelemetry/sdk/trace/exporter.h"

#include "opentelemetry/ext/http/client/http_client.h"

#include <chrono>
#include <memory>
#include <mutex>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
// The default URL path to post trace data.
constexpr char kDefaultTracePath[] = "/v1/traces";
// The default URL path to post metric data.
constexpr char kDefaultMetricsPath[] = "/v1/metrics";
// The default URL path to post metric data.
constexpr char kDefaultLogPath[] = "/v1/logs";
// The HTTP header "Content-Type"
constexpr char kHttpJsonContentType[]   = "application/json";
constexpr char kHttpBinaryContentType[] = "application/x-protobuf";

enum class JsonBytesMappingKind
{
  kHexId,
  kHex,
  kBase64,
};

enum class HttpRequestContentType
{
  kJson,
  kBinary,
};

/**
 * Struct to hold OTLP exporter options.
 */
struct OtlpHttpExporterOptions
{
  // The endpoint to export to. By default
  // @see
  // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/protocol/otlp.md
  // @see https://github.com/open-telemetry/opentelemetry-collector/tree/main/receiver/otlpreceiver
  std::string url = std::string("http://localhost:4317") + kDefaultTracePath;

  // By default, post json data
  HttpRequestContentType content_type = HttpRequestContentType::kJson;

  // If convert bytes into hex. By default, we will convert all bytes but id into base64
  // This option is ignored if content_type is not kJson
  JsonBytesMappingKind json_bytes_mapping = JsonBytesMappingKind::kHexId;

  // If using the json name of protobuf field to set the key of json. By default, we will use the
  // field name just like proto files.
  bool use_json_name = false;

  // Whether to print the status of the exporter in the console
  bool console_debug = false;

  // TODO: Enable/disable to verify SSL certificate
  std::chrono::milliseconds timeout = std::chrono::milliseconds(30000);

  // filepath of the sll certifcate, this is empty if there is none. 
  std::string sslCertPath;
};

/**
 * The OTLP exporter exports span data in OpenTelemetry Protocol (OTLP) format.
 */
class OtlpHttpExporter final : public opentelemetry::sdk::trace::SpanExporter
{
public:
  /**
   * Create an OtlpHttpExporter using all default options.
   */
  OtlpHttpExporter();

  /**
   * Create an OtlpHttpExporter using the given options.
   */
  explicit OtlpHttpExporter(const OtlpHttpExporterOptions &options);

  /**
   * Create a span recordable.
   * @return a newly initialized Recordable object
   */
  std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override;

  /**
   * Export
   * @param spans a span of unique pointers to span recordables
   */
  sdk::common::ExportResult Export(
      const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept override;

  /**
   * Shut down the exporter.
   * @param timeout an optional timeout, the default timeout of 0 means that no
   * timeout is applied.
   * @return return the status of this operation
   */
  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override;

private:
  // Stores if this exporter had its Shutdown() method called
  bool is_shutdown_ = false;

  // For testing
  friend class OtlpHttpExporterTestPeer;

  // The configuration options associated with this exporter.
  const OtlpHttpExporterOptions options_;

  // Object that stores the HTTP sessions that have been created
  std::shared_ptr<ext::http::client::HttpClient> http_client_;
  // Cached parsed URI
  std::string http_uri_;
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
