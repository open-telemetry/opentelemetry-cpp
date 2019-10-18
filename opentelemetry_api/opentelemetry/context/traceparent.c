/* Copyright 2019, OpenTelemetry Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "traceparent.h"

status_t _uint_from_hex_string(unsigned int* value, const char* s) {
  char c = s[0];
  unsigned int result;

  if (c >= '0' && c <= '9')
  {
    result = c - '0';
  }
  else if (c >= 'a' && c <= 'f')
  {
    result = c - 'a' + 10;
  }
  else
  {
    return STATUS_ERROR_INPUT_ILLEGAL;
  }
  result <<= 4;
  c = s[1];
  if (c >= '0' && c <= '9')
  {
    result += c - '0';
  }
  else if (c >= 'a' && c <= 'f')
  {
    result += c - 'a' + 10;
  }
  else
  {
    return STATUS_ERROR_INPUT_ILLEGAL;
  }
  *value = result;

  return STATUS_OK;
}

status_t _trace_parent_from_string(TraceParent* trace_parent, const char* s)
{
  unsigned int value;
  /* if this function failed, trace_parent could be polluted,
     do we want to take a copy (perf sucks)?
   */
  IF_ERROR_RETURN(_uint_from_hex_string(&value, s));
  trace_parent->version = value;
  s += 2;
  IF_FALSE_RETURN('-' == *s, STATUS_ERROR_INPUT_ILLEGAL)
  s += 1;
  for (int i = 0; i < 16; i++)
  {
    IF_ERROR_RETURN(_uint_from_hex_string(&value, s));
    trace_parent->trace_id[i] = value;
    s += 2;
  }
  IF_FALSE_RETURN('-' == *s, STATUS_ERROR_INPUT_ILLEGAL)
  s += 1;
  for (int i = 0; i < 8; i++)
  {
    IF_ERROR_RETURN(_uint_from_hex_string(&value, s));
    trace_parent->parent_id[i] = value;
    s += 2;
  }
  IF_FALSE_RETURN('-' == *s, STATUS_ERROR_INPUT_ILLEGAL)
  s += 1;
  IF_ERROR_RETURN(_uint_from_hex_string(&value, s));
  trace_parent->trace_flags = value;
  s += 2;
  IF_FALSE_RETURN(('-' == *s || ('\0' == *s)), STATUS_ERROR_INPUT_ILLEGAL)
  return STATUS_OK;
}

status_t _trace_parent_to_string(const TraceParent* trace_parent, char* s)
{
  return STATUS_ERROR_UNKNOWN;
}

status_t trace_parent_from_string(TraceParent* trace_parent, const char* s)
{
  IF_FALSE_RETURN(trace_parent, STATUS_ERROR_INPUT_NULL_PTR)
  IF_FALSE_RETURN(s, STATUS_ERROR_INPUT_NULL_PTR)

  return _trace_parent_from_string(trace_parent, s);
}

status_t trace_parent_to_string(const TraceParent* trace_parent, char* s)
{
  IF_FALSE_RETURN(trace_parent, STATUS_ERROR_INPUT_NULL_PTR)
  IF_FALSE_RETURN(s, STATUS_ERROR_INPUT_NULL_PTR)

  return _trace_parent_to_string(trace_parent, s);
}
