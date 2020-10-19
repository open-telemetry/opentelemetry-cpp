#pragma once

#include <map>
#include <unordered_map>
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/logs/logger.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace logs
{

/* Note: using a class enum here won't allow enum values to be compared to integers, i.e. only other Severity enums (need an explicit cast)
*/
enum class Severity
{
  NONE   = 0,       // default Severity; added, not part of Log Data Model
  TRACE  = 1,
  TRACE2 = 2,
  TRACE3 = 3,
  TRACE4 = 4,
  DEBUG  = 5,
  DEBUG2 = 6,
  DEBUG3 = 7,
  DEBUG4 = 8,
  INFO   = 9,
  INFO2  = 10,
  INFO3  = 11,
  INFO4  = 12,
  WARN   = 13,
  WARN2  = 14,
  WARN3  = 15,
  WARN4  = 16,
  ERROR  = 17,
  ERROR2 = 18,
  ERROR3 = 19,
  ERROR4 = 20,
  FATAL  = 21,
  FATAL2 = 22,
  FATAL3 = 23,
  FATAL4 = 24
};

/**
 * A default Event object to be passed in log statements,
 * matching the 10 fields of the Log Data Model.
 * (https://github.com/open-telemetry/opentelemetry-specification/blob/master/specification/logs/data-model.md#log-and-event-record-definition)
 *
 */
struct LogRecord
{
  // default fields that will be set if the user doesn't specify them
  uint64_t timestamp;                   // uint64 nanoseconds since Unix epoch
  trace::TraceId trace_id;              // byte sequence
  trace::SpanId span_id;                // byte sequence
  trace::TraceFlags trace_flag;         // byte
  Severity severity;                    // Severity enum that combines severity_text and severity_number in the LogDataModel (can separate in SDK)

  // other fields
  nostd::string_view name;               // string
  nostd::string_view body;               // currently a simple string, but should be changed "Any" type
  common::KeyValueIterable &resource;    // key/value pair list
  common::KeyValueIterable &attributes;  // key/value pair list

 /* Default log record if user does not overwrite this.
  *  TODO: Potentially add other constructors to take default arguments from the user
  *  TODO: find way to correlate actual trace_id and span_id
  *  TODO: find way to get current timestamp
  *  TODO: find better data type for "body" field
  **/
  LogRecord() : resource(_nullKV), attributes(_nullKV)
  {
    timestamp       = 0;
    // trace_id        = current_trace_id;   // TODO: correlate 
    // span_id         = current_span_id;    // TODO: correlate
    // trace_flag      = current_trace_flag; // TODO: correlate 
    name            = "";
    body            = "";
    resource        = common::KeyValueIterableView<std::map<nostd::string_view, nostd::string_view>>{{}};
    attributes      = common::KeyValueIterableView<std::map<nostd::string_view, nostd::string_view>>{{}};
  }

  /* for ease of use, user can use this function to convert a map into a KeyValueIterable for the resources */
  template <class T, nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  inline void SetResource(const T &_resource)
  {
    resource = common::KeyValueIterableView<T>(_resource);
  }

  /* for ease of use, user to use this function to convert a map into a KeyValueIterable for the attributes */
  template <class T, nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  inline void SetAttributes(const T &_attributes)
  {
    attributes = common::KeyValueIterableView<T>(_attributes);
  }


private:
  // Move to SDK *(logger.cc):
  Severity severity_number = Severity::NONE;
  nostd::string_view severity_text   = severityToString(severity_number);
  inline nostd::string_view severityToString(Severity)
  {
      switch (this->severity_number)
      {
      case Severity::NONE:
          return "NONE";
      case Severity::TRACE:
      case Severity::TRACE2:
      case Severity::TRACE3:
      case Severity::TRACE4:
          return "TRACE";
      case Severity::DEBUG:
      case Severity::DEBUG2:
      case Severity::DEBUG3:
      case Severity::DEBUG4:
          return "DEBUG";
      case Severity::INFO:
      case Severity::INFO2:
      case Severity::INFO3:
      case Severity::INFO4:
          return "INFO";
      case Severity::WARN:
      case Severity::WARN2:
      case Severity::WARN3:
      case Severity::WARN4:
          return "WARN";
      case Severity::ERROR:
      case Severity::ERROR2:
      case Severity::ERROR3:
      case Severity::ERROR4:
          return "ERROR";
      case Severity::FATAL:
      case Severity::FATAL2:
      case Severity::FATAL3:
      case Severity::FATAL4:
          return "FATAL";
      default:
          return "INVALID_SEVERITY";
      }
  }

  /* _nullKV is defined as a private variable that allows "resource" and
    "attributes" fields to be instantiated using it as the default value */
  common::KeyValueIterableView<std::map<nostd::string_view, nostd::string_view>> _nullKV =
      common::KeyValueIterableView<std::map<nostd::string_view, nostd::string_view>>{{}};

};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
