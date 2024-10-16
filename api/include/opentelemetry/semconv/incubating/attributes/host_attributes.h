

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
namespace host
{

/**
 * The CPU architecture the host system is running on.
 */
static const char *kHostArch = "host.arch";

/**
 * The amount of level 2 memory cache available to the processor (in Bytes).
 */
static const char *kHostCpuCacheL2Size = "host.cpu.cache.l2.size";

/**
 * Family or generation of the CPU.
 */
static const char *kHostCpuFamily = "host.cpu.family";

/**
 * Model identifier. It provides more granular information about the CPU, distinguishing it from
 * other CPUs within the same family.
 */
static const char *kHostCpuModelId = "host.cpu.model.id";

/**
 * Model designation of the processor.
 */
static const char *kHostCpuModelName = "host.cpu.model.name";

/**
 * Stepping or core revisions.
 */
static const char *kHostCpuStepping = "host.cpu.stepping";

/**
 * Processor manufacturer identifier. A maximum 12-character string.
 * Note: <a href="https://wiki.osdev.org/CPUID">CPUID</a> command returns the vendor ID string in
 * EBX, EDX and ECX registers. Writing these to memory in this order results in a 12-character
 * string.
 */
static const char *kHostCpuVendorId = "host.cpu.vendor.id";

/**
 * Unique host ID. For Cloud, this must be the instance_id assigned by the cloud provider. For
 * non-containerized systems, this should be the @code machine-id @endcode. See the table below for
 * the sources to use to determine the @code machine-id @endcode based on operating system.
 */
static const char *kHostId = "host.id";

/**
 * VM image ID or host OS image ID. For Cloud, this value is from the provider.
 */
static const char *kHostImageId = "host.image.id";

/**
 * Name of the VM image or OS install the host was instantiated from.
 */
static const char *kHostImageName = "host.image.name";

/**
 * The version string of the VM image or host OS as defined in <a
 * href="/docs/resource/README.md#version-attributes">Version Attributes</a>.
 */
static const char *kHostImageVersion = "host.image.version";

/**
 * Available IP addresses of the host, excluding loopback interfaces.
 * Note: IPv4 Addresses MUST be specified in dotted-quad notation. IPv6 addresses MUST be specified
 * in the <a href="https://www.rfc-editor.org/rfc/rfc5952.html">RFC 5952</a> format.
 */
static const char *kHostIp = "host.ip";

/**
 * Available MAC addresses of the host, excluding loopback interfaces.
 * Note: MAC Addresses MUST be represented in <a
 * href="https://standards.ieee.org/wp-content/uploads/import/documents/tutorials/eui.pdf">IEEE RA
 * hexadecimal form</a>: as hyphen-separated octets in uppercase hexadecimal form from most to least
 * significant.
 */
static const char *kHostMac = "host.mac";

/**
 * Name of the host. On Unix systems, it may contain what the hostname command returns, or the fully
 * qualified hostname, or another name specified by the user.
 */
static const char *kHostName = "host.name";

/**
 * Type of host. For Cloud, this must be the machine type.
 */
static const char *kHostType = "host.type";

// DEBUG: {"brief": "The CPU architecture the host system is running on.\n", "name": "host.arch",
// "requirement_level": "recommended", "root_namespace": "host", "stability": "experimental",
// "type": {"allow_custom_values": true, "members": [{"brief": "AMD64", "deprecated": none, "id":
// "amd64", "note": none, "stability": "experimental", "value": "amd64"}, {"brief": "ARM32",
// "deprecated": none, "id": "arm32", "note": none, "stability": "experimental", "value": "arm32"},
// {"brief": "ARM64", "deprecated": none, "id": "arm64", "note": none, "stability": "experimental",
// "value": "arm64"}, {"brief": "Itanium", "deprecated": none, "id": "ia64", "note": none,
// "stability": "experimental", "value": "ia64"}, {"brief": "32-bit PowerPC", "deprecated": none,
// "id": "ppc32", "note": none, "stability": "experimental", "value": "ppc32"}, {"brief": "64-bit
// PowerPC", "deprecated": none, "id": "ppc64", "note": none, "stability": "experimental", "value":
// "ppc64"}, {"brief": "IBM z/Architecture", "deprecated": none, "id": "s390x", "note": none,
// "stability": "experimental", "value": "s390x"}, {"brief": "32-bit x86", "deprecated": none, "id":
// "x86", "note": none, "stability": "experimental", "value": "x86"}]}}
namespace HostArchValues
{
/**
 * AMD64.
 */
// DEBUG: {"brief": "AMD64", "deprecated": none, "id": "amd64", "note": none, "stability":
// "experimental", "value": "amd64"}
static constexpr const char *kAmd64 = "amd64";
/**
 * ARM32.
 */
// DEBUG: {"brief": "ARM32", "deprecated": none, "id": "arm32", "note": none, "stability":
// "experimental", "value": "arm32"}
static constexpr const char *kArm32 = "arm32";
/**
 * ARM64.
 */
// DEBUG: {"brief": "ARM64", "deprecated": none, "id": "arm64", "note": none, "stability":
// "experimental", "value": "arm64"}
static constexpr const char *kArm64 = "arm64";
/**
 * Itanium.
 */
// DEBUG: {"brief": "Itanium", "deprecated": none, "id": "ia64", "note": none, "stability":
// "experimental", "value": "ia64"}
static constexpr const char *kIa64 = "ia64";
/**
 * 32-bit PowerPC.
 */
// DEBUG: {"brief": "32-bit PowerPC", "deprecated": none, "id": "ppc32", "note": none, "stability":
// "experimental", "value": "ppc32"}
static constexpr const char *kPpc32 = "ppc32";
/**
 * 64-bit PowerPC.
 */
// DEBUG: {"brief": "64-bit PowerPC", "deprecated": none, "id": "ppc64", "note": none, "stability":
// "experimental", "value": "ppc64"}
static constexpr const char *kPpc64 = "ppc64";
/**
 * IBM z/Architecture.
 */
// DEBUG: {"brief": "IBM z/Architecture", "deprecated": none, "id": "s390x", "note": none,
// "stability": "experimental", "value": "s390x"}
static constexpr const char *kS390x = "s390x";
/**
 * 32-bit x86.
 */
// DEBUG: {"brief": "32-bit x86", "deprecated": none, "id": "x86", "note": none, "stability":
// "experimental", "value": "x86"}
static constexpr const char *kX86 = "x86";
}  // namespace HostArchValues

}  // namespace host
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
