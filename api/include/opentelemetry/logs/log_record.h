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

enum class Severity  // using a class enum here won't allow enum values to be compared to integers
                     // only other Severity enums (need an explicit cast)
{
  NONE   = 0,  // default Severity; added, not part of Log Data Model
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
  uint64_t timestamp = 0;            // uint64 nanoseconds since Unix epoch
  trace::TraceId trace_id;           // byte sequence
  trace::SpanId span_id;             // byte sequence
  trace::TraceFlags trace_flag;      // byte
  nostd::string_view severity_text;  // string; TODO: add a severity_num_to_text() method in
                                     // Logger.h's Severity class enum?
  // Severity severity_number = Severity::NONE;  // integer

  // other fields
  nostd::string_view name;             // string
  common::AttributeValue body;         // any type
  nostd::shared_ptr<common::KeyValueIterable> resource;    // key/value pair list
  nostd::shared_ptr<common::KeyValueIterable> attributes;  // key/value pair list
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
