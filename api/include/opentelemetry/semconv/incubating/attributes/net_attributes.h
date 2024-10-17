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
 * Deprecated, use @code network.local.address @endcode.
 * <p>
 * @deprecated
 * Replaced by @code network.local.address @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetHostIp = "net.host.ip";

/**
 * Deprecated, use @code server.address @endcode.
 * <p>
 * @deprecated
 * Replaced by @code server.address @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetHostName = "net.host.name";

/**
 * Deprecated, use @code server.port @endcode.
 * <p>
 * @deprecated
 * Replaced by @code server.port @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetHostPort = "net.host.port";

/**
 * Deprecated, use @code network.peer.address @endcode.
 * <p>
 * @deprecated
 * Replaced by @code network.peer.address @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetPeerIp = "net.peer.ip";

/**
 * Deprecated, use @code server.address @endcode on client spans and @code client.address @endcode
 * on server spans. <p>
 * @deprecated
 * Replaced by @code server.address @endcode on client spans and @code client.address @endcode on
 * server spans.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetPeerName = "net.peer.name";

/**
 * Deprecated, use @code server.port @endcode on client spans and @code client.port @endcode on
 * server spans. <p>
 * @deprecated
 * Replaced by @code server.port @endcode on client spans and @code client.port @endcode on server
 * spans.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetPeerPort = "net.peer.port";

/**
 * Deprecated, use @code network.protocol.name @endcode.
 * <p>
 * @deprecated
 * Replaced by @code network.protocol.name @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetProtocolName = "net.protocol.name";

/**
 * Deprecated, use @code network.protocol.version @endcode.
 * <p>
 * @deprecated
 * Replaced by @code network.protocol.version @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetProtocolVersion = "net.protocol.version";

/**
 * Deprecated, use @code network.transport @endcode and @code network.type @endcode.
 * <p>
 * @deprecated
 * Split to @code network.transport @endcode and @code network.type @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetSockFamily = "net.sock.family";

/**
 * Deprecated, use @code network.local.address @endcode.
 * <p>
 * @deprecated
 * Replaced by @code network.local.address @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetSockHostAddr = "net.sock.host.addr";

/**
 * Deprecated, use @code network.local.port @endcode.
 * <p>
 * @deprecated
 * Replaced by @code network.local.port @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetSockHostPort = "net.sock.host.port";

/**
 * Deprecated, use @code network.peer.address @endcode.
 * <p>
 * @deprecated
 * Replaced by @code network.peer.address @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetSockPeerAddr = "net.sock.peer.addr";

/**
 * Deprecated, no replacement at this time.
 * <p>
 * @deprecated
 * Removed.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetSockPeerName = "net.sock.peer.name";

/**
 * Deprecated, use @code network.peer.port @endcode.
 * <p>
 * @deprecated
 * Replaced by @code network.peer.port @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetSockPeerPort = "net.sock.peer.port";

/**
 * Deprecated, use @code network.transport @endcode.
 * <p>
 * @deprecated
 * Replaced by @code network.transport @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kNetTransport = "net.transport";

namespace NetSockFamilyValues
{
/**
 * IPv4 address
 */
static constexpr const char *kInet = "inet";

/**
 * IPv6 address
 */
static constexpr const char *kInet6 = "inet6";

/**
 * Unix domain socket path
 */
static constexpr const char *kUnix = "unix";

}  // namespace NetSockFamilyValues

namespace NetTransportValues
{
/**
 * none
 */
static constexpr const char *kIpTcp = "ip_tcp";

/**
 * none
 */
static constexpr const char *kIpUdp = "ip_udp";

/**
 * Named or anonymous pipe.
 */
static constexpr const char *kPipe = "pipe";

/**
 * In-process communication.
 */
static constexpr const char *kInproc = "inproc";

/**
 * Something else (non IP-based).
 */
static constexpr const char *kOther = "other";

}  // namespace NetTransportValues

}  // namespace net
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
