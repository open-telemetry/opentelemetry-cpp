// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{
namespace detail
{

/**
 * Retrieves the hostname.
 * Platform-specific behavior:
 *   - Windows: Uses GetComputerNameExW() with ComputerNameDnsHostname.
 *   - Linux/MacOS: Uses gethostname().
 * @return the hostname or an empty string on error
 */
std::string GetHostName();

/**
 * Returns the host.arch semantic convention value for the architecture this
 * binary was compiled for, or an empty string when the architecture is not one
 * of the values the semantic conventions enumerate.
 */
std::string GetHostArch();

/**
 * Retrieves a unique identifier for the host.
 * Platform-specific behavior:
 *   - Windows: Reads the MachineGuid value from the
 *     HKLM\SOFTWARE\Microsoft\Cryptography registry key.
 *   - MacOS: Uses gethostuuid().
 *   - Linux/Unix: Reads /etc/machine-id, falling back to
 *     /var/lib/dbus/machine-id.
 * @return the host id or an empty string when no source is available
 */
std::string GetHostId();

/**
 * Reads a machine id from the first line of the given file, trimmed of
 * surrounding whitespace. This function is the Linux implementation behind
 * GetHostId() and is exposed for testability.
 * @param file_path path of the machine id file
 * @return the machine id or an empty string on error
 */
std::string GetHostIdFromFile(const char *file_path);

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
