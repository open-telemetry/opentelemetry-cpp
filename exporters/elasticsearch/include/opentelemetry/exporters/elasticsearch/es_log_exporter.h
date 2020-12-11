#pragma once

#include "nlohmann/json.hpp"
#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/ext//http/client/curl//http_client_curl.h"

#include "opentelemetry/version.h"

#include <time.h>
#include <iostream>
#include <map>
#include <sstream>

namespace nostd   = opentelemetry::nostd;
namespace sdklogs = opentelemetry::sdk::logs;
using json = nlohmann::json;

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
   * 
   * @param host The host of the Elasticsearch instance
   * @param port The port of the Elasticsearch instance 
   * @param index The index/shard that the logs will be written to
   * @param response_timeout The maximum time the exporter should wait after sending a request to Elasticsearch
   * @param console_debug Print the status of the exporter methods in the console
   */
  ElasticsearchExporterOptions(std::string host = "localhost", int port = 9200, std::string index = "logs/_doc?pretty", int response_timeout = 30, bool console_debug = false) : 
      host_{host},
      port_{port},
      index_{index},
      response_timeout_{response_timeout},
      console_debug_{console_debug}
  {}
};

/**
 * The ElasticsearchLogExporter exports logs through an ostream to JSON format
 */
class ElasticsearchLogExporter final : public sdklogs::LogExporter
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
   * Exports a vector of log records to the Elasticsearch instance. Guaranteed to return after a  
   * timeout specified from the options passed from the constructor.
   * @param records A list of log records to send to Elasticsearch.
   */
  sdklogs::ExportResult Export(const nostd::span<std::unique_ptr<opentelemetry::logs::LogRecord>> 
                                      &records) noexcept override;

  /**
   * Shutdown this exporter. 
   * @param timeout The maximum time to wait for the shutdown method to return
   */
  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override;
private:
  bool isShutdown_ = false;
  
  // Configuration options for the exporter
  ElasticsearchExporterOptions options_;

  // Connection related variables
  opentelemetry::ext::http::client::curl::SessionManager session_manager_;
  std::shared_ptr<opentelemetry::ext::http::client::curl::Session> session_;
};
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
