#pragma once

#include "nlohmann/json.hpp"
#include "opentelemetry/ext//http/client/curl//http_client_curl.h"
#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/sdk/logs/exporter.h"

#include "opentelemetry/version.h"

#include <time.h>
#include <iostream>
#include <map>
#include <sstream>

namespace nostd   = opentelemetry::nostd;
namespace sdklogs = opentelemetry::sdk::logs;
using json        = nlohmann::json;

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
   * @param response_timeout The maximum time the exporter should wait after sending a request to
   * Elasticsearch
   * @param console_debug Print the status of the exporter methods in the console
   */
  ElasticsearchExporterOptions(std::string host     = "localhost",
                               int port             = 9200,
                               std::string index    = "logs/_doc?pretty",
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
  // Stores if this exporter had its Shutdown() method called
  bool isShutdown_ = false;

  // Configuration options for the exporter
  ElasticsearchExporterOptions options_;

  // Connection related variables
  opentelemetry::ext::http::client::curl::SessionManager session_manager_;
  std::shared_ptr<opentelemetry::ext::http::client::curl::Session> session_;

  /**
   * Converts a log record into a nlohmann::json object.
   */
  json RecordToJSON(std::unique_ptr<opentelemetry::logs::LogRecord> record);

  /**
   * Converts a common::AttributeValue into a string, which is used for parsing the attributes
   * and resource KeyValueIterables
   */
  const std::string ValueToString(const common::AttributeValue &value)
  {
    switch (value.index())
    {
      case common::AttributeType::TYPE_BOOL:
        return (opentelemetry::nostd::get<bool>(value) ? "true" : "false");
        break;
      case common::AttributeType::TYPE_INT:
        return std::to_string(opentelemetry::nostd::get<int>(value));
        break;
      case common::AttributeType::TYPE_INT64:
        return std::to_string(opentelemetry::nostd::get<int64_t>(value));
        break;
      case common::AttributeType::TYPE_UINT:
        return std::to_string(opentelemetry::nostd::get<unsigned int>(value));
        break;
      case common::AttributeType::TYPE_UINT64:
        return std::to_string(opentelemetry::nostd::get<uint64_t>(value));
        break;
      case common::AttributeType::TYPE_DOUBLE:
        return std::to_string(opentelemetry::nostd::get<double>(value));
        break;
      case common::AttributeType::TYPE_STRING:
      case common::AttributeType::TYPE_CSTRING:
        return opentelemetry::nostd::get<opentelemetry::nostd::string_view>(value).data();
        break;
      default:
        return "Invalid type";
        break;
    }
  }
};
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
