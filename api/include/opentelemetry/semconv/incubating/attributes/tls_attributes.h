

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
namespace tls
{

/**
 * String indicating the <a
 * href="https://datatracker.ietf.org/doc/html/rfc5246#appendix-A.5">cipher</a> used during the
 * current connection. Note: The values allowed for @code tls.cipher @endcode MUST be one of the
 * @code Descriptions @endcode of the <a
 * href="https://www.iana.org/assignments/tls-parameters/tls-parameters.xhtml#table-tls-parameters-4">registered
 * TLS Cipher Suits</a>.
 */
static const char *kTlsCipher = "tls.cipher";

/**
 * PEM-encoded stand-alone certificate offered by the client. This is usually mutually-exclusive of
 * @code client.certificate_chain @endcode since this value also exists in that list.
 */
static const char *kTlsClientCertificate = "tls.client.certificate";

/**
 * Array of PEM-encoded certificates that make up the certificate chain offered by the client. This
 * is usually mutually-exclusive of @code client.certificate @endcode since that value should be the
 * first certificate in the chain.
 */
static const char *kTlsClientCertificateChain = "tls.client.certificate_chain";

/**
 * Certificate fingerprint using the MD5 digest of DER-encoded version of certificate offered by the
 * client. For consistency with other hash values, this value should be formatted as an uppercase
 * hash.
 */
static const char *kTlsClientHashMd5 = "tls.client.hash.md5";

/**
 * Certificate fingerprint using the SHA1 digest of DER-encoded version of certificate offered by
 * the client. For consistency with other hash values, this value should be formatted as an
 * uppercase hash.
 */
static const char *kTlsClientHashSha1 = "tls.client.hash.sha1";

/**
 * Certificate fingerprint using the SHA256 digest of DER-encoded version of certificate offered by
 * the client. For consistency with other hash values, this value should be formatted as an
 * uppercase hash.
 */
static const char *kTlsClientHashSha256 = "tls.client.hash.sha256";

/**
 * Distinguished name of <a
 * href="https://datatracker.ietf.org/doc/html/rfc5280#section-4.1.2.6">subject</a> of the issuer of
 * the x.509 certificate presented by the client.
 */
static const char *kTlsClientIssuer = "tls.client.issuer";

/**
 * A hash that identifies clients based on how they perform an SSL/TLS handshake.
 */
static const char *kTlsClientJa3 = "tls.client.ja3";

/**
 * Date/Time indicating when client certificate is no longer considered valid.
 */
static const char *kTlsClientNotAfter = "tls.client.not_after";

/**
 * Date/Time indicating when client certificate is first considered valid.
 */
static const char *kTlsClientNotBefore = "tls.client.not_before";

/**
 * @Deprecated: Replaced by `server.address.
 */
static const char *kTlsClientServerName = "tls.client.server_name";

/**
 * Distinguished name of subject of the x.509 certificate presented by the client.
 */
static const char *kTlsClientSubject = "tls.client.subject";

/**
 * Array of ciphers offered by the client during the client hello.
 */
static const char *kTlsClientSupportedCiphers = "tls.client.supported_ciphers";

/**
 * String indicating the curve used for the given cipher, when applicable.
 */
static const char *kTlsCurve = "tls.curve";

/**
 * Boolean flag indicating if the TLS negotiation was successful and transitioned to an encrypted
 * tunnel.
 */
static const char *kTlsEstablished = "tls.established";

/**
 * String indicating the protocol being tunneled. Per the values in the <a
 * href="https://www.iana.org/assignments/tls-extensiontype-values/tls-extensiontype-values.xhtml#alpn-protocol-ids">IANA
 * registry</a>, this string should be lower case.
 */
static const char *kTlsNextProtocol = "tls.next_protocol";

/**
 * Normalized lowercase protocol name parsed from original string of the negotiated <a
 * href="https://www.openssl.org/docs/man1.1.1/man3/SSL_get_version.html#RETURN-VALUES">SSL/TLS
 * protocol version</a>.
 */
static const char *kTlsProtocolName = "tls.protocol.name";

/**
 * Numeric part of the version parsed from the original string of the negotiated <a
 * href="https://www.openssl.org/docs/man1.1.1/man3/SSL_get_version.html#RETURN-VALUES">SSL/TLS
 * protocol version</a>.
 */
static const char *kTlsProtocolVersion = "tls.protocol.version";

/**
 * Boolean flag indicating if this TLS connection was resumed from an existing TLS negotiation.
 */
static const char *kTlsResumed = "tls.resumed";

/**
 * PEM-encoded stand-alone certificate offered by the server. This is usually mutually-exclusive of
 * @code server.certificate_chain @endcode since this value also exists in that list.
 */
static const char *kTlsServerCertificate = "tls.server.certificate";

/**
 * Array of PEM-encoded certificates that make up the certificate chain offered by the server. This
 * is usually mutually-exclusive of @code server.certificate @endcode since that value should be the
 * first certificate in the chain.
 */
static const char *kTlsServerCertificateChain = "tls.server.certificate_chain";

/**
 * Certificate fingerprint using the MD5 digest of DER-encoded version of certificate offered by the
 * server. For consistency with other hash values, this value should be formatted as an uppercase
 * hash.
 */
static const char *kTlsServerHashMd5 = "tls.server.hash.md5";

/**
 * Certificate fingerprint using the SHA1 digest of DER-encoded version of certificate offered by
 * the server. For consistency with other hash values, this value should be formatted as an
 * uppercase hash.
 */
static const char *kTlsServerHashSha1 = "tls.server.hash.sha1";

/**
 * Certificate fingerprint using the SHA256 digest of DER-encoded version of certificate offered by
 * the server. For consistency with other hash values, this value should be formatted as an
 * uppercase hash.
 */
static const char *kTlsServerHashSha256 = "tls.server.hash.sha256";

/**
 * Distinguished name of <a
 * href="https://datatracker.ietf.org/doc/html/rfc5280#section-4.1.2.6">subject</a> of the issuer of
 * the x.509 certificate presented by the client.
 */
static const char *kTlsServerIssuer = "tls.server.issuer";

/**
 * A hash that identifies servers based on how they perform an SSL/TLS handshake.
 */
static const char *kTlsServerJa3s = "tls.server.ja3s";

/**
 * Date/Time indicating when server certificate is no longer considered valid.
 */
static const char *kTlsServerNotAfter = "tls.server.not_after";

/**
 * Date/Time indicating when server certificate is first considered valid.
 */
static const char *kTlsServerNotBefore = "tls.server.not_before";

/**
 * Distinguished name of subject of the x.509 certificate presented by the server.
 */
static const char *kTlsServerSubject = "tls.server.subject";

// DEBUG: {"brief": "Normalized lowercase protocol name parsed from original string of the
// negotiated [SSL/TLS protocol
// version](https://www.openssl.org/docs/man1.1.1/man3/SSL_get_version.html#RETURN-VALUES)\n",
// "name": "tls.protocol.name", "requirement_level": "recommended", "root_namespace": "tls",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": none,
// "deprecated": none, "id": "ssl", "note": none, "stability": "experimental", "value": "ssl"},
// {"brief": none, "deprecated": none, "id": "tls", "note": none, "stability": "experimental",
// "value": "tls"}]}}
namespace TlsProtocolNameValues
{
/**
 * ssl.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "ssl", "note": none, "stability":
// "experimental", "value": "ssl"}
static constexpr const char *kSsl = "ssl";
/**
 * tls.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "tls", "note": none, "stability":
// "experimental", "value": "tls"}
static constexpr const char *kTls = "tls";
}  // namespace TlsProtocolNameValues

}  // namespace tls
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
