// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{

/**
 * ProcessResourceDetector to detect resource attributes when running in a process.
 * This detector extracts metadata such as process ID, executable path, executable name,
 * process creation time, process owner, and executable build ID, then sets attributes
 * following the OpenTelemetry semantic conventions:
 *
 *  Process entity attributes:
 *  - process.pid                        (required)  — current process identifier
 *  - process.creation.time              (required)  — ISO 8601 UTC creation timestamp
 *  - process.owner                      (recommended) — username of the process owner
 *
 *  Process Executable entity attributes:
 *  - process.executable.path            (recommended) — full path via /proc or Win32 APIs
 *  - process.executable.name            (recommended) — basename of the executable path
 *  - process.executable.build_id.htlhash (required) — deterministic SHA256-based build ID
 *
 * Attributes that cannot be determined on the current platform are omitted.
 */
class ProcessResourceDetector : public opentelemetry::sdk::resource::ResourceDetector
{
public:
  /**
   * Detect retrieves the resource attributes for the current process.
   * See the class-level documentation for the complete list of attributes populated.
   */
  opentelemetry::sdk::resource::Resource Detect() noexcept override;
};

}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
