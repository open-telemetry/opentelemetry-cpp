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

#ifndef OPENTELEMETRY_CONTEXT_TRACEPARENT_H_
#define OPENTELEMETRY_CONTEXT_TRACEPARENT_H_

#include "../status.h"

#ifdef __cplusplus
namespace opentelemetry {
namespace context {
extern "C" {
#endif

#pragma pack(push, 1)
typedef struct
{
  uint8_t version;
  uint8_t trace_id[16];
  uint8_t parent_id[8];
  uint8_t trace_flags;
} TraceParent;
#pragma pack(pop)

/* unchecked version */
status_t _trace_parent_from_string(TraceParent* trace_parent, const char* s);
status_t _trace_parent_to_string(const TraceParent* trace_parent, char* s);

/* checked version */
status_t trace_parent_from_string(TraceParent* trace_parent, const char* s);
status_t trace_parent_to_string(const TraceParent* trace_parent, char* s);

#ifdef __cplusplus
}  /* extern "C" */
}  /* namespace context */
}  /* namespace opentelemetry */
#endif

#endif  /* OPENTELEMETRY_CONTEXT_TRACEPARENT_H_ */
