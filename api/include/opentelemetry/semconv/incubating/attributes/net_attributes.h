

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
namespace net
{

/**
 * @Deprecated: Replaced by @code network.local.address @endcode.
 */
static const char *kNetHostIp = "net.host.ip";

/**
 * @Deprecated: Replaced by @code server.address @endcode.
 */
static const char *kNetHostName = "net.host.name";

/**
 * @Deprecated: Replaced by @code server.port @endcode.
 */
static const char *kNetHostPort = "net.host.port";

/**
 * @Deprecated: Replaced by @code network.peer.address @endcode.
 */
static const char *kNetPeerIp = "net.peer.ip";

/**
 * @Deprecated: Replaced by @code server.address @endcode on client spans and @code client.address
 * @endcode on server spans.
 */
static const char *kNetPeerName = "net.peer.name";

/**
 * @Deprecated: Replaced by @code server.port @endcode on client spans and @code client.port
 * @endcode on server spans.
 */
static const char *kNetPeerPort = "net.peer.port";

/**
 * @Deprecated: Replaced by @code network.protocol.name @endcode.
 */
static const char *kNetProtocolName = "net.protocol.name";

/**
 * @Deprecated: Replaced by @code network.protocol.version @endcode.
 */
static const char *kNetProtocolVersion = "net.protocol.version";

/**
 * @Deprecated: Split to @code network.transport @endcode and @code network.type @endcode.
 */
static const char *kNetSockFamily = "net.sock.family";

/**
 * @Deprecated: Replaced by @code network.local.address @endcode.
 */
static const char *kNetSockHostAddr = "net.sock.host.addr";

/**
 * @Deprecated: Replaced by @code network.local.port @endcode.
 */
static const char *kNetSockHostPort = "net.sock.host.port";

/**
 * @Deprecated: Replaced by @code network.peer.address @endcode.
 */
static const char *kNetSockPeerAddr = "net.sock.peer.addr";

/**
 * @Deprecated: Removed.
 */
static const char *kNetSockPeerName = "net.sock.peer.name";

/**
 * @Deprecated: Replaced by @code network.peer.port @endcode.
 */
static const char *kNetSockPeerPort = "net.sock.peer.port";

/**
 * @Deprecated: Replaced by @code network.transport @endcode.
 */
static const char *kNetTransport = "net.transport";

// @deprecated(reason="The attribute net.sock.family is deprecated - Split to `network.transport`
// and `network.type`")  # type: ignore DEBUG: {"brief": "Deprecated, use `network.transport` and
// `network.type`.", "deprecated": "Split to `network.transport` and `network.type`.", "name":
// "net.sock.family", "requirement_level": "recommended", "root_namespace": "net", "stability":
// "experimental", "type": {"allow_custom_values": true, "members": [{"brief": "IPv4 address",
// "deprecated": none, "id": "inet", "note": none, "stability": "experimental", "value": "inet"},
// {"brief": "IPv6 address", "deprecated": none, "id": "inet6", "note": none, "stability":
// "experimental", "value": "inet6"}, {"brief": "Unix domain socket path", "deprecated": none, "id":
// "unix", "note": none, "stability": "experimental", "value": "unix"}]}}
namespace NetSockFamilyValues
{
/**
 * IPv4 address.
 */
// DEBUG: {"brief": "IPv4 address", "deprecated": none, "id": "inet", "note": none, "stability":
// "experimental", "value": "inet"}
static constexpr const char *kInet = "inet";
/**
 * IPv6 address.
 */
// DEBUG: {"brief": "IPv6 address", "deprecated": none, "id": "inet6", "note": none, "stability":
// "experimental", "value": "inet6"}
static constexpr const char *kInet6 = "inet6";
/**
 * Unix domain socket path.
 */
// DEBUG: {"brief": "Unix domain socket path", "deprecated": none, "id": "unix", "note": none,
// "stability": "experimental", "value": "unix"}
static constexpr const char *kUnix = "unix";
}  // namespace NetSockFamilyValues

// @deprecated(reason="The attribute net.transport is deprecated - Replaced by `network.transport`")
// # type: ignore DEBUG: {"brief": "Deprecated, use `network.transport`.", "deprecated": "Replaced
// by `network.transport`.", "name": "net.transport", "requirement_level": "recommended",
// "root_namespace": "net", "stability": "experimental", "type": {"allow_custom_values": true,
// "members": [{"brief": none, "deprecated": none, "id": "ip_tcp", "note": none, "stability":
// "experimental", "value": "ip_tcp"}, {"brief": none, "deprecated": none, "id": "ip_udp", "note":
// none, "stability": "experimental", "value": "ip_udp"}, {"brief": "Named or anonymous pipe.",
// "deprecated": none, "id": "pipe", "note": none, "stability": "experimental", "value": "pipe"},
// {"brief": "In-process communication.", "deprecated": none, "id": "inproc", "note": "Signals that
// there is only in-process communication not using a \"real\" network protocol in cases where
// network attributes would normally be expected. Usually all other network attributes can be left
// out in that case.\n", "stability": "experimental", "value": "inproc"}, {"brief": "Something else
// (non IP-based).", "deprecated": none, "id": "other", "note": none, "stability": "experimental",
// "value": "other"}]}}
namespace NetTransportValues
{
/**
 * ip_tcp.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "ip_tcp", "note": none, "stability":
// "experimental", "value": "ip_tcp"}
static constexpr const char *kIpTcp = "ip_tcp";
/**
 * ip_udp.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "ip_udp", "note": none, "stability":
// "experimental", "value": "ip_udp"}
static constexpr const char *kIpUdp = "ip_udp";
/**
 * Named or anonymous pipe.
 */
// DEBUG: {"brief": "Named or anonymous pipe.", "deprecated": none, "id": "pipe", "note": none,
// "stability": "experimental", "value": "pipe"}
static constexpr const char *kPipe = "pipe";
/**
 * In-process communication.
 */
// DEBUG: {"brief": "In-process communication.", "deprecated": none, "id": "inproc", "note":
// "Signals that there is only in-process communication not using a \"real\" network protocol in
// cases where network attributes would normally be expected. Usually all other network attributes
// can be left out in that case.\n", "stability": "experimental", "value": "inproc"}
static constexpr const char *kInproc = "inproc";
/**
 * Something else (non IP-based).
 */
// DEBUG: {"brief": "Something else (non IP-based).", "deprecated": none, "id": "other", "note":
// none, "stability": "experimental", "value": "other"}
static constexpr const char *kOther = "other";
}  // namespace NetTransportValues

}  // namespace net
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
