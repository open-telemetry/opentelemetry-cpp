

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
namespace system
{

/**
 * The logical CPU number [0..n-1].
 */
static const char *kSystemCpuLogicalNumber = "system.cpu.logical_number";

/**
 * @Deprecated: Replaced by @code cpu.mode @endcode.
 */
static const char *kSystemCpuState = "system.cpu.state";

/**
 * The device identifier.
 */
static const char *kSystemDevice = "system.device";

/**
 * The filesystem mode.
 */
static const char *kSystemFilesystemMode = "system.filesystem.mode";

/**
 * The filesystem mount path.
 */
static const char *kSystemFilesystemMountpoint = "system.filesystem.mountpoint";

/**
 * The filesystem state.
 */
static const char *kSystemFilesystemState = "system.filesystem.state";

/**
 * The filesystem type.
 */
static const char *kSystemFilesystemType = "system.filesystem.type";

/**
 * The memory state.
 */
static const char *kSystemMemoryState = "system.memory.state";

/**
 * A stateless protocol MUST NOT set this attribute.
 */
static const char *kSystemNetworkState = "system.network.state";

/**
 * The paging access direction.
 */
static const char *kSystemPagingDirection = "system.paging.direction";

/**
 * The memory paging state.
 */
static const char *kSystemPagingState = "system.paging.state";

/**
 * The memory paging type.
 */
static const char *kSystemPagingType = "system.paging.type";

/**
 * The process state, e.g., <a
 * href="https://man7.org/linux/man-pages/man1/ps.1.html#PROCESS_STATE_CODES">Linux Process State
 * Codes</a>.
 */
static const char *kSystemProcessStatus = "system.process.status";

/**
 * @Deprecated: Replaced by @code system.process.status @endcode.
 */
static const char *kSystemProcessesStatus = "system.processes.status";

// @deprecated(reason="The attribute system.cpu.state is deprecated - Replaced by `cpu.mode`")  #
// type: ignore DEBUG: {"brief": "Deprecated, use `cpu.mode` instead.", "deprecated": "Replaced by
// `cpu.mode`", "examples": ["idle", "interrupt"], "name": "system.cpu.state", "requirement_level":
// "recommended", "root_namespace": "system", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id": "user",
// "note": none, "stability": "experimental", "value": "user"}, {"brief": none, "deprecated": none,
// "id": "system", "note": none, "stability": "experimental", "value": "system"}, {"brief": none,
// "deprecated": none, "id": "nice", "note": none, "stability": "experimental", "value": "nice"},
// {"brief": none, "deprecated": none, "id": "idle", "note": none, "stability": "experimental",
// "value": "idle"}, {"brief": none, "deprecated": none, "id": "iowait", "note": none, "stability":
// "experimental", "value": "iowait"}, {"brief": none, "deprecated": none, "id": "interrupt",
// "note": none, "stability": "experimental", "value": "interrupt"}, {"brief": none, "deprecated":
// none, "id": "steal", "note": none, "stability": "experimental", "value": "steal"}]}}
namespace SystemCpuStateValues
{
/**
 * user.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "user", "note": none, "stability":
// "experimental", "value": "user"}
static constexpr const char *kUser = "user";
/**
 * system.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "system", "note": none, "stability":
// "experimental", "value": "system"}
static constexpr const char *kSystem = "system";
/**
 * nice.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "nice", "note": none, "stability":
// "experimental", "value": "nice"}
static constexpr const char *kNice = "nice";
/**
 * idle.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "idle", "note": none, "stability":
// "experimental", "value": "idle"}
static constexpr const char *kIdle = "idle";
/**
 * iowait.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "iowait", "note": none, "stability":
// "experimental", "value": "iowait"}
static constexpr const char *kIowait = "iowait";
/**
 * interrupt.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "interrupt", "note": none, "stability":
// "experimental", "value": "interrupt"}
static constexpr const char *kInterrupt = "interrupt";
/**
 * steal.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "steal", "note": none, "stability":
// "experimental", "value": "steal"}
static constexpr const char *kSteal = "steal";
}  // namespace SystemCpuStateValues

// DEBUG: {"brief": "The filesystem state", "examples": ["used"], "name": "system.filesystem.state",
// "requirement_level": "recommended", "root_namespace": "system", "stability": "experimental",
// "type": {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id":
// "used", "note": none, "stability": "experimental", "value": "used"}, {"brief": none,
// "deprecated": none, "id": "free", "note": none, "stability": "experimental", "value": "free"},
// {"brief": none, "deprecated": none, "id": "reserved", "note": none, "stability": "experimental",
// "value": "reserved"}]}}
namespace SystemFilesystemStateValues
{
/**
 * used.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "used", "note": none, "stability":
// "experimental", "value": "used"}
static constexpr const char *kUsed = "used";
/**
 * free.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "free", "note": none, "stability":
// "experimental", "value": "free"}
static constexpr const char *kFree = "free";
/**
 * reserved.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "reserved", "note": none, "stability":
// "experimental", "value": "reserved"}
static constexpr const char *kReserved = "reserved";
}  // namespace SystemFilesystemStateValues

// DEBUG: {"brief": "The filesystem type", "examples": ["ext4"], "name": "system.filesystem.type",
// "requirement_level": "recommended", "root_namespace": "system", "stability": "experimental",
// "type": {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id":
// "fat32", "note": none, "stability": "experimental", "value": "fat32"}, {"brief": none,
// "deprecated": none, "id": "exfat", "note": none, "stability": "experimental", "value": "exfat"},
// {"brief": none, "deprecated": none, "id": "ntfs", "note": none, "stability": "experimental",
// "value": "ntfs"}, {"brief": none, "deprecated": none, "id": "refs", "note": none, "stability":
// "experimental", "value": "refs"}, {"brief": none, "deprecated": none, "id": "hfsplus", "note":
// none, "stability": "experimental", "value": "hfsplus"}, {"brief": none, "deprecated": none, "id":
// "ext4", "note": none, "stability": "experimental", "value": "ext4"}]}}
namespace SystemFilesystemTypeValues
{
/**
 * fat32.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "fat32", "note": none, "stability":
// "experimental", "value": "fat32"}
static constexpr const char *kFat32 = "fat32";
/**
 * exfat.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "exfat", "note": none, "stability":
// "experimental", "value": "exfat"}
static constexpr const char *kExfat = "exfat";
/**
 * ntfs.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "ntfs", "note": none, "stability":
// "experimental", "value": "ntfs"}
static constexpr const char *kNtfs = "ntfs";
/**
 * refs.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "refs", "note": none, "stability":
// "experimental", "value": "refs"}
static constexpr const char *kRefs = "refs";
/**
 * hfsplus.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "hfsplus", "note": none, "stability":
// "experimental", "value": "hfsplus"}
static constexpr const char *kHfsplus = "hfsplus";
/**
 * ext4.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "ext4", "note": none, "stability":
// "experimental", "value": "ext4"}
static constexpr const char *kExt4 = "ext4";
}  // namespace SystemFilesystemTypeValues

// DEBUG: {"brief": "The memory state", "examples": ["free", "cached"], "name":
// "system.memory.state", "requirement_level": "recommended", "root_namespace": "system",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": none,
// "deprecated": none, "id": "used", "note": none, "stability": "experimental", "value": "used"},
// {"brief": none, "deprecated": none, "id": "free", "note": none, "stability": "experimental",
// "value": "free"}, {"brief": none, "deprecated": "Removed, report shared memory usage with
// `metric.system.memory.shared` metric", "id": "shared", "note": none, "stability": "experimental",
// "value": "shared"}, {"brief": none, "deprecated": none, "id": "buffers", "note": none,
// "stability": "experimental", "value": "buffers"}, {"brief": none, "deprecated": none, "id":
// "cached", "note": none, "stability": "experimental", "value": "cached"}]}}
namespace SystemMemoryStateValues
{
/**
 * used.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "used", "note": none, "stability":
// "experimental", "value": "used"}
static constexpr const char *kUsed = "used";
/**
 * free.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "free", "note": none, "stability":
// "experimental", "value": "free"}
static constexpr const char *kFree = "free";
/**
 * @Deprecated: Removed, report shared memory usage with @code metric.system.memory.shared @endcode
 * metric.
 */
// DEBUG: {"brief": none, "deprecated": "Removed, report shared memory usage with
// `metric.system.memory.shared` metric", "id": "shared", "note": none, "stability": "experimental",
// "value": "shared"}
static constexpr const char *kShared = "shared";
/**
 * buffers.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "buffers", "note": none, "stability":
// "experimental", "value": "buffers"}
static constexpr const char *kBuffers = "buffers";
/**
 * cached.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "cached", "note": none, "stability":
// "experimental", "value": "cached"}
static constexpr const char *kCached = "cached";
}  // namespace SystemMemoryStateValues

// DEBUG: {"brief": "A stateless protocol MUST NOT set this attribute", "examples": ["close_wait"],
// "name": "system.network.state", "requirement_level": "recommended", "root_namespace": "system",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": none,
// "deprecated": none, "id": "close", "note": none, "stability": "experimental", "value": "close"},
// {"brief": none, "deprecated": none, "id": "close_wait", "note": none, "stability":
// "experimental", "value": "close_wait"}, {"brief": none, "deprecated": none, "id": "closing",
// "note": none, "stability": "experimental", "value": "closing"}, {"brief": none, "deprecated":
// none, "id": "delete", "note": none, "stability": "experimental", "value": "delete"}, {"brief":
// none, "deprecated": none, "id": "established", "note": none, "stability": "experimental",
// "value": "established"}, {"brief": none, "deprecated": none, "id": "fin_wait_1", "note": none,
// "stability": "experimental", "value": "fin_wait_1"}, {"brief": none, "deprecated": none, "id":
// "fin_wait_2", "note": none, "stability": "experimental", "value": "fin_wait_2"}, {"brief": none,
// "deprecated": none, "id": "last_ack", "note": none, "stability": "experimental", "value":
// "last_ack"}, {"brief": none, "deprecated": none, "id": "listen", "note": none, "stability":
// "experimental", "value": "listen"}, {"brief": none, "deprecated": none, "id": "syn_recv", "note":
// none, "stability": "experimental", "value": "syn_recv"}, {"brief": none, "deprecated": none,
// "id": "syn_sent", "note": none, "stability": "experimental", "value": "syn_sent"}, {"brief":
// none, "deprecated": none, "id": "time_wait", "note": none, "stability": "experimental", "value":
// "time_wait"}]}}
namespace SystemNetworkStateValues
{
/**
 * close.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "close", "note": none, "stability":
// "experimental", "value": "close"}
static constexpr const char *kClose = "close";
/**
 * close_wait.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "close_wait", "note": none, "stability":
// "experimental", "value": "close_wait"}
static constexpr const char *kCloseWait = "close_wait";
/**
 * closing.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "closing", "note": none, "stability":
// "experimental", "value": "closing"}
static constexpr const char *kClosing = "closing";
/**
 * delete.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "delete", "note": none, "stability":
// "experimental", "value": "delete"}
static constexpr const char *kDelete = "delete";
/**
 * established.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "established", "note": none, "stability":
// "experimental", "value": "established"}
static constexpr const char *kEstablished = "established";
/**
 * fin_wait_1.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "fin_wait_1", "note": none, "stability":
// "experimental", "value": "fin_wait_1"}
static constexpr const char *kFinWait1 = "fin_wait_1";
/**
 * fin_wait_2.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "fin_wait_2", "note": none, "stability":
// "experimental", "value": "fin_wait_2"}
static constexpr const char *kFinWait2 = "fin_wait_2";
/**
 * last_ack.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "last_ack", "note": none, "stability":
// "experimental", "value": "last_ack"}
static constexpr const char *kLastAck = "last_ack";
/**
 * listen.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "listen", "note": none, "stability":
// "experimental", "value": "listen"}
static constexpr const char *kListen = "listen";
/**
 * syn_recv.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "syn_recv", "note": none, "stability":
// "experimental", "value": "syn_recv"}
static constexpr const char *kSynRecv = "syn_recv";
/**
 * syn_sent.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "syn_sent", "note": none, "stability":
// "experimental", "value": "syn_sent"}
static constexpr const char *kSynSent = "syn_sent";
/**
 * time_wait.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "time_wait", "note": none, "stability":
// "experimental", "value": "time_wait"}
static constexpr const char *kTimeWait = "time_wait";
}  // namespace SystemNetworkStateValues

// DEBUG: {"brief": "The paging access direction", "examples": ["in"], "name":
// "system.paging.direction", "requirement_level": "recommended", "root_namespace": "system",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": none,
// "deprecated": none, "id": "in", "note": none, "stability": "experimental", "value": "in"},
// {"brief": none, "deprecated": none, "id": "out", "note": none, "stability": "experimental",
// "value": "out"}]}}
namespace SystemPagingDirectionValues
{
/**
 * in.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "in", "note": none, "stability": "experimental",
// "value": "in"}
static constexpr const char *kIn = "in";
/**
 * out.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "out", "note": none, "stability":
// "experimental", "value": "out"}
static constexpr const char *kOut = "out";
}  // namespace SystemPagingDirectionValues

// DEBUG: {"brief": "The memory paging state", "examples": ["free"], "name": "system.paging.state",
// "requirement_level": "recommended", "root_namespace": "system", "stability": "experimental",
// "type": {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id":
// "used", "note": none, "stability": "experimental", "value": "used"}, {"brief": none,
// "deprecated": none, "id": "free", "note": none, "stability": "experimental", "value": "free"}]}}
namespace SystemPagingStateValues
{
/**
 * used.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "used", "note": none, "stability":
// "experimental", "value": "used"}
static constexpr const char *kUsed = "used";
/**
 * free.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "free", "note": none, "stability":
// "experimental", "value": "free"}
static constexpr const char *kFree = "free";
}  // namespace SystemPagingStateValues

// DEBUG: {"brief": "The memory paging type", "examples": ["minor"], "name": "system.paging.type",
// "requirement_level": "recommended", "root_namespace": "system", "stability": "experimental",
// "type": {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id":
// "major", "note": none, "stability": "experimental", "value": "major"}, {"brief": none,
// "deprecated": none, "id": "minor", "note": none, "stability": "experimental", "value":
// "minor"}]}}
namespace SystemPagingTypeValues
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
}  // namespace SystemPagingTypeValues

// DEBUG: {"brief": "The process state, e.g., [Linux Process State
// Codes](https://man7.org/linux/man-pages/man1/ps.1.html#PROCESS_STATE_CODES)\n", "examples":
// ["running"], "name": "system.process.status", "requirement_level": "recommended",
// "root_namespace": "system", "stability": "experimental", "type": {"allow_custom_values": true,
// "members": [{"brief": none, "deprecated": none, "id": "running", "note": none, "stability":
// "experimental", "value": "running"}, {"brief": none, "deprecated": none, "id": "sleeping",
// "note": none, "stability": "experimental", "value": "sleeping"}, {"brief": none, "deprecated":
// none, "id": "stopped", "note": none, "stability": "experimental", "value": "stopped"}, {"brief":
// none, "deprecated": none, "id": "defunct", "note": none, "stability": "experimental", "value":
// "defunct"}]}}
namespace SystemProcessStatusValues
{
/**
 * running.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "running", "note": none, "stability":
// "experimental", "value": "running"}
static constexpr const char *kRunning = "running";
/**
 * sleeping.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "sleeping", "note": none, "stability":
// "experimental", "value": "sleeping"}
static constexpr const char *kSleeping = "sleeping";
/**
 * stopped.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "stopped", "note": none, "stability":
// "experimental", "value": "stopped"}
static constexpr const char *kStopped = "stopped";
/**
 * defunct.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "defunct", "note": none, "stability":
// "experimental", "value": "defunct"}
static constexpr const char *kDefunct = "defunct";
}  // namespace SystemProcessStatusValues

// @deprecated(reason="The attribute system.processes.status is deprecated - Replaced by
// `system.process.status`")  # type: ignore DEBUG: {"brief": "Deprecated, use
// `system.process.status` instead.", "deprecated": "Replaced by `system.process.status`.",
// "examples": ["running"], "name": "system.processes.status", "requirement_level": "recommended",
// "root_namespace": "system", "stability": "experimental", "type": {"allow_custom_values": true,
// "members": [{"brief": none, "deprecated": none, "id": "running", "note": none, "stability":
// "experimental", "value": "running"}, {"brief": none, "deprecated": none, "id": "sleeping",
// "note": none, "stability": "experimental", "value": "sleeping"}, {"brief": none, "deprecated":
// none, "id": "stopped", "note": none, "stability": "experimental", "value": "stopped"}, {"brief":
// none, "deprecated": none, "id": "defunct", "note": none, "stability": "experimental", "value":
// "defunct"}]}}
namespace SystemProcessesStatusValues
{
/**
 * running.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "running", "note": none, "stability":
// "experimental", "value": "running"}
static constexpr const char *kRunning = "running";
/**
 * sleeping.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "sleeping", "note": none, "stability":
// "experimental", "value": "sleeping"}
static constexpr const char *kSleeping = "sleeping";
/**
 * stopped.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "stopped", "note": none, "stability":
// "experimental", "value": "stopped"}
static constexpr const char *kStopped = "stopped";
/**
 * defunct.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "defunct", "note": none, "stability":
// "experimental", "value": "defunct"}
static constexpr const char *kDefunct = "defunct";
}  // namespace SystemProcessesStatusValues

}  // namespace system
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
