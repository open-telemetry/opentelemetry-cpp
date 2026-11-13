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
namespace destination
{

/**
  Destination address as observed at the point of instrumentation - typically an IP address, or a
  UNIX domain socket name; a domain name only when the receiver was addressed by name (for example a
  peer-to-peer node dialed by hostname), never obtained via reverse DNS lookup. <p>
  @code destination.address @endcode SHOULD be the receiver address as observed at the point of
  instrumentation, for example the destination of the packet, flow, or exchange seen on the wire or
  socket. Instrumentation SHOULD capture the available address and SHOULD NOT try to resolve the
  address behind intermediaries such as proxies or load balancers, or perform a reverse DNS lookup
  to obtain a domain name.
 */
static constexpr const char *kDestinationAddress = "destination.address";

/**
  The <a href="https://www.iana.org/assignments/as-numbers/as-numbers.xhtml">Autonomous System
  Number (ASN)</a> associated with the destination IP address. <p> This attribute SHOULD only be set
  when the destination address is an IP address and the ASN is available from routing, enrichment,
  or lookup data. The value SHOULD be represented as an integer without the @code AS @endcode
  prefix.
 */
static constexpr const char *kDestinationAsNumber = "destination.as.number";

/**
  The name of the organization associated with the autonomous system of the destination IP address.
  <p>
  This attribute SHOULD only be set when @code destination.as.number @endcode is set and the
  organization name is available. The value SHOULD be the organization name as provided by the
  lookup or enrichment source, not the AS name or short code.
 */
static constexpr const char *kDestinationAsOrganization = "destination.as.organization";

/**
  Destination Media Access Control (MAC) address.
  <p>
  MAC addresses MUST be represented in <a
  href="https://standards.ieee.org/wp-content/uploads/import/documents/tutorials/eui.pdf">IEEE RA
  hexadecimal form</a>: as hyphen-separated octets in uppercase hexadecimal form from most to least
  significant. <blockquote>
  [!WARNING]
  <p>
  This attribute may contain sensitive (PII) information.</blockquote>
 */
static constexpr const char *kDestinationMacAddress = "destination.mac.address";

/**
  Destination port number
 */
static constexpr const char *kDestinationPort = "destination.port";

/**
  The IP prefix associated with the destination IP address, in CIDR notation.
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
  This attribute MAY be set whether or not @code destination.address @endcode is set. If
  @code destination.address @endcode is also set to an IP address, it MUST use the same IP version
  and be contained in this prefix.
 */
static constexpr const char *kDestinationPrefix = "destination.prefix";

}  // namespace destination
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
