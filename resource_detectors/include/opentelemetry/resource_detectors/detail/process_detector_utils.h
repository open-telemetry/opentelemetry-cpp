// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <string>
#include <vector>

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
 * Returns the base name (filename) of the process executable.
 * Derived from GetExecutablePath() by stripping the directory components.
 * Platform-specific behavior mirrors GetExecutablePath().
 *
 * @param pid Process ID.
 */
std::string GetExecutableName(const int32_t &pid);

/**
 * Extracts the command-line arguments and the command.
 * Platform-specific behavior:
 *   - Windows: Uses CommandLineToArgvW() to parse the command line.
 *   - Linux/Unix: Reads the /proc/<pid>/cmdline file and splits it into command and arguments.
 *   - TODO: Need to implement for Darwin
 */
std::vector<std::string> ExtractCommandWithArgs(const std::string &command_line_path);

/**
 * Retrieves the command-line arguments and the command used to launch the process for a given PID.
 * This function is a wrapper around ExtractCommandWithArgs() and is provided for convenience and
 * testability of ExtractCommandWithArgs().
 */
std::vector<std::string> GetCommandWithArgs(const int32_t &pid);

/**
 * Retrieves the process creation time as an ISO 8601 string (e.g. "2023-11-21T09:25:34.853Z").
 * Platform-specific behavior:
 *   - Linux: Reads starttime from /proc/<pid>/stat and combines with boot time.
 *   - macOS: Uses sysctl(KERN_PROC) to obtain kinfo_proc.kp_proc.p_starttime.
 *   - Windows: Uses GetProcessTimes() to obtain lpCreationTime (FILETIME).
 * Returns an empty string if the information is unavailable.
 *
 * @param pid Process ID.
 */
std::string GetProcessCreationTime(const int32_t &pid);

/**
 * Retrieves the username of the user that owns the process.
 * Platform-specific behavior:
 *   - Linux/macOS: Uses getuid() + getpwuid_r() to resolve the effective user name.
 *   - Windows: Uses OpenProcessToken() + GetTokenInformation() + LookupAccountSidW().
 * Returns an empty string if the information is unavailable.
 *
 * @param pid Process ID.
 */
std::string GetProcessOwner(const int32_t &pid);

/**
 * Computes the deterministic htlhash build ID for the process executable.
 * Algorithm: SHA256(File[:4096] || File[-4096:] || BigEndianUInt64(FileLen))
 * The result is the first 16 bytes (128 bits) of the digest as a lowercase hex string.
 * Returns an empty string if the executable cannot be read.
 *
 * @param pid Process ID.
 */
std::string GetExecutableBuildIdHtlhash(const int32_t &pid);

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
