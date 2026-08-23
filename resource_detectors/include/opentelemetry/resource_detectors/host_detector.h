// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{

/**
 * HostResourceDetector to detect resource attributes describing the host the process
 * runs on. It sets host.name, host.arch and host.id following the OpenTelemetry
 * semantic conventions.
 *
 * Platform specific behavior and limitations:
 * - host.name is read with gethostname() on Linux and MacOS and with
 *   GetComputerNameExW() on Windows.
 * - host.id is read from /etc/machine-id (falling back to /var/lib/dbus/machine-id)
 *   on Linux, from gethostuuid() on MacOS and from the
 *   HKLM\SOFTWARE\Microsoft\Cryptography\MachineGuid registry value on Windows.
 *   The attribute is omitted when no source is available, for example in Linux
 *   containers without a machine id.
 * - host.arch is derived at compile time from the architecture the SDK was built
 *   for and is omitted when that architecture has no host.arch value in the
 *   semantic conventions.
 */
class HostResourceDetector : public opentelemetry::sdk::resource::ResourceDetector
{
public:
  opentelemetry::sdk::resource::Resource Detect() noexcept override;
};

}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
