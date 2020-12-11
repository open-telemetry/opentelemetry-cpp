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
 * Severity Levels assigned to log events, based on Log Data Model,
 * with the addition of kInvalid (mapped to a severity number of 0).
 */
enum class Severity : uint8_t
{
  kInvalid,
  kTrace,
  kTrace2,
  kTrace3,
  kTrace4,
  kDebug,
  kDebug2,
  kDebug3,
  kDebug4,
  kInfo,
  kInfo2,
  kInfo3,
  kInfo4,
  kWarn,
  kWarn2,
  kWarn3,
  kWarn4,
  kError,
  kError2,
  kError3,
  kError4,
  kFatal,
  kFatal2,
  kFatal3,
  kFatal4
};

}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
