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
  The ISO 3166-1 alpha-2 2-character country code associated with the mobile carrier network.
 */
static constexpr const char *kNetworkCarrierIcc = "network.carrier.icc";

/**
  The mobile carrier country code.
 */
static constexpr const char *kNetworkCarrierMcc = "network.carrier.mcc";

/**
  The mobile carrier network code.
 */
static constexpr const char *kNetworkCarrierMnc = "network.carrier.mnc";

/**
  The name of the mobile carrier.
 */
static constexpr const char *kNetworkCarrierName = "network.carrier.name";

/**
  The state of network connection
  <p>
  Connection states are defined as part of the <a
  href="https://datatracker.ietf.org/doc/html/rfc9293#section-3.3.2">rfc9293</a>
 */
static constexpr const char *kNetworkConnectionState = "network.connection.state";

/**
  This describes more details regarding the connection.type. It may be the type of cell technology
  connection, but it could be used for describing details about a Wi-Fi connection.
 */
static constexpr const char *kNetworkConnectionSubtype = "network.connection.subtype";

/**
  The internet connection type.
 */
static constexpr const char *kNetworkConnectionType = "network.connection.type";

/**
  The network interface name.
 */
static constexpr const char *kNetworkInterfaceName = "network.interface.name";

/**
  The direction of traffic from the perspective of the observing host's physical or virtual network
  interface. It should not be used to represent the logical direction of a stateful connection or
  network flow.
 */
static constexpr const char *kNetworkIoDirection = "network.io.direction";

/**
  Local address of the network connection - IP address or UNIX domain socket name.
 */
static constexpr const char *kNetworkLocalAddress = "network.local.address";

/**
  The <a href="https://www.iana.org/assignments/as-numbers/as-numbers.xhtml">Autonomous System
  Number (ASN)</a> associated with the local IP address. <p> This attribute SHOULD only be set when
  the local address is an IP address and the ASN is available from routing, enrichment, or lookup
  data. The value SHOULD be represented as an integer without the @code AS @endcode prefix.
 */
static constexpr const char *kNetworkLocalAsNumber = "network.local.as.number";

/**
  The name of the organization associated with the autonomous system of the local IP address.
  <p>
  This attribute SHOULD only be set when @code network.local.as.number @endcode is set and the
  organization name is available. The value SHOULD be the organization name as provided by the
  lookup or enrichment source, not the AS name or short code.
 */
static constexpr const char *kNetworkLocalAsOrganization = "network.local.as.organization";

/**
  Local Media Access Control (MAC) address of the network connection.
  <p>
  MAC addresses MUST be represented in <a
  href="https://standards.ieee.org/wp-content/uploads/import/documents/tutorials/eui.pdf">IEEE RA
  hexadecimal form</a>: as hyphen-separated octets in uppercase hexadecimal form from most to least
  significant. <blockquote>
  [!WARNING]
  <p>
  This attribute may contain sensitive (PII) information.</blockquote>
 */
static constexpr const char *kNetworkLocalMacAddress = "network.local.mac.address";

/**
  Local port number of the network connection.
 */
static constexpr const char *kNetworkLocalPort = "network.local.port";

/**
  The IP prefix associated with the local IP address, in CIDR notation.
  <blockquote>
  [!WARNING]
  <p>
  This attribute may contain sensitive (PII) information. A @code /32 @endcode IPv4 prefix
  or @code /128 @endcode IPv6 prefix preserves the complete endpoint IP address.
  Using CIDR notation does not guarantee that the data is anonymous.</blockquote>
  <p>
  The value MUST be an IPv4 or IPv6 network prefix in CIDR notation, as defined in
  <a href="https://www.rfc-editor.org/rfc/rfc4632.html">RFC 4632</a> for IPv4 and
  <a href="https://www.rfc-editor.org/rfc/rfc4291.html#section-2.3">RFC 4291</a> for IPv6.
  IPv4 prefixes MUST use a dotted-decimal address followed by @code / @endcode and a decimal
  prefix length from 0 to 32. IPv6 prefixes MUST use the canonical address text
  representation defined in <a href="https://www.rfc-editor.org/rfc/rfc5952.html">RFC 5952</a>,
  followed by @code / @endcode and a decimal prefix length from 0 to 128. All host bits in
  the address MUST be set to zero. The value MUST NOT contain a domain name,
  UNIX domain socket name, or IPv6 zone identifier.
  <p>
  Instrumentation or Collector receivers that populate this attribute MUST
  convert native representations, such as an address and subnet mask or an
  address and prefix length, to equivalent CIDR notation before emitting it.
  <p>
  This attribute MAY be set whether or not @code network.local.address @endcode is set. If
  @code network.local.address @endcode is also set to an IP address, it MUST use the same IP version
  and be contained in this prefix.
 */
static constexpr const char *kNetworkLocalPrefix = "network.local.prefix";

/**
  Peer address of the network connection - IP address or UNIX domain socket name.
 */
static constexpr const char *kNetworkPeerAddress = "network.peer.address";

/**
  The <a href="https://www.iana.org/assignments/as-numbers/as-numbers.xhtml">Autonomous System
  Number (ASN)</a> associated with the peer IP address. <p> This attribute SHOULD only be set when
  the peer address is an IP address and the ASN is available from routing, enrichment, or lookup
  data. The value SHOULD be represented as an integer without the @code AS @endcode prefix.
 */
static constexpr const char *kNetworkPeerAsNumber = "network.peer.as.number";

/**
  The name of the organization associated with the autonomous system of the peer IP address.
  <p>
  This attribute SHOULD only be set when @code network.peer.as.number @endcode is set and the
  organization name is available. The value SHOULD be the organization name as provided by the
  lookup or enrichment source, not the AS name or short code.
 */
static constexpr const char *kNetworkPeerAsOrganization = "network.peer.as.organization";

/**
  Peer Media Access Control (MAC) address of the network connection.
  <p>
  MAC addresses MUST be represented in <a
  href="https://standards.ieee.org/wp-content/uploads/import/documents/tutorials/eui.pdf">IEEE RA
  hexadecimal form</a>: as hyphen-separated octets in uppercase hexadecimal form from most to least
  significant. <blockquote>
  [!WARNING]
  <p>
  This attribute may contain sensitive (PII) information.</blockquote>
 */
static constexpr const char *kNetworkPeerMacAddress = "network.peer.mac.address";

/**
  Peer port number of the network connection.
 */
static constexpr const char *kNetworkPeerPort = "network.peer.port";

/**
  The IP prefix associated with the peer IP address, in CIDR notation.
  <blockquote>
  [!WARNING]
  <p>
  This attribute may contain sensitive (PII) information. A @code /32 @endcode IPv4 prefix
  or @code /128 @endcode IPv6 prefix preserves the complete endpoint IP address.
  Using CIDR notation does not guarantee that the data is anonymous.</blockquote>
  <p>
  The value MUST be an IPv4 or IPv6 network prefix in CIDR notation, as defined in
  <a href="https://www.rfc-editor.org/rfc/rfc4632.html">RFC 4632</a> for IPv4 and
  <a href="https://www.rfc-editor.org/rfc/rfc4291.html#section-2.3">RFC 4291</a> for IPv6.
  IPv4 prefixes MUST use a dotted-decimal address followed by @code / @endcode and a decimal
  prefix length from 0 to 32. IPv6 prefixes MUST use the canonical address text
  representation defined in <a href="https://www.rfc-editor.org/rfc/rfc5952.html">RFC 5952</a>,
  followed by @code / @endcode and a decimal prefix length from 0 to 128. All host bits in
  the address MUST be set to zero. The value MUST NOT contain a domain name,
  UNIX domain socket name, or IPv6 zone identifier.
  <p>
  Instrumentation or Collector receivers that populate this attribute MUST
  convert native representations, such as an address and subnet mask or an
  address and prefix length, to equivalent CIDR notation before emitting it.
  <p>
  This attribute MAY be set whether or not @code network.peer.address @endcode is set. If
  @code network.peer.address @endcode is also set to an IP address, it MUST use the same IP version
  and be contained in this prefix.
 */
static constexpr const char *kNetworkPeerPrefix = "network.peer.prefix";

/**
  <a href="https://wikipedia.org/wiki/Application_layer">OSI application layer</a> or non-OSI
  equivalent. <p> The value SHOULD be normalized to lowercase.
 */
static constexpr const char *kNetworkProtocolName = "network.protocol.name";

/**
  The actual version of the protocol used for network communication.
  <p>
  If protocol version is subject to negotiation (for example using <a
  href="https://www.rfc-editor.org/rfc/rfc7301.html">ALPN</a>), this attribute SHOULD be set to the
  negotiated version. If the actual protocol version is not known, this attribute SHOULD NOT be set.
 */
static constexpr const char *kNetworkProtocolVersion = "network.protocol.version";

/**
  <a href="https://wikipedia.org/wiki/Transport_layer">OSI transport layer</a> or <a
  href="https://wikipedia.org/wiki/Inter-process_communication">inter-process communication
  method</a>. <p> The value SHOULD be normalized to lowercase. <p> Consider always setting the
  transport when setting a port number, since a port number is ambiguous without knowing the
  transport. For example different processes could be listening on TCP port 12345 and UDP port
  12345.
 */
static constexpr const char *kNetworkTransport = "network.transport";

/**
  <a href="https://wikipedia.org/wiki/Network_layer">OSI network layer</a> or non-OSI equivalent.
  <p>
  The value SHOULD be normalized to lowercase.
 */
static constexpr const char *kNetworkType = "network.type";

namespace NetworkConnectionStateValues
{

static constexpr const char *kClosed = "closed";

static constexpr const char *kCloseWait = "close_wait";

static constexpr const char *kClosing = "closing";

static constexpr const char *kEstablished = "established";

static constexpr const char *kFinWait1 = "fin_wait_1";

static constexpr const char *kFinWait2 = "fin_wait_2";

static constexpr const char *kLastAck = "last_ack";

static constexpr const char *kListen = "listen";

static constexpr const char *kSynReceived = "syn_received";

static constexpr const char *kSynSent = "syn_sent";

static constexpr const char *kTimeWait = "time_wait";

}  // namespace NetworkConnectionStateValues

namespace NetworkConnectionSubtypeValues
{
/**
  GPRS
 */
static constexpr const char *kGprs = "gprs";

/**
  EDGE
 */
static constexpr const char *kEdge = "edge";

/**
  UMTS
 */
static constexpr const char *kUmts = "umts";

/**
  CDMA
 */
static constexpr const char *kCdma = "cdma";

/**
  EVDO Rel. 0
 */
static constexpr const char *kEvdo0 = "evdo_0";

/**
  EVDO Rev. A
 */
static constexpr const char *kEvdoA = "evdo_a";

/**
  CDMA2000 1XRTT
 */
static constexpr const char *kCdma20001xrtt = "cdma2000_1xrtt";

/**
  HSDPA
 */
static constexpr const char *kHsdpa = "hsdpa";

/**
  HSUPA
 */
static constexpr const char *kHsupa = "hsupa";

/**
  HSPA
 */
static constexpr const char *kHspa = "hspa";

/**
  IDEN
 */
static constexpr const char *kIden = "iden";

/**
  EVDO Rev. B
 */
static constexpr const char *kEvdoB = "evdo_b";

/**
  LTE
 */
static constexpr const char *kLte = "lte";

/**
  EHRPD
 */
static constexpr const char *kEhrpd = "ehrpd";

/**
  HSPAP
 */
static constexpr const char *kHspap = "hspap";

/**
  GSM
 */
static constexpr const char *kGsm = "gsm";

/**
  TD-SCDMA
 */
static constexpr const char *kTdScdma = "td_scdma";

/**
  IWLAN
 */
static constexpr const char *kIwlan = "iwlan";

/**
  5G NR (New Radio)
 */
static constexpr const char *kNr = "nr";

/**
  5G NRNSA (New Radio Non-Standalone)
 */
static constexpr const char *kNrnsa = "nrnsa";

/**
  LTE CA
 */
static constexpr const char *kLteCa = "lte_ca";

}  // namespace NetworkConnectionSubtypeValues

namespace NetworkConnectionTypeValues
{
/**
  Wi-Fi
 */
static constexpr const char *kWifi = "wifi";

static constexpr const char *kWired = "wired";

static constexpr const char *kCell = "cell";

static constexpr const char *kUnavailable = "unavailable";

static constexpr const char *kUnknown = "unknown";

}  // namespace NetworkConnectionTypeValues

namespace NetworkIoDirectionValues
{

static constexpr const char *kTransmit = "transmit";

static constexpr const char *kReceive = "receive";

}  // namespace NetworkIoDirectionValues

namespace NetworkTransportValues
{
/**
  TCP
 */
static constexpr const char *kTcp = "tcp";

/**
  UDP
 */
static constexpr const char *kUdp = "udp";

/**
  Named or anonymous pipe.
 */
static constexpr const char *kPipe = "pipe";

/**
  UNIX domain socket
 */
static constexpr const char *kUnix = "unix";

/**
  QUIC
 */
static constexpr const char *kQuic = "quic";

}  // namespace NetworkTransportValues

namespace NetworkTypeValues
{
/**
  IPv4
 */
static constexpr const char *kIpv4 = "ipv4";

/**
  IPv6
 */
static constexpr const char *kIpv6 = "ipv6";

}  // namespace NetworkTypeValues

}  // namespace network
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
