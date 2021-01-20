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

#ifdef CUSTOM_ETW_FIELDS_H
#  include CUSTOM_ETW_FIELDS_H
#else
/* clang-format off */

/* Field name constants */
#  define ETW_FIELD_NAME            "name"                  /* Event name             */
#  define ETW_FIELD_TYPE            "type"                  /* Event type (TLD)       */
#  define ETW_FIELD_VERSION         "ver"                   /* Event version          */
#  define ETW_FIELD_TIME            "time"                  /* Event time             */
#  define ETW_FIELD_KIND            "kind"                  /* Event kind (MsgPack)   */

/* Common TraceId constant to assoc events with trace parent */
#  define ETW_FIELD_TRACE_ID        "TraceId"               /* OT Trace Id            */

/* Span constants */
#  define ETW_FIELD_SPAN_ID         "SpanId"                /* OT Span Id             */
#  define ETW_FIELD_SPAN_START      "SpanStart"             /* ETW for Span Start     */
#  define ETW_FIELD_SPAN_END        "SpanEnd"               /* ETW for Span Start     */
#  define ETW_FIELD_SPAN_LINKS      "SpanLinks"             /* OT Span Links array    */
#  define ETW_FIELD_SPAN_ATTRIBS    "SpanAttributes"        /* OT Span Attribs array  */
#  define ETW_FIELD_SPAN_PARENTID   "SpanParentId"          /* OT Span ParentId       */
#  define ETW_FIELD_SPAN_KIND       "SpanKind"

/* Span option constants */
#  define ETW_FIELD_STARTTIME       "startTime"             /* ETW event startTime    */
#  define ETW_FIELD_DURATION        "duration"              /* OT Span or Op duration */
#  define ETW_FIELD_SUCCESS         "success"               /* OT Span success        */
#  define ETW_FIELD_STATUSCODE      "statusCode"            /* OT Span status code    */

/* clang-format on */
#endif
