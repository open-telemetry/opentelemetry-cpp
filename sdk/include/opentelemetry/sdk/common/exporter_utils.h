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
namespace sdk
{
namespace common
{
/**
 * ExportResult is returned as result of exporting a batch of Records.
 */
enum class ExportResult
{
  // Batch was exported successfully.
  kSuccess = 0,

  // Batch exporting failed, caller must not retry exporting the same batch
  // and the batch must be dropped.
  kFailure = 1,

  // The collection does not have enough space to receive the export batch.
  kFailureFull = 2,

  // The export() function was passed an invalid argument.
  kFailureInvalidArgument = 3
};

}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
