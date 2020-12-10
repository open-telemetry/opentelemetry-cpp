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
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace logs
{

/**
 * Severity Levels assigned to log events, based on Log Data Model
 */
enum class Severity : uint8_t
{
  kInvalid = 0,
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
  kFatal4  = 24
};

}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
