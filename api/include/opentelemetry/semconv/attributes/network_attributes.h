

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
namespace network
{

/**
 * Local address of the network connection - IP address or Unix domain socket name.
 */
static const char *kNetworkLocalAddress = "network.local.address";

/**
 * Local port number of the network connection.
 */
static const char *kNetworkLocalPort = "network.local.port";

/**
 * Peer address of the network connection - IP address or Unix domain socket name.
 */
static const char *kNetworkPeerAddress = "network.peer.address";

/**
 * Peer port number of the network connection.
 */
static const char *kNetworkPeerPort = "network.peer.port";

/**
 * <a href="https://osi-model.com/application-layer/">OSI application layer</a> or non-OSI
 * equivalent. Note: The value SHOULD be normalized to lowercase.
 */
static const char *kNetworkProtocolName = "network.protocol.name";

/**
 * The actual version of the protocol used for network communication.
 * Note: If protocol version is subject to negotiation (for example using <a
 * href="https://www.rfc-editor.org/rfc/rfc7301.html">ALPN</a>), this attribute SHOULD be set to the
 * negotiated version. If the actual protocol version is not known, this attribute SHOULD NOT be
 * set.
 */
static const char *kNetworkProtocolVersion = "network.protocol.version";

/**
 * <a href="https://osi-model.com/transport-layer/">OSI transport layer</a> or <a
 * href="https://wikipedia.org/wiki/Inter-process_communication">inter-process communication
 * method</a>. Note: The value SHOULD be normalized to lowercase. <p> Consider always setting the
 * transport when setting a port number, since a port number is ambiguous without knowing the
 * transport. For example different processes could be listening on TCP port 12345 and UDP port
 * 12345.
 */
static const char *kNetworkTransport = "network.transport";

/**
 * <a href="https://osi-model.com/network-layer/">OSI network layer</a> or non-OSI equivalent.
 * Note: The value SHOULD be normalized to lowercase.
 */
static const char *kNetworkType = "network.type";

// DEBUG: {"brief": "[OSI transport layer](https://osi-model.com/transport-layer/) or [inter-process
// communication method](https://wikipedia.org/wiki/Inter-process_communication).\n", "examples":
// ["tcp", "udp"], "name": "network.transport", "note": "The value SHOULD be normalized to
// lowercase.\n\nConsider always setting the transport when setting a port number, since\na port
// number is ambiguous without knowing the transport. For example\ndifferent processes could be
// listening on TCP port 12345 and UDP port 12345.\n", "requirement_level": "recommended",
// "root_namespace": "network", "stability": "stable", "type": {"allow_custom_values": true,
// "members": [{"brief": "TCP", "deprecated": none, "id": "tcp", "note": none, "stability":
// "stable", "value": "tcp"}, {"brief": "UDP", "deprecated": none, "id": "udp", "note": none,
// "stability": "stable", "value": "udp"}, {"brief": "Named or anonymous pipe.", "deprecated": none,
// "id": "pipe", "note": none, "stability": "stable", "value": "pipe"}, {"brief": "Unix domain
// socket", "deprecated": none, "id": "unix", "note": none, "stability": "stable", "value": "unix"},
// {"brief": "QUIC", "deprecated": none, "id": "quic", "note": none, "stability": "experimental",
// "value": "quic"}]}}
namespace NetworkTransportValues
{
/**
 * TCP.
 */
// DEBUG: {"brief": "TCP", "deprecated": none, "id": "tcp", "note": none, "stability": "stable",
// "value": "tcp"}
static constexpr const char *kTcp = "tcp";
/**
 * UDP.
 */
// DEBUG: {"brief": "UDP", "deprecated": none, "id": "udp", "note": none, "stability": "stable",
// "value": "udp"}
static constexpr const char *kUdp = "udp";
/**
 * Named or anonymous pipe.
 */
// DEBUG: {"brief": "Named or anonymous pipe.", "deprecated": none, "id": "pipe", "note": none,
// "stability": "stable", "value": "pipe"}
static constexpr const char *kPipe = "pipe";
/**
 * Unix domain socket.
 */
// DEBUG: {"brief": "Unix domain socket", "deprecated": none, "id": "unix", "note": none,
// "stability": "stable", "value": "unix"}
static constexpr const char *kUnix = "unix";
/**
 * QUIC.
 */
// DEBUG: {"brief": "QUIC", "deprecated": none, "id": "quic", "note": none, "stability":
// "experimental", "value": "quic"}
static constexpr const char *kQuic = "quic";
}  // namespace NetworkTransportValues

// DEBUG: {"brief": "[OSI network layer](https://osi-model.com/network-layer/) or non-OSI
// equivalent.", "examples": ["ipv4", "ipv6"], "name": "network.type", "note": "The value SHOULD be
// normalized to lowercase.", "requirement_level": "recommended", "root_namespace": "network",
// "stability": "stable", "type": {"allow_custom_values": true, "members": [{"brief": "IPv4",
// "deprecated": none, "id": "ipv4", "note": none, "stability": "stable", "value": "ipv4"},
// {"brief": "IPv6", "deprecated": none, "id": "ipv6", "note": none, "stability": "stable", "value":
// "ipv6"}]}}
namespace NetworkTypeValues
{
/**
 * IPv4.
 */
// DEBUG: {"brief": "IPv4", "deprecated": none, "id": "ipv4", "note": none, "stability": "stable",
// "value": "ipv4"}
static constexpr const char *kIpv4 = "ipv4";
/**
 * IPv6.
 */
// DEBUG: {"brief": "IPv6", "deprecated": none, "id": "ipv6", "note": none, "stability": "stable",
// "value": "ipv6"}
static constexpr const char *kIpv6 = "ipv6";
}  // namespace NetworkTypeValues

}  // namespace network
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
