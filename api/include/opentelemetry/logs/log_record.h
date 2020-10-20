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
enum class Severity : uint8_t
{ //Follows this standard: https://google.github.io/styleguide/cppguide.html#Enumerator_Names
  kNone   = 0,       // default Severity; added, not part of Log Data Model
  kTrace  = 1,
  KTrace2 = 2,
  kTrace3 = 3,
  kTrace4 = 4,
  kDebug  = 5,
  kDebug2 = 6,
  kDebug3 = 7,
  kDebug4 = 8,
  kInfo   = 9,
  kInfo2  = 10,
  kInfo3  = 11,
  kInfo4  = 12,
  kWarn   = 13,
  kWarn2  = 14,
  kWarn3  = 15,
  kWarn4  = 16,
  kError  = 17,
  kError2 = 18,
  kError3 = 19,
  kError4 = 20,
  kFatal  = 21,
  kFatal2 = 22,
  kFatal3 = 23,
  kFatal4 = 24
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
  core::SystemTimestamp timestamp;                   // uint64 nanoseconds since Unix epoch
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
    timestamp       = core::SystemTimestamp(std::chrono::system_clock::now());
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
  /* _nullKV is defined as a private variable that allows "resource" and
    "attributes" fields to be instantiated using it as the default value */
  common::KeyValueIterableView<std::map<nostd::string_view, nostd::string_view>> _nullKV =
      common::KeyValueIterableView<std::map<nostd::string_view, nostd::string_view>>{{}};

};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
