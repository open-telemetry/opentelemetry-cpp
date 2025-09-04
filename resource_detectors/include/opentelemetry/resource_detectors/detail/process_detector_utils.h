// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdint.h>
#include <string>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{
namespace detail
{

/**
 * Forms a file path for a process type based on the given PID.
 * for example - /proc/<pid>/cmdline, /proc/<pid>/exe
 */
std::string FormFilePath(const int32_t &pid, const char *process_type);

/**
 * Retrieves the absolute file system path to the executable for a given PID.
 * Platform-specific behavior:
 *   - Windows: Uses OpenProcess() + GetProcessImageFileNameW().
 *   - Linux/Unix: Reads the /proc/<pid>/exe symbolic link.
 *   - TODO: Need to implement for Darwin
 *
 * @param pid Process ID.
 */
std::string GetExecutablePath(const int32_t &pid);

/**
 * Retrieves the command used to launch the process for a given PID.
 * Platform-specific behavior:
 *   - Windows: Uses GetCommandLineW() to get the command of the current process.
 *   - Linux/Unix: Reads the zeroth string of /proc/<pid>/cmdline file.
 *   - TODO: Need to implement for Darwin
 */
std::string ExtractCommand(const std::string &command_line_path);

/**
 * Retrieves the command used to launch the process for a given PID.
 * This function is a wrapper around ExtractCommand() and is provided for convenience and
 * testability of ExtractCommand().
 */
std::string GetCommand(const int32_t &pid);

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
