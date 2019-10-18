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

#ifndef OPENTELEMETRY_STATUS_H_
#define OPENTELEMETRY_STATUS_H_

#include <stdint.h>

typedef int32_t status_t;

enum {
  STATUS_OK                   = 0,
  STATUS_ERROR_UNKNOWN        = 0x00010000,
  STATUS_ERROR_INPUT_NULL_PTR = 0x00010001,
  STATUS_ERROR_INPUT_ILLEGAL  = 0x00010002,
};

#define IF_ERROR_RETURN(expr) { \
  status_t status = (expr); \
  if (STATUS_OK != status) \
    return status; \
}

#define IF_FALSE_RETURN(expr, retval) { \
  if (!(expr)) \
    return (retval); \
}

#endif  /* OPENTELEMETRY_STATUS_H_ */
