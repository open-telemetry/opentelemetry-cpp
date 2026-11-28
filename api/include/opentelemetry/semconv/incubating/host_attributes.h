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
  The CPU architecture the host system is running on.
 */
static constexpr const char *kHostArch = "host.arch";

/**
  The amount of level 2 memory cache available to the processor (in Bytes).
 */
static constexpr const char *kHostCpuCacheL2Size = "host.cpu.cache.l2.size";

/**
  Family or generation of the CPU.
 */
static constexpr const char *kHostCpuFamily = "host.cpu.family";

/**
  Model identifier. It provides more granular information about the CPU, distinguishing it from
  other CPUs within the same family.
 */
static constexpr const char *kHostCpuModelId = "host.cpu.model.id";

/**
  Model designation of the processor.
 */
static constexpr const char *kHostCpuModelName = "host.cpu.model.name";

/**
  Stepping or core revisions.
 */
static constexpr const char *kHostCpuStepping = "host.cpu.stepping";

/**
  Processor manufacturer identifier. A maximum 12-character string.
  <p>
  <a href="https://wiki.osdev.org/CPUID">CPUID</a> command returns the vendor ID string in EBX, EDX
  and ECX registers. Writing these to memory in this order results in a 12-character string.
 */
static constexpr const char *kHostCpuVendorId = "host.cpu.vendor.id";

/**
  Unique host ID, determined by the first matching source in the priority order.
  <p>
  SHOULD be determined using the first available source from the following priority tiers:
  <ol>
    <li>Known cloud vendors - cloud instance ID</li>
    <li>Known VM vendors - provider VM ID</li>
    <li>OS-dependent machine ID</li>
    <li>User-provided value</li>
  </ol>
  <p>
  The value SHOULD conform to the following properties:
  <ul>
    <li><strong>Unique</strong> within a given context;</li>
    <li><strong><a
  href="https://opentelemetry.io/docs/specs/otel/entities/data-model/#repeatable-identity">Repeatable</a></strong>
  - multiple observers on the same host come up with the same value;</li>
    <li><strong>Persistent</strong> - survives host restarts and software updates.</li>
  </ul>
  <p>
  <strong>Collecting @code host.id @endcode From Known Environments:</strong> When @code host.id
  @endcode is derived from a cloud vendor instance ID, the resource SHOULD also carry the
  corresponding
  @code cloud.provider @endcode and @code cloud.platform @endcode attributes so that the source of
  the @code host.id @endcode value can be unambiguously determined. <p> The following cloud
  environments are currently recognized: <p> | Environment | @code host.id @endcode source | @code
  cloud.provider @endcode | @code cloud.platform @endcode | | --- | --- | --- | --- | | AWS EC2 |
  EC2 instance ID | @code aws @endcode | @code aws_ec2 @endcode | | AWS EKS (node) | EC2 instance ID
  | @code aws @endcode | @code aws_eks @endcode | | Azure VM | @code vmId @endcode | @code azure
  @endcode | @code azure.vm @endcode | | GCP Compute Engine | GCE instance ID | @code gcp @endcode |
  @code gcp_compute_engine @endcode | | GCP GKE (node) | GCE instance ID | @code gcp @endcode |
  @code gcp_kubernetes_engine @endcode | <p> The listed @code cloud.provider @endcode/@code
  cloud.platform @endcode values are for example only. Refer to the <a
  href="/docs/registry/attributes/cloud.md">@code cloud @endcode</a> attribute definitions for the
  relevant set of values. <p> <strong>Non-privileged Machine ID Lookup:</strong> When collecting
  @code host.id @endcode for non-containerized systems, non-privileged lookups of the machine ID are
  preferred. SDK detector implementations MUST use the sources listed below to obtain the machine
  ID. <p> | OS | Primary | Fallback | | --- | --- | --- | | Linux | contents of @code
  /etc/machine-id @endcode | contents of @code /var/lib/dbus/machine-id @endcode | | BSD | contents
  of @code /etc/hostid @endcode | output of @code /bin/kenv -q smbios.system.uuid @endcode | | macOS
  | @code IOPlatformUUID @endcode line from the output of @code /usr/sbin/ioreg -rd1 -c
  "IOPlatformExpertDevice" @endcode | - | | Windows | @code MachineGuid @endcode from registry @code
  HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography @endcode | - | <p> <strong>Privileged Machine
  ID Lookup:</strong> The @code host.id @endcode can be looked up using privileged sources. For
  example, Linux systems can use the output of @code dmidecode -t system @endcode,
  @code dmidecode -t baseboard @endcode,
  @code dmidecode -t chassis @endcode, or read the corresponding data from the filesystem
  (e.g. @code cat /sys/devices/virtual/dmi/id/product_id @endcode,
  @code cat /sys/devices/virtual/dmi/id/product_uuid @endcode, etc), however, SDK resource
  detector implementations MUST NOT collect @code host.id @endcode from privileged sources. If
  privileged lookup of @code host.id @endcode is required, the value SHOULD be injected via the
  @code OTEL_RESOURCE_ATTRIBUTES @endcode environment variable.
  <p>
  <strong>Collecting @code host.id @endcode From User Configuration:</strong> If none of the above
  sources yields a non-empty value, the implementation MAY be unable to determine @code host.id
  @endcode automatically and SHOULD omit the attribute. In that case, the @code host.id @endcode can
  only be provided through user configuration or other available manual options. Users SHOULD ensure
  that the @code host.id @endcode value conforms to the properties stated above.
 */
static constexpr const char *kHostId = "host.id";

/**
  VM image ID or host OS image ID. For Cloud, this value is from the provider.
 */
static constexpr const char *kHostImageId = "host.image.id";

/**
  Name of the VM image or OS install the host was instantiated from.
 */
static constexpr const char *kHostImageName = "host.image.name";

/**
  The version string of the VM image or host OS as defined in <a
  href="/docs/resource/README.md#version-attributes">Version Attributes</a>.
 */
static constexpr const char *kHostImageVersion = "host.image.version";

/**
  Available IP addresses of the host, excluding loopback interfaces.
  <p>
  IPv4 Addresses MUST be specified in dotted-quad notation. IPv6 addresses MUST be specified in the
  <a href="https://www.rfc-editor.org/rfc/rfc5952.html">RFC 5952</a> format.
 */
static constexpr const char *kHostIp = "host.ip";

/**
  Available MAC addresses of the host, excluding loopback interfaces.
  <p>
  MAC Addresses MUST be represented in <a
  href="https://standards.ieee.org/wp-content/uploads/import/documents/tutorials/eui.pdf">IEEE RA
  hexadecimal form</a>: as hyphen-separated octets in uppercase hexadecimal form from most to least
  significant.
 */
static constexpr const char *kHostMac = "host.mac";

/**
  Name of the host. On UNIX systems, it may contain what the hostname command returns, or the fully
  qualified hostname, or another name specified by the user.
 */
static constexpr const char *kHostName = "host.name";

/**
  Type of host. For Cloud, this must be the machine type.
 */
static constexpr const char *kHostType = "host.type";

namespace HostArchValues
{
/**
  AMD64
 */
static constexpr const char *kAmd64 = "amd64";

/**
  ARM32
 */
static constexpr const char *kArm32 = "arm32";

/**
  ARM64
 */
static constexpr const char *kArm64 = "arm64";

/**
  Itanium
 */
static constexpr const char *kIa64 = "ia64";

/**
  32-bit PowerPC
 */
static constexpr const char *kPpc32 = "ppc32";

/**
  64-bit PowerPC
 */
static constexpr const char *kPpc64 = "ppc64";

/**
  IBM z/Architecture
 */
static constexpr const char *kS390x = "s390x";

/**
  32-bit x86
 */
static constexpr const char *kX86 = "x86";

}  // namespace HostArchValues

}  // namespace host
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
