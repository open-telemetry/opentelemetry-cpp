#pragma once

#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/version.h"

#include <iostream>
#include <map>
#include <sstream>

namespace nostd   = opentelemetry::nostd;
namespace sdklogs = opentelemetry::sdk::logs;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{
/**
 * The OStreamLogExporter exports logs through an ostream (default set to std::cout)
 */
class OStreamLogExporter final : public sdklogs::LogExporter
{
public:
  /**
   * Create an OStreamLogExporter. This constructor takes in a reference to an ostream that the
   * Export() method will send log data into. The default ostream is set to stdout.
   */
  explicit OStreamLogExporter(std::ostream &sout = std::cout) noexcept;

  /*********************** Overloads of LogExporter interface *********************/

  /**
   * Exports a span of logs sent from the processor.
   */
  sdklogs::ExportResult Export(const nostd::span<std::shared_ptr<opentelemetry::logs::LogRecord>>
                                   &records) noexcept override;

  /**
   * Marks the OStream Log Exporter as shut down.
   */
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

private:
  // The OStream to send the logs to
  std::ostream &sout_;
  // Whether this exporter is ShutDown
  bool isShutdown_ = false;

  /**
   * Internal map of the severity number (from 0 to 24) to severity text, matching the
   * values set by the default Severity enum in api/include/opentelemetry/logs/log_record.h
   *
   * If more than one exporter requires this, could move this to Severity enum.
   */
  const nostd::string_view severityNumToText[25] = {
      "kInvalid", "kTrace",  "kTrace2", "kTrace3", "kTrace4", "kDebug",  "kDebug2",
      "kDebug3",  "kDebug4", "kInfo",   "kInfo2",  "kInfo3",  "kInfo4",  "kWarn",
      "kWarn2",   "kWarn3",  "kWarn4",  "kError",  "kError2", "kError3", "kError4",
      "kFatal",   "kFatal2", "kFatal3", "kFatal4"};

  /**
   * Helper function to print a KeyValueIterable. Outputs a AttributeValue type.
   *
   * Based off api/include/opentelemetry/common/attribute_value.h
   * In the future, may be better to add operator overloads to attribute_value.h
   * to make more maintainable.
   *
   */
  void print_value(const common::AttributeValue &value)
  {
    switch (value.index())
    {
      case common::AttributeType::TYPE_BOOL:
        sout_ << (nostd::get<bool>(value) ? "true" : "false");
        break;
      case common::AttributeType::TYPE_INT:
        sout_ << nostd::get<int>(value);
        break;
      case common::AttributeType::TYPE_INT64:
        sout_ << nostd::get<int64_t>(value);
        break;
      case common::AttributeType::TYPE_UINT:
        sout_ << nostd::get<unsigned int>(value);
        break;
      case common::AttributeType::TYPE_UINT64:
        sout_ << nostd::get<uint64_t>(value);
        break;
      case common::AttributeType::TYPE_DOUBLE:
        sout_ << nostd::get<double>(value);
        break;
      case common::AttributeType::TYPE_STRING:
      case common::AttributeType::TYPE_CSTRING:
        sout_ << nostd::get<nostd::string_view>(value);
        break;

        /*** Need to support these? ***/
        // case common::AttributeType::TYPE_SPAN_BOOL:
        //   sout_ << nostd::get<nostd::span<const bool>>(value);
        //   break;
        // case common::AttributeType::TYPE_SPAN_INT:
        //   sout_ << nostd::get<nostd::span<const int>>(value);
        //   break;
        // case common::AttributeType::TYPE_SPAN_INT64:
        //   sout_ << nostd::get<nostd::span<const int64_t>>(value);
        //   break;
        // case common::AttributeType::TYPE_SPAN_UINT:
        //   sout_ << nostd::get<nostd::span<const unsigned int>>(value);
        //   break;
        // case common::AttributeType::TYPE_SPAN_UINT64:
        //   sout_ << nostd::get<nostd::span<const uint64_t>>(value);
        //   break;
        // case common::AttributeType::TYPE_SPAN_DOUBLE:
        //   sout_ << nostd::get<nostd::span<const double>>(value);
        //   break;
        // case common::AttributeType::TYPE_SPAN_STRING:
        //   sout_ << nostd::get<nostd::span<const nostd::string_view>>(value);
        //   break;
        /******** Up to here ************/

      default:
        sout_ << "Invalid type";
        break;
    }
  }

  /**
   * Helper function to print a KeyValueIterable.
   * Outputs a <Key, Value> pair of type <string_view, AttributeValue>
   */
  void printKV(bool &firstKV, const nostd::string_view &key, const common::AttributeValue &value)
  {
    if (firstKV)
    {
      firstKV = false;
    }
    else
    {
      sout_ << ", ";
    }

    sout_ << "{" << key << ": ";
    print_value(value);
    sout_ << "}";
  }
};
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
