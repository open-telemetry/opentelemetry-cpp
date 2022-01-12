// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include "nlohmann/json.hpp"
#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/ext/http/client/curl/http_client_curl.h"
#  include "opentelemetry/nostd/type_traits.h"
#  include "opentelemetry/sdk/logs/exporter.h"
#  include "opentelemetry/sdk/logs/log_record.h"

#  include <time.h>
#  include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{
/**
 * Struct to hold Elasticsearch exporter configuration options.
 */
struct ElasticsearchExporterOptions
{
  // Configuration options to establish Elasticsearch connection
  std::string host_;
  int port_;
  std::string index_;

  // Maximum time to wait for response after sending request to Elasticsearch
  int response_timeout_;

  // Whether to print the status of the exporter in the console
  bool console_debug_;

  /**
   * Constructor for the ElasticsearchExporterOptions. By default, the endpoint is
   * localhost:9200/logs with a timeout of 30 seconds and disabled console debugging
   * @param host The host of the Elasticsearch instance
   * @param port The port of the Elasticsearch instance
   * @param index The index/shard that the logs will be written to
   * @param response_timeout The maximum time in seconds the exporter should wait for a response
   * from elasticsearch
   * @param console_debug If true, print the status of the exporter methods in the console
   */
  ElasticsearchExporterOptions(std::string host     = "localhost",
                               int port             = 9200,
                               std::string index    = "logs",
                               int response_timeout = 30,
                               bool console_debug   = false)
      : host_{host},
        port_{port},
        index_{index},
        response_timeout_{response_timeout},
        console_debug_{console_debug}
  {}
};

/**
 * The ElasticsearchLogExporter exports logs to Elasticsearch in JSON format
 */
class ElasticsearchLogExporter final : public opentelemetry::sdk::logs::LogExporter
{
public:
  /**
   * Create an ElasticsearchLogExporter with default exporter options.
   */
  ElasticsearchLogExporter();

  /**
   * Create an ElasticsearchLogExporter with user specified options.
   * @param options An object containing the user's configuration options.
   */
  ElasticsearchLogExporter(const ElasticsearchExporterOptions &options);

  /**
   * Creates a recordable that stores the data in a JSON object
   */
  std::unique_ptr<opentelemetry::sdk::logs::Recordable> MakeRecordable() noexcept override;

  /**
   * Exports a vector of log records to the Elasticsearch instance. Guaranteed to return after a
   * timeout specified from the options passed from the constructor.
   * @param records A list of log records to send to Elasticsearch.
   */
  sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>>
          &records) noexcept override;

  /**
   * Shutdown this exporter.
   * @param timeout The maximum time to wait for the shutdown method to return
   */
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

private:
  // Stores if this exporter had its Shutdown() method called
  bool is_shutdown_ = false;

  // Configuration options for the exporter
  ElasticsearchExporterOptions options_;

  // Object that stores the HTTP sessions that have been created
  std::unique_ptr<ext::http::client::HttpClient> http_client_;
  mutable opentelemetry::common::SpinLockMutex lock_;
  bool isShutdown() const noexcept;
};
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
