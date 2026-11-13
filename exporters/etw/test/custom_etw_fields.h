// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <string>

inline const char *CustomEventName()
{
  static const std::string value = "shared-generated-event-name-and-span-id";
  return value.c_str();
}
inline const char *CustomSpanId()
{
  static const std::string value = "shared-generated-event-name-and-span-id";
  return value.c_str();
}

#define ETW_FIELD_VERSION "Version"
#define ETW_FIELD_TYPE "Type"
#define ETW_FIELD_NAME CustomEventName()
#define ETW_FIELD_TIME "_time"
#define ETW_FIELD_OPCODE "OpCode"
#define ETW_FIELD_TRACE_ID "shared-trace-id-and-payload-name"
#define ETW_FIELD_SPAN_ID CustomSpanId()
#define ETW_FIELD_SPAN_PARENTID "ParentId"
#define ETW_FIELD_SPAN_KIND "Kind"
#define ETW_FIELD_SPAN_LINKS "Links"
#define ETW_FIELD_SPAN_LINKS_TO_SPAN_ID "toSpanId"
#define ETW_FIELD_SPAN_LINKS_TO_TRACE_ID "toTraceId"
#define ETW_FIELD_PAYLOAD_NAME "shared-trace-id-and-payload-name"
#define ETW_FIELD_STARTTIME "shared-start-time-and-status"
#define ETW_FIELD_ENDTTIME "EndTime"
#define ETW_FIELD_DURATION "shared-duration-and-success"
#define ETW_FIELD_STATUSCODE "shared-start-time-and-status"
#define ETW_FIELD_STATUSMESSAGE "shared-start-time-and-status"
#define ETW_FIELD_SUCCESS "shared-duration-and-success"
#define ETW_FIELD_TIMESTAMP "Timestamp"
#define ETW_VALUE_SPAN "Span"
#define ETW_VALUE_LOG "Log"
#define ETW_VALUE_SPAN_START "SpanStart"
#define ETW_VALUE_SPAN_END "SpanEnd"
#define ETW_FIELD_ENV_PROPERTIES "env_properties"
#define ETW_FIELD_LOG_BODY "body"
#define ETW_FIELD_LOG_SEVERITY_TEXT "severityText"
#define ETW_FIELD_LOG_SEVERITY_NUM "severityNumber"
