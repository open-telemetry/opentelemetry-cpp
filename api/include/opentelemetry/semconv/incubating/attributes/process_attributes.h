

/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * DO NOT EDIT, this is an Auto-generated file from:
 * buildscripts/semantic-convention/templates/registry/semantic_attributes-h.j2
 */

#pragma once

#include "opentelemetry/common/macros.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace semconv
{
namespace process
{

/**
 * The command used to launch the process (i.e. the command name). On Linux based systems, can be
 * set to the zeroth string in @code proc/[pid]/cmdline @endcode. On Windows, can be set to the
 * first parameter extracted from @code GetCommandLineW @endcode.
 */
static const char *kProcessCommand = "process.command";

/**
 * All the command arguments (including the command/executable itself) as received by the process.
 * On Linux-based systems (and some other Unixoid systems supporting procfs), can be set according
 * to the list of null-delimited strings extracted from @code proc/[pid]/cmdline @endcode. For
 * libc-based executables, this would be the full argv vector passed to @code main @endcode.
 */
static const char *kProcessCommandArgs = "process.command_args";

/**
 * The full command used to launch the process as a single string representing the full command. On
 * Windows, can be set to the result of @code GetCommandLineW @endcode. Do not set this if you have
 * to assemble it just for monitoring; use @code process.command_args @endcode instead.
 */
static const char *kProcessCommandLine = "process.command_line";

/**
 * Specifies whether the context switches for this data point were voluntary or involuntary.
 */
static const char *kProcessContextSwitchType = "process.context_switch_type";

/**
 * @Deprecated: Replaced by @code cpu.mode @endcode.
 */
static const char *kProcessCpuState = "process.cpu.state";

/**
 * The date and time the process was created, in ISO 8601 format.
 */
static const char *kProcessCreationTime = "process.creation.time";

/**
 * The name of the process executable. On Linux based systems, can be set to the @code Name @endcode
 * in @code proc/[pid]/status @endcode. On Windows, can be set to the base name of @code
 * GetProcessImageFileNameW @endcode.
 */
static const char *kProcessExecutableName = "process.executable.name";

/**
 * The full path to the process executable. On Linux based systems, can be set to the target of
 * @code proc/[pid]/exe @endcode. On Windows, can be set to the result of @code
 * GetProcessImageFileNameW @endcode.
 */
static const char *kProcessExecutablePath = "process.executable.path";

/**
 * The exit code of the process.
 */
static const char *kProcessExitCode = "process.exit.code";

/**
 * The date and time the process exited, in ISO 8601 format.
 */
static const char *kProcessExitTime = "process.exit.time";

/**
 * The PID of the process's group leader. This is also the process group ID (PGID) of the process.
 */
static const char *kProcessGroupLeaderPid = "process.group_leader.pid";

/**
 * Whether the process is connected to an interactive shell.
 */
static const char *kProcessInteractive = "process.interactive";

/**
 * The username of the user that owns the process.
 */
static const char *kProcessOwner = "process.owner";

/**
 * The type of page fault for this data point. Type @code major @endcode is for major/hard page
 * faults, and @code minor @endcode is for minor/soft page faults.
 */
static const char *kProcessPagingFaultType = "process.paging.fault_type";

/**
 * Parent Process identifier (PPID).
 */
static const char *kProcessParentPid = "process.parent_pid";

/**
 * Process identifier (PID).
 */
static const char *kProcessPid = "process.pid";

/**
 * The real user ID (RUID) of the process.
 */
static const char *kProcessRealUserId = "process.real_user.id";

/**
 * The username of the real user of the process.
 */
static const char *kProcessRealUserName = "process.real_user.name";

/**
 * An additional description about the runtime of the process, for example a specific vendor
 * customization of the runtime environment.
 */
static const char *kProcessRuntimeDescription = "process.runtime.description";

/**
 * The name of the runtime of this process.
 */
static const char *kProcessRuntimeName = "process.runtime.name";

/**
 * The version of the runtime of this process, as returned by the runtime without modification.
 */
static const char *kProcessRuntimeVersion = "process.runtime.version";

/**
 * The saved user ID (SUID) of the process.
 */
static const char *kProcessSavedUserId = "process.saved_user.id";

/**
 * The username of the saved user.
 */
static const char *kProcessSavedUserName = "process.saved_user.name";

/**
 * The PID of the process's session leader. This is also the session ID (SID) of the process.
 */
static const char *kProcessSessionLeaderPid = "process.session_leader.pid";

/**
 * The effective user ID (EUID) of the process.
 */
static const char *kProcessUserId = "process.user.id";

/**
 * The username of the effective user of the process.
 */
static const char *kProcessUserName = "process.user.name";

/**
 * Virtual process identifier.
 * Note: The process ID within a PID namespace. This is not necessarily unique across all processes
 * on the host but it is unique within the process namespace that the process exists within.
 */
static const char *kProcessVpid = "process.vpid";

// DEBUG: {"brief": "Specifies whether the context switches for this data point were voluntary or
// involuntary.", "name": "process.context_switch_type", "requirement_level": "recommended",
// "root_namespace": "process", "stability": "experimental", "type": {"allow_custom_values": true,
// "members": [{"brief": none, "deprecated": none, "id": "voluntary", "note": none, "stability":
// "experimental", "value": "voluntary"}, {"brief": none, "deprecated": none, "id": "involuntary",
// "note": none, "stability": "experimental", "value": "involuntary"}]}}
namespace ProcessContextSwitchTypeValues
{
/**
 * voluntary.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "voluntary", "note": none, "stability":
// "experimental", "value": "voluntary"}
static constexpr const char *kVoluntary = "voluntary";
/**
 * involuntary.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "involuntary", "note": none, "stability":
// "experimental", "value": "involuntary"}
static constexpr const char *kInvoluntary = "involuntary";
}  // namespace ProcessContextSwitchTypeValues

// @deprecated(reason="The attribute process.cpu.state is deprecated - Replaced by `cpu.mode`")  #
// type: ignore DEBUG: {"brief": "Deprecated, use `cpu.mode` instead.", "deprecated": "Replaced by
// `cpu.mode`", "name": "process.cpu.state", "requirement_level": "recommended", "root_namespace":
// "process", "stability": "experimental", "type": {"allow_custom_values": true, "members":
// [{"brief": none, "deprecated": none, "id": "system", "note": none, "stability": "experimental",
// "value": "system"}, {"brief": none, "deprecated": none, "id": "user", "note": none, "stability":
// "experimental", "value": "user"}, {"brief": none, "deprecated": none, "id": "wait", "note": none,
// "stability": "experimental", "value": "wait"}]}}
namespace ProcessCpuStateValues
{
/**
 * system.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "system", "note": none, "stability":
// "experimental", "value": "system"}
static constexpr const char *kSystem = "system";
/**
 * user.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "user", "note": none, "stability":
// "experimental", "value": "user"}
static constexpr const char *kUser = "user";
/**
 * wait.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "wait", "note": none, "stability":
// "experimental", "value": "wait"}
static constexpr const char *kWait = "wait";
}  // namespace ProcessCpuStateValues

// DEBUG: {"brief": "The type of page fault for this data point. Type `major` is for major/hard page
// faults, and `minor` is for minor/soft page faults.\n", "name": "process.paging.fault_type",
// "requirement_level": "recommended", "root_namespace": "process", "stability": "experimental",
// "type": {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id":
// "major", "note": none, "stability": "experimental", "value": "major"}, {"brief": none,
// "deprecated": none, "id": "minor", "note": none, "stability": "experimental", "value":
// "minor"}]}}
namespace ProcessPagingFaultTypeValues
{
/**
 * major.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "major", "note": none, "stability":
// "experimental", "value": "major"}
static constexpr const char *kMajor = "major";
/**
 * minor.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "minor", "note": none, "stability":
// "experimental", "value": "minor"}
static constexpr const char *kMinor = "minor";
}  // namespace ProcessPagingFaultTypeValues

}  // namespace process
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
