

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
namespace os
{

/**
 * Unique identifier for a particular build or compilation of the operating system.
 */
static const char *kOsBuildId = "os.build_id";

/**
 * Human readable (not intended to be parsed) OS version information, like e.g. reported by @code
 * ver @endcode or @code lsb_release -a @endcode commands.
 */
static const char *kOsDescription = "os.description";

/**
 * Human readable operating system name.
 */
static const char *kOsName = "os.name";

/**
 * The operating system type.
 */
static const char *kOsType = "os.type";

/**
 * The version string of the operating system as defined in <a
 * href="/docs/resource/README.md#version-attributes">Version Attributes</a>.
 */
static const char *kOsVersion = "os.version";

// DEBUG: {"brief": "The operating system type.\n", "name": "os.type", "requirement_level":
// "recommended", "root_namespace": "os", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": "Microsoft Windows", "deprecated": none,
// "id": "windows", "note": none, "stability": "experimental", "value": "windows"}, {"brief":
// "Linux", "deprecated": none, "id": "linux", "note": none, "stability": "experimental", "value":
// "linux"}, {"brief": "Apple Darwin", "deprecated": none, "id": "darwin", "note": none,
// "stability": "experimental", "value": "darwin"}, {"brief": "FreeBSD", "deprecated": none, "id":
// "freebsd", "note": none, "stability": "experimental", "value": "freebsd"}, {"brief": "NetBSD",
// "deprecated": none, "id": "netbsd", "note": none, "stability": "experimental", "value":
// "netbsd"}, {"brief": "OpenBSD", "deprecated": none, "id": "openbsd", "note": none, "stability":
// "experimental", "value": "openbsd"}, {"brief": "DragonFly BSD", "deprecated": none, "id":
// "dragonflybsd", "note": none, "stability": "experimental", "value": "dragonflybsd"}, {"brief":
// "HP-UX (Hewlett Packard Unix)", "deprecated": none, "id": "hpux", "note": none, "stability":
// "experimental", "value": "hpux"}, {"brief": "AIX (Advanced Interactive eXecutive)", "deprecated":
// none, "id": "aix", "note": none, "stability": "experimental", "value": "aix"}, {"brief": "SunOS,
// Oracle Solaris", "deprecated": none, "id": "solaris", "note": none, "stability": "experimental",
// "value": "solaris"}, {"brief": "IBM z/OS", "deprecated": none, "id": "z_os", "note": none,
// "stability": "experimental", "value": "z_os"}]}}
namespace OsTypeValues
{
/**
 * Microsoft Windows.
 */
// DEBUG: {"brief": "Microsoft Windows", "deprecated": none, "id": "windows", "note": none,
// "stability": "experimental", "value": "windows"}
static constexpr const char *kWindows = "windows";
/**
 * Linux.
 */
// DEBUG: {"brief": "Linux", "deprecated": none, "id": "linux", "note": none, "stability":
// "experimental", "value": "linux"}
static constexpr const char *kLinux = "linux";
/**
 * Apple Darwin.
 */
// DEBUG: {"brief": "Apple Darwin", "deprecated": none, "id": "darwin", "note": none, "stability":
// "experimental", "value": "darwin"}
static constexpr const char *kDarwin = "darwin";
/**
 * FreeBSD.
 */
// DEBUG: {"brief": "FreeBSD", "deprecated": none, "id": "freebsd", "note": none, "stability":
// "experimental", "value": "freebsd"}
static constexpr const char *kFreebsd = "freebsd";
/**
 * NetBSD.
 */
// DEBUG: {"brief": "NetBSD", "deprecated": none, "id": "netbsd", "note": none, "stability":
// "experimental", "value": "netbsd"}
static constexpr const char *kNetbsd = "netbsd";
/**
 * OpenBSD.
 */
// DEBUG: {"brief": "OpenBSD", "deprecated": none, "id": "openbsd", "note": none, "stability":
// "experimental", "value": "openbsd"}
static constexpr const char *kOpenbsd = "openbsd";
/**
 * DragonFly BSD.
 */
// DEBUG: {"brief": "DragonFly BSD", "deprecated": none, "id": "dragonflybsd", "note": none,
// "stability": "experimental", "value": "dragonflybsd"}
static constexpr const char *kDragonflybsd = "dragonflybsd";
/**
 * HP-UX (Hewlett Packard Unix).
 */
// DEBUG: {"brief": "HP-UX (Hewlett Packard Unix)", "deprecated": none, "id": "hpux", "note": none,
// "stability": "experimental", "value": "hpux"}
static constexpr const char *kHpux = "hpux";
/**
 * AIX (Advanced Interactive eXecutive).
 */
// DEBUG: {"brief": "AIX (Advanced Interactive eXecutive)", "deprecated": none, "id": "aix", "note":
// none, "stability": "experimental", "value": "aix"}
static constexpr const char *kAix = "aix";
/**
 * SunOS, Oracle Solaris.
 */
// DEBUG: {"brief": "SunOS, Oracle Solaris", "deprecated": none, "id": "solaris", "note": none,
// "stability": "experimental", "value": "solaris"}
static constexpr const char *kSolaris = "solaris";
/**
 * IBM z/OS.
 */
// DEBUG: {"brief": "IBM z/OS", "deprecated": none, "id": "z_os", "note": none, "stability":
// "experimental", "value": "z_os"}
static constexpr const char *kZOs = "z_os";
}  // namespace OsTypeValues

}  // namespace os
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
