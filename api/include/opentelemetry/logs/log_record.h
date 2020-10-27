/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <map>
#include <unordered_map>
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace logs
{

/* Note: using a class enum here won't allow enum values to be compared to integers, i.e. only other
 * Severity enums (need an explicit cast)
 * Follows the Google standard for naming:
 * https://google.github.io/styleguide/cppguide.html#Enumerator_Names
 */
enum class Severity : uint8_t
{
  kTrace   = 1,
  kTrace2  = 2,
  kTrace3  = 3,
  kTrace4  = 4,
  kDebug   = 5,
  kDebug2  = 6,
  kDebug3  = 7,
  kDebug4  = 8,
  kInfo    = 9,
  kInfo2   = 10,
  kInfo3   = 11,
  kInfo4   = 12,
  kWarn    = 13,
  kWarn2   = 14,
  kWarn3   = 15,
  kWarn4   = 16,
  kError   = 17,
  kError2  = 18,
  kError3  = 19,
  kError4  = 20,
  kFatal   = 21,
  kFatal2  = 22,
  kFatal3  = 23,
  kFatal4  = 24,
  kDefault = kInfo  // default severity is set to kInfo level, similar to what is done in ILogger
};

/* _nullKV is defined as a private variable that allows "resource" and
    "attributes" fields to be instantiated using it as the default value */
static common::KeyValueIterableView<std::map<nostd::string_view, nostd::string_view>> _nullKV =
    common::KeyValueIterableView<std::map<nostd::string_view, nostd::string_view>>{{}};

/**
 * A default Event object to be passed in log statements,
 * matching the 10 fields of the Log Data Model.
 * (https://github.com/open-telemetry/opentelemetry-specification/blob/master/specification/logs/data-model.md#log-and-event-record-definition)
 *
 */
struct LogRecord
{
  // default fields that will be set if the user doesn't specify them
  core::SystemTimestamp timestamp;  // uint64 nanoseconds since Unix epoch
  trace::TraceId trace_id;          // byte sequence
  trace::SpanId span_id;            // byte sequence
  trace::TraceFlags trace_flag;     // byte
  Severity severity;  // Severity enum that combines severity_text and severity_number in the
                      // LogDataModel (can separate in SDK)

  // other fields that will not be set by default
  nostd::string_view name;  // string
  nostd::string_view body;  // currently a simple string, but should be changed "Any" type
  common::KeyValueIterable &resource;    // key/value pair list
  common::KeyValueIterable &attributes;  // key/value pair list

  /* Default log record if user does not overwrite this.
   * TODO: find better data type to represent the <Any> type for "body"
   * Future enhancement: Potentially add other constructors to take default arguments
   * from the user
   **/
  LogRecord() : resource(_nullKV), attributes(_nullKV)
  {
    // TODO: in SDK, assign a default timestamp if not specified
    name = "";
  }

  /* for ease of use; user can use this function to convert a map into a KeyValueIterable for the
   * resources field */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  inline void SetResource(const T &_resource)
  {
    resource = common::KeyValueIterableView<T>(_resource);
  }

  /* for ease of use; user can use this function to convert a map into a KeyValueIterable for the
   * attributes field */
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  inline void SetAttributes(const T &_attributes)
  {
    attributes = common::KeyValueIterableView<T>(_attributes);
  }
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
