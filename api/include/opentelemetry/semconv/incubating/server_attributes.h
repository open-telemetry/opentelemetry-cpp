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
namespace server
{

/**
  Server domain name if available without reverse DNS lookup; otherwise, IP address or UNIX domain
  socket name. <p> When observed from the client side, and when communicating through an
  intermediary, @code server.address @endcode SHOULD represent the server address behind any
  intermediaries, for example proxies, if it's available.
 */
static constexpr const char *kServerAddress = "server.address";

/**
  The <a href="https://www.iana.org/assignments/as-numbers/as-numbers.xhtml">Autonomous System
  Number (ASN)</a> associated with the server IP address. <p> This attribute SHOULD only be set when
  the server address is an IP address and the ASN is available from routing, enrichment, or lookup
  data. The value SHOULD be represented as an integer without the @code AS @endcode prefix.
 */
static constexpr const char *kServerAsNumber = "server.as.number";

/**
  The name of the organization associated with the autonomous system of the server IP address.
  <p>
  This attribute SHOULD only be set when @code server.as.number @endcode is set and the organization
  name is available. The value SHOULD be the organization name as provided by the lookup or
  enrichment source, not the AS name or short code.
 */
static constexpr const char *kServerAsOrganization = "server.as.organization";

/**
  Server Media Access Control (MAC) address.
  <p>
  This attribute MUST identify a MAC address associated with the logical server
  endpoint, including when that endpoint is behind intermediaries. If the server's
  MAC address is unknown, this attribute MUST NOT be set. The MAC address of a
  proxy, gateway, or next-hop router MUST NOT be used as a substitute.
  <p>
  MAC addresses MUST be represented in <a
  href="https://standards.ieee.org/wp-content/uploads/import/documents/tutorials/eui.pdf">IEEE RA
  hexadecimal form</a>: as hyphen-separated octets in uppercase hexadecimal form from most to least
  significant. <blockquote>
  [!WARNING]
  <p>
  This attribute may contain sensitive (PII) information.</blockquote>
 */
static constexpr const char *kServerMacAddress = "server.mac.address";

/**
  Server port number.
  <p>
  When observed from the client side, and when communicating through an intermediary, @code
  server.port @endcode SHOULD represent the server port behind any intermediaries, for example
  proxies, if it's available.
 */
static constexpr const char *kServerPort = "server.port";

/**
  The IP prefix associated with the server IP address, in CIDR notation.
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
  This attribute MUST NOT be set unless @code server.address @endcode is set to an IPv4 or
  IPv6 address. The address MUST use the same IP version as this prefix and
  be contained in it.
 */
static constexpr const char *kServerPrefix = "server.prefix";

/**
  Logical name of the service acting as the server in a connection-based network interaction.
  <p>
  SHOULD be equal to the actual <a href="/docs/resource/README.md#service">@code service.name
  @endcode</a> resource attribute of the logical server, if any. This identifies the server service
  whether the telemetry is recorded by the client, the server, or another observer.
 */
static constexpr const char *kServerServiceName = "server.service.name";

/**
  Logical namespace of the service acting as the server in a connection-based network interaction.
  <p>
  SHOULD be equal to the actual <a href="/docs/resource/README.md#service">@code service.namespace
  @endcode</a> resource attribute of the logical server, if any. This identifies the server
  service's namespace whether the telemetry is recorded by the client, the server, or another
  observer. Together with @code server.service.name @endcode, this identifies the server service. An
  empty namespace is equivalent to an unspecified namespace.
 */
static constexpr const char *kServerServiceNamespace = "server.service.namespace";

}  // namespace server
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
