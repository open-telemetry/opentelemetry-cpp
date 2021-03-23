/* Copyright 2020-2021, OpenTelemetry Authors
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

#include <unordered_map>

#include "opentelemetry/exporters/etw/utils.h"

/* clang-format off */
#ifdef CUSTOM_ETW_FIELDS_H
#  include CUSTOM_ETW_FIELDS_H
#else

/* ETW constants at envelope level that would be appended at different spot depending
 * on what serializer used to encode the payload
 */
#  define ETW_FIELD_NAME            "name"                  /* Event name             */
#  define ETW_FIELD_TYPE            "type"                  /* Event type             */
#  define ETW_FIELD_VERSION         "ver"                   /* Event version          */
#  define ETW_FIELD_TIME            "time"                  /* Event time at envelope */
#  define ETW_FIELD_KIND            "kind"                  /* Event kind             */

/* Common TraceId constant to associate events with a trace */
#  define ETW_FIELD_TRACE_ID        "TraceId"               /* Trace Id               */

/* Span constants */
#  define ETW_FIELD_SPAN_ID         "SpanId"                /* SpanId                 */
#  define ETW_FIELD_SPAN_KIND       "SpanKind"              /* SpanKind               */
#  define ETW_FIELD_SPAN_LINKS      "Links"                 /* Span Links array       */
#  define ETW_FIELD_SPAN_PARENTID   "ParentId"              /* Span ParentId          */

/* Span option constants */
#  define ETW_FIELD_STARTTIME       "Time"                  /* Operation start time   */
#  define ETW_FIELD_DURATION        "Duration"              /* Operation duration     */
#  define ETW_FIELD_STATUSCODE      "SpanStatusCode"        /* OT Span status code    */

/* Value constants */
#  define ETW_VALUE_SPAN_START      "SpanStart"             /* ETW for Span Start     */
#  define ETW_VALUE_SPAN_END        "SpanEnd"               /* ETW for Span Start     */
#  define ETW_VALUE_SUCCESS         "Success"               /* OT Span success        */

#endif

/* clang-format on */
