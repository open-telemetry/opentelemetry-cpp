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
