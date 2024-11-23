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
 * The ISO 3166-1 alpha-2 2-character country code associated with the mobile carrier network.
 */
static constexpr const char *kNetworkCarrierIcc = "network.carrier.icc";

/**
 * The mobile carrier country code.
 */
static constexpr const char *kNetworkCarrierMcc = "network.carrier.mcc";

/**
 * The mobile carrier network code.
 */
static constexpr const char *kNetworkCarrierMnc = "network.carrier.mnc";

/**
 * The name of the mobile carrier.
 */
static constexpr const char *kNetworkCarrierName = "network.carrier.name";

/**
 * This describes more details regarding the connection.type. It may be the type of cell technology
 * connection, but it could be used for describing details about a wifi connection.
 */
static constexpr const char *kNetworkConnectionSubtype = "network.connection.subtype";

/**
 * The internet connection type.
 */
static constexpr const char *kNetworkConnectionType = "network.connection.type";

/**
 * The network IO operation direction.
 */
static constexpr const char *kNetworkIoDirection = "network.io.direction";

/**
 * Local address of the network connection - IP address or Unix domain socket name.
 */
static constexpr const char *kNetworkLocalAddress = "network.local.address";

/**
 * Local port number of the network connection.
 */
static constexpr const char *kNetworkLocalPort = "network.local.port";

/**
 * Peer address of the network connection - IP address or Unix domain socket name.
 */
static constexpr const char *kNetworkPeerAddress = "network.peer.address";

/**
 * Peer port number of the network connection.
 */
static constexpr const char *kNetworkPeerPort = "network.peer.port";

/**
 * <a href="https://osi-model.com/application-layer/">OSI application layer</a> or non-OSI
 * equivalent. <p> The value SHOULD be normalized to lowercase.
 */
static constexpr const char *kNetworkProtocolName = "network.protocol.name";

/**
 * The actual version of the protocol used for network communication.
 * <p>
 * If protocol version is subject to negotiation (for example using <a
 * href="https://www.rfc-editor.org/rfc/rfc7301.html">ALPN</a>), this attribute SHOULD be set to the
 * negotiated version. If the actual protocol version is not known, this attribute SHOULD NOT be
 * set.
 */
static constexpr const char *kNetworkProtocolVersion = "network.protocol.version";

/**
 * <a href="https://osi-model.com/transport-layer/">OSI transport layer</a> or <a
 * href="https://wikipedia.org/wiki/Inter-process_communication">inter-process communication
 * method</a>. <p> The value SHOULD be normalized to lowercase. <p> Consider always setting the
 * transport when setting a port number, since a port number is ambiguous without knowing the
 * transport. For example different processes could be listening on TCP port 12345 and UDP port
 * 12345.
 */
static constexpr const char *kNetworkTransport = "network.transport";

/**
 * <a href="https://osi-model.com/network-layer/">OSI network layer</a> or non-OSI equivalent.
 * <p>
 * The value SHOULD be normalized to lowercase.
 */
static constexpr const char *kNetworkType = "network.type";

namespace NetworkConnectionSubtypeValues
{
/**
 * GPRS
 */
static constexpr const char *kGprs = "gprs";

/**
 * EDGE
 */
static constexpr const char *kEdge = "edge";

/**
 * UMTS
 */
static constexpr const char *kUmts = "umts";

/**
 * CDMA
 */
static constexpr const char *kCdma = "cdma";

/**
 * EVDO Rel. 0
 */
static constexpr const char *kEvdo0 = "evdo_0";

/**
 * EVDO Rev. A
 */
static constexpr const char *kEvdoA = "evdo_a";

/**
 * CDMA2000 1XRTT
 */
static constexpr const char *kCdma20001xrtt = "cdma2000_1xrtt";

/**
 * HSDPA
 */
static constexpr const char *kHsdpa = "hsdpa";

/**
 * HSUPA
 */
static constexpr const char *kHsupa = "hsupa";

/**
 * HSPA
 */
static constexpr const char *kHspa = "hspa";

/**
 * IDEN
 */
static constexpr const char *kIden = "iden";

/**
 * EVDO Rev. B
 */
static constexpr const char *kEvdoB = "evdo_b";

/**
 * LTE
 */
static constexpr const char *kLte = "lte";

/**
 * EHRPD
 */
static constexpr const char *kEhrpd = "ehrpd";

/**
 * HSPAP
 */
static constexpr const char *kHspap = "hspap";

/**
 * GSM
 */
static constexpr const char *kGsm = "gsm";

/**
 * TD-SCDMA
 */
static constexpr const char *kTdScdma = "td_scdma";

/**
 * IWLAN
 */
static constexpr const char *kIwlan = "iwlan";

/**
 * 5G NR (New Radio)
 */
static constexpr const char *kNr = "nr";

/**
 * 5G NRNSA (New Radio Non-Standalone)
 */
static constexpr const char *kNrnsa = "nrnsa";

/**
 * LTE CA
 */
static constexpr const char *kLteCa = "lte_ca";

}  // namespace NetworkConnectionSubtypeValues

namespace NetworkConnectionTypeValues
{
/**
 * none
 */
static constexpr const char *kWifi = "wifi";

/**
 * none
 */
static constexpr const char *kWired = "wired";

/**
 * none
 */
static constexpr const char *kCell = "cell";

/**
 * none
 */
static constexpr const char *kUnavailable = "unavailable";

/**
 * none
 */
static constexpr const char *kUnknown = "unknown";

}  // namespace NetworkConnectionTypeValues

namespace NetworkIoDirectionValues
{
/**
 * none
 */
static constexpr const char *kTransmit = "transmit";

/**
 * none
 */
static constexpr const char *kReceive = "receive";

}  // namespace NetworkIoDirectionValues

namespace NetworkTransportValues
{
/**
 * TCP
 */
static constexpr const char *kTcp = "tcp";

/**
 * UDP
 */
static constexpr const char *kUdp = "udp";

/**
 * Named or anonymous pipe.
 */
static constexpr const char *kPipe = "pipe";

/**
 * Unix domain socket
 */
static constexpr const char *kUnix = "unix";

/**
 * QUIC
 */
static constexpr const char *kQuic = "quic";

}  // namespace NetworkTransportValues

namespace NetworkTypeValues
{
/**
 * IPv4
 */
static constexpr const char *kIpv4 = "ipv4";

/**
 * IPv6
 */
static constexpr const char *kIpv6 = "ipv6";

}  // namespace NetworkTypeValues

}  // namespace network
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
