

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
static const char *kNetworkCarrierIcc = "network.carrier.icc";

/**
 * The mobile carrier country code.
 */
static const char *kNetworkCarrierMcc = "network.carrier.mcc";

/**
 * The mobile carrier network code.
 */
static const char *kNetworkCarrierMnc = "network.carrier.mnc";

/**
 * The name of the mobile carrier.
 */
static const char *kNetworkCarrierName = "network.carrier.name";

/**
 * This describes more details regarding the connection.type. It may be the type of cell technology
 * connection, but it could be used for describing details about a wifi connection.
 */
static const char *kNetworkConnectionSubtype = "network.connection.subtype";

/**
 * The internet connection type.
 */
static const char *kNetworkConnectionType = "network.connection.type";

/**
 * The network IO operation direction.
 */
static const char *kNetworkIoDirection = "network.io.direction";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes. @endcode.
 */
static const char *kNetworkLocalAddress = "network.local.address";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes. @endcode.
 */
static const char *kNetworkLocalPort = "network.local.port";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes. @endcode.
 */
static const char *kNetworkPeerAddress = "network.peer.address";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes. @endcode.
 */
static const char *kNetworkPeerPort = "network.peer.port";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes. @endcode.
 */
static const char *kNetworkProtocolName = "network.protocol.name";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes. @endcode.
 */
static const char *kNetworkProtocolVersion = "network.protocol.version";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes. @endcode.
 */
static const char *kNetworkTransport = "network.transport";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes. @endcode.
 */
static const char *kNetworkType = "network.type";

// DEBUG: {"brief": "This describes more details regarding the connection.type. It may be the type
// of cell technology connection, but it could be used for describing details about a wifi
// connection.", "examples": "LTE", "name": "network.connection.subtype", "requirement_level":
// "recommended", "root_namespace": "network", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": "GPRS", "deprecated": none, "id": "gprs",
// "note": none, "stability": "experimental", "value": "gprs"}, {"brief": "EDGE", "deprecated":
// none, "id": "edge", "note": none, "stability": "experimental", "value": "edge"}, {"brief":
// "UMTS", "deprecated": none, "id": "umts", "note": none, "stability": "experimental", "value":
// "umts"}, {"brief": "CDMA", "deprecated": none, "id": "cdma", "note": none, "stability":
// "experimental", "value": "cdma"}, {"brief": "EVDO Rel. 0", "deprecated": none, "id": "evdo_0",
// "note": none, "stability": "experimental", "value": "evdo_0"}, {"brief": "EVDO Rev. A",
// "deprecated": none, "id": "evdo_a", "note": none, "stability": "experimental", "value":
// "evdo_a"}, {"brief": "CDMA2000 1XRTT", "deprecated": none, "id": "cdma2000_1xrtt", "note": none,
// "stability": "experimental", "value": "cdma2000_1xrtt"}, {"brief": "HSDPA", "deprecated": none,
// "id": "hsdpa", "note": none, "stability": "experimental", "value": "hsdpa"}, {"brief": "HSUPA",
// "deprecated": none, "id": "hsupa", "note": none, "stability": "experimental", "value": "hsupa"},
// {"brief": "HSPA", "deprecated": none, "id": "hspa", "note": none, "stability": "experimental",
// "value": "hspa"}, {"brief": "IDEN", "deprecated": none, "id": "iden", "note": none, "stability":
// "experimental", "value": "iden"}, {"brief": "EVDO Rev. B", "deprecated": none, "id": "evdo_b",
// "note": none, "stability": "experimental", "value": "evdo_b"}, {"brief": "LTE", "deprecated":
// none, "id": "lte", "note": none, "stability": "experimental", "value": "lte"}, {"brief": "EHRPD",
// "deprecated": none, "id": "ehrpd", "note": none, "stability": "experimental", "value": "ehrpd"},
// {"brief": "HSPAP", "deprecated": none, "id": "hspap", "note": none, "stability": "experimental",
// "value": "hspap"}, {"brief": "GSM", "deprecated": none, "id": "gsm", "note": none, "stability":
// "experimental", "value": "gsm"}, {"brief": "TD-SCDMA", "deprecated": none, "id": "td_scdma",
// "note": none, "stability": "experimental", "value": "td_scdma"}, {"brief": "IWLAN", "deprecated":
// none, "id": "iwlan", "note": none, "stability": "experimental", "value": "iwlan"}, {"brief": "5G
// NR (New Radio)", "deprecated": none, "id": "nr", "note": none, "stability": "experimental",
// "value": "nr"}, {"brief": "5G NRNSA (New Radio Non-Standalone)", "deprecated": none, "id":
// "nrnsa", "note": none, "stability": "experimental", "value": "nrnsa"}, {"brief": "LTE CA",
// "deprecated": none, "id": "lte_ca", "note": none, "stability": "experimental", "value":
// "lte_ca"}]}}
namespace NetworkConnectionSubtypeValues
{
/**
 * GPRS.
 */
// DEBUG: {"brief": "GPRS", "deprecated": none, "id": "gprs", "note": none, "stability":
// "experimental", "value": "gprs"}
static constexpr const char *kGprs = "gprs";
/**
 * EDGE.
 */
// DEBUG: {"brief": "EDGE", "deprecated": none, "id": "edge", "note": none, "stability":
// "experimental", "value": "edge"}
static constexpr const char *kEdge = "edge";
/**
 * UMTS.
 */
// DEBUG: {"brief": "UMTS", "deprecated": none, "id": "umts", "note": none, "stability":
// "experimental", "value": "umts"}
static constexpr const char *kUmts = "umts";
/**
 * CDMA.
 */
// DEBUG: {"brief": "CDMA", "deprecated": none, "id": "cdma", "note": none, "stability":
// "experimental", "value": "cdma"}
static constexpr const char *kCdma = "cdma";
/**
 * EVDO Rel. 0.
 */
// DEBUG: {"brief": "EVDO Rel. 0", "deprecated": none, "id": "evdo_0", "note": none, "stability":
// "experimental", "value": "evdo_0"}
static constexpr const char *kEvdo0 = "evdo_0";
/**
 * EVDO Rev. A.
 */
// DEBUG: {"brief": "EVDO Rev. A", "deprecated": none, "id": "evdo_a", "note": none, "stability":
// "experimental", "value": "evdo_a"}
static constexpr const char *kEvdoA = "evdo_a";
/**
 * CDMA2000 1XRTT.
 */
// DEBUG: {"brief": "CDMA2000 1XRTT", "deprecated": none, "id": "cdma2000_1xrtt", "note": none,
// "stability": "experimental", "value": "cdma2000_1xrtt"}
static constexpr const char *kCdma20001xrtt = "cdma2000_1xrtt";
/**
 * HSDPA.
 */
// DEBUG: {"brief": "HSDPA", "deprecated": none, "id": "hsdpa", "note": none, "stability":
// "experimental", "value": "hsdpa"}
static constexpr const char *kHsdpa = "hsdpa";
/**
 * HSUPA.
 */
// DEBUG: {"brief": "HSUPA", "deprecated": none, "id": "hsupa", "note": none, "stability":
// "experimental", "value": "hsupa"}
static constexpr const char *kHsupa = "hsupa";
/**
 * HSPA.
 */
// DEBUG: {"brief": "HSPA", "deprecated": none, "id": "hspa", "note": none, "stability":
// "experimental", "value": "hspa"}
static constexpr const char *kHspa = "hspa";
/**
 * IDEN.
 */
// DEBUG: {"brief": "IDEN", "deprecated": none, "id": "iden", "note": none, "stability":
// "experimental", "value": "iden"}
static constexpr const char *kIden = "iden";
/**
 * EVDO Rev. B.
 */
// DEBUG: {"brief": "EVDO Rev. B", "deprecated": none, "id": "evdo_b", "note": none, "stability":
// "experimental", "value": "evdo_b"}
static constexpr const char *kEvdoB = "evdo_b";
/**
 * LTE.
 */
// DEBUG: {"brief": "LTE", "deprecated": none, "id": "lte", "note": none, "stability":
// "experimental", "value": "lte"}
static constexpr const char *kLte = "lte";
/**
 * EHRPD.
 */
// DEBUG: {"brief": "EHRPD", "deprecated": none, "id": "ehrpd", "note": none, "stability":
// "experimental", "value": "ehrpd"}
static constexpr const char *kEhrpd = "ehrpd";
/**
 * HSPAP.
 */
// DEBUG: {"brief": "HSPAP", "deprecated": none, "id": "hspap", "note": none, "stability":
// "experimental", "value": "hspap"}
static constexpr const char *kHspap = "hspap";
/**
 * GSM.
 */
// DEBUG: {"brief": "GSM", "deprecated": none, "id": "gsm", "note": none, "stability":
// "experimental", "value": "gsm"}
static constexpr const char *kGsm = "gsm";
/**
 * TD-SCDMA.
 */
// DEBUG: {"brief": "TD-SCDMA", "deprecated": none, "id": "td_scdma", "note": none, "stability":
// "experimental", "value": "td_scdma"}
static constexpr const char *kTdScdma = "td_scdma";
/**
 * IWLAN.
 */
// DEBUG: {"brief": "IWLAN", "deprecated": none, "id": "iwlan", "note": none, "stability":
// "experimental", "value": "iwlan"}
static constexpr const char *kIwlan = "iwlan";
/**
 * 5G NR (New Radio).
 */
// DEBUG: {"brief": "5G NR (New Radio)", "deprecated": none, "id": "nr", "note": none, "stability":
// "experimental", "value": "nr"}
static constexpr const char *kNr = "nr";
/**
 * 5G NRNSA (New Radio Non-Standalone).
 */
// DEBUG: {"brief": "5G NRNSA (New Radio Non-Standalone)", "deprecated": none, "id": "nrnsa",
// "note": none, "stability": "experimental", "value": "nrnsa"}
static constexpr const char *kNrnsa = "nrnsa";
/**
 * LTE CA.
 */
// DEBUG: {"brief": "LTE CA", "deprecated": none, "id": "lte_ca", "note": none, "stability":
// "experimental", "value": "lte_ca"}
static constexpr const char *kLteCa = "lte_ca";
}  // namespace NetworkConnectionSubtypeValues

// DEBUG: {"brief": "The internet connection type.", "examples": "wifi", "name":
// "network.connection.type", "requirement_level": "recommended", "root_namespace": "network",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": none,
// "deprecated": none, "id": "wifi", "note": none, "stability": "experimental", "value": "wifi"},
// {"brief": none, "deprecated": none, "id": "wired", "note": none, "stability": "experimental",
// "value": "wired"}, {"brief": none, "deprecated": none, "id": "cell", "note": none, "stability":
// "experimental", "value": "cell"}, {"brief": none, "deprecated": none, "id": "unavailable",
// "note": none, "stability": "experimental", "value": "unavailable"}, {"brief": none, "deprecated":
// none, "id": "unknown", "note": none, "stability": "experimental", "value": "unknown"}]}}
namespace NetworkConnectionTypeValues
{
/**
 * wifi.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "wifi", "note": none, "stability":
// "experimental", "value": "wifi"}
static constexpr const char *kWifi = "wifi";
/**
 * wired.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "wired", "note": none, "stability":
// "experimental", "value": "wired"}
static constexpr const char *kWired = "wired";
/**
 * cell.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "cell", "note": none, "stability":
// "experimental", "value": "cell"}
static constexpr const char *kCell = "cell";
/**
 * unavailable.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "unavailable", "note": none, "stability":
// "experimental", "value": "unavailable"}
static constexpr const char *kUnavailable = "unavailable";
/**
 * unknown.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "unknown", "note": none, "stability":
// "experimental", "value": "unknown"}
static constexpr const char *kUnknown = "unknown";
}  // namespace NetworkConnectionTypeValues

// DEBUG: {"brief": "The network IO operation direction.", "examples": ["transmit"], "name":
// "network.io.direction", "requirement_level": "recommended", "root_namespace": "network",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": none,
// "deprecated": none, "id": "transmit", "note": none, "stability": "experimental", "value":
// "transmit"}, {"brief": none, "deprecated": none, "id": "receive", "note": none, "stability":
// "experimental", "value": "receive"}]}}
namespace NetworkIoDirectionValues
{
/**
 * transmit.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "transmit", "note": none, "stability":
// "experimental", "value": "transmit"}
static constexpr const char *kTransmit = "transmit";
/**
 * receive.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "receive", "note": none, "stability":
// "experimental", "value": "receive"}
static constexpr const char *kReceive = "receive";
}  // namespace NetworkIoDirectionValues

// @deprecated(reason="Deprecated in favor of stable
// `opentelemetry.semconv.attributes.network_attributes.NetworkTransportValues`.")  # type: ignore
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
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes.NetworkTransportValues.kTcp @endcode.
 */
// DEBUG: {"brief": "TCP", "deprecated": none, "id": "tcp", "note": none, "stability": "stable",
// "value": "tcp"}
static constexpr const char *kTcp = "tcp";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes.NetworkTransportValues.kUdp @endcode.
 */
// DEBUG: {"brief": "UDP", "deprecated": none, "id": "udp", "note": none, "stability": "stable",
// "value": "udp"}
static constexpr const char *kUdp = "udp";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes.NetworkTransportValues.kPipe @endcode.
 */
// DEBUG: {"brief": "Named or anonymous pipe.", "deprecated": none, "id": "pipe", "note": none,
// "stability": "stable", "value": "pipe"}
static constexpr const char *kPipe = "pipe";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes.NetworkTransportValues.kUnix @endcode.
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

// @deprecated(reason="Deprecated in favor of stable
// `opentelemetry.semconv.attributes.network_attributes.NetworkTypeValues`.")  # type: ignore DEBUG:
// {"brief": "[OSI network layer](https://osi-model.com/network-layer/) or non-OSI equivalent.",
// "examples": ["ipv4", "ipv6"], "name": "network.type", "note": "The value SHOULD be normalized to
// lowercase.", "requirement_level": "recommended", "root_namespace": "network", "stability":
// "stable", "type": {"allow_custom_values": true, "members": [{"brief": "IPv4", "deprecated": none,
// "id": "ipv4", "note": none, "stability": "stable", "value": "ipv4"}, {"brief": "IPv6",
// "deprecated": none, "id": "ipv6", "note": none, "stability": "stable", "value": "ipv6"}]}}
namespace NetworkTypeValues
{
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes.NetworkTypeValues.kIpv4 @endcode.
 */
// DEBUG: {"brief": "IPv4", "deprecated": none, "id": "ipv4", "note": none, "stability": "stable",
// "value": "ipv4"}
static constexpr const char *kIpv4 = "ipv4";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.network_attributes.NetworkTypeValues.kIpv6 @endcode.
 */
// DEBUG: {"brief": "IPv6", "deprecated": none, "id": "ipv6", "note": none, "stability": "stable",
// "value": "ipv6"}
static constexpr const char *kIpv6 = "ipv6";
}  // namespace NetworkTypeValues

}  // namespace network
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
