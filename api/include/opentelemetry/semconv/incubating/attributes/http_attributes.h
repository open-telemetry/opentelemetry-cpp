

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
namespace http
{

/**
 * @Deprecated: Replaced by @code client.address @endcode.
 */
static const char *kHttpClientIp = "http.client_ip";

/**
 * State of the HTTP connection in the HTTP connection pool.
 */
static const char *kHttpConnectionState = "http.connection.state";

/**
 * @Deprecated: Replaced by @code network.protocol.name @endcode.
 */
static const char *kHttpFlavor = "http.flavor";

/**
 * @Deprecated: Replaced by one of @code server.address @endcode, @code client.address @endcode or
 * @code http.request.header.host @endcode, depending on the usage.
 */
static const char *kHttpHost = "http.host";

/**
 * @Deprecated: Replaced by @code http.request.method @endcode.
 */
static const char *kHttpMethod = "http.method";

/**
 * The size of the request payload body in bytes. This is the number of bytes transferred excluding
 * headers and is often, but not always, present as the <a
 * href="https://www.rfc-editor.org/rfc/rfc9110.html#field.content-length">Content-Length</a>
 * header. For requests using transport encoding, this should be the compressed size.
 */
static const char *kHttpRequestBodySize = "http.request.body.size";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.http_attributes.
 * @endcode.
 */
static const char *kHttpRequestHeader = "http.request.header";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.http_attributes.
 * @endcode.
 */
static const char *kHttpRequestMethod = "http.request.method";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.http_attributes.
 * @endcode.
 */
static const char *kHttpRequestMethodOriginal = "http.request.method_original";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.http_attributes.
 * @endcode.
 */
static const char *kHttpRequestResendCount = "http.request.resend_count";

/**
 * The total size of the request in bytes. This should be the total number of bytes sent over the
 * wire, including the request line (HTTP/1.1), framing (HTTP/2 and HTTP/3), headers, and request
 * body if any.
 */
static const char *kHttpRequestSize = "http.request.size";

/**
 * @Deprecated: Replaced by @code http.request.header.content-length @endcode.
 */
static const char *kHttpRequestContentLength = "http.request_content_length";

/**
 * @Deprecated: Replaced by @code http.request.body.size @endcode.
 */
static const char *kHttpRequestContentLengthUncompressed =
    "http.request_content_length_uncompressed";

/**
 * The size of the response payload body in bytes. This is the number of bytes transferred excluding
 * headers and is often, but not always, present as the <a
 * href="https://www.rfc-editor.org/rfc/rfc9110.html#field.content-length">Content-Length</a>
 * header. For requests using transport encoding, this should be the compressed size.
 */
static const char *kHttpResponseBodySize = "http.response.body.size";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.http_attributes.
 * @endcode.
 */
static const char *kHttpResponseHeader = "http.response.header";

/**
 * The total size of the response in bytes. This should be the total number of bytes sent over the
 * wire, including the status line (HTTP/1.1), framing (HTTP/2 and HTTP/3), headers, and response
 * body and trailers if any.
 */
static const char *kHttpResponseSize = "http.response.size";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.http_attributes.
 * @endcode.
 */
static const char *kHttpResponseStatusCode = "http.response.status_code";

/**
 * @Deprecated: Replaced by @code http.response.header.content-length @endcode.
 */
static const char *kHttpResponseContentLength = "http.response_content_length";

/**
 * @Deprecated: Replace by @code http.response.body.size @endcode.
 */
static const char *kHttpResponseContentLengthUncompressed =
    "http.response_content_length_uncompressed";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.http_attributes.
 * @endcode.
 */
static const char *kHttpRoute = "http.route";

/**
 * @Deprecated: Replaced by @code url.scheme @endcode instead.
 */
static const char *kHttpScheme = "http.scheme";

/**
 * @Deprecated: Replaced by @code server.address @endcode.
 */
static const char *kHttpServerName = "http.server_name";

/**
 * @Deprecated: Replaced by @code http.response.status_code @endcode.
 */
static const char *kHttpStatusCode = "http.status_code";

/**
 * @Deprecated: Split to @code url.path @endcode and `url.query.
 */
static const char *kHttpTarget = "http.target";

/**
 * @Deprecated: Replaced by @code url.full @endcode.
 */
static const char *kHttpUrl = "http.url";

/**
 * @Deprecated: Replaced by @code user_agent.original @endcode.
 */
static const char *kHttpUserAgent = "http.user_agent";

// DEBUG: {"brief": "State of the HTTP connection in the HTTP connection pool.", "examples":
// ["active", "idle"], "name": "http.connection.state", "requirement_level": "recommended",
// "root_namespace": "http", "stability": "experimental", "type": {"allow_custom_values": true,
// "members": [{"brief": "active state.", "deprecated": none, "id": "active", "note": none,
// "stability": "experimental", "value": "active"}, {"brief": "idle state.", "deprecated": none,
// "id": "idle", "note": none, "stability": "experimental", "value": "idle"}]}}
namespace HttpConnectionStateValues
{
/**
 * active state.
 */
// DEBUG: {"brief": "active state.", "deprecated": none, "id": "active", "note": none, "stability":
// "experimental", "value": "active"}
static constexpr const char *kActive = "active";
/**
 * idle state.
 */
// DEBUG: {"brief": "idle state.", "deprecated": none, "id": "idle", "note": none, "stability":
// "experimental", "value": "idle"}
static constexpr const char *kIdle = "idle";
}  // namespace HttpConnectionStateValues

// @deprecated(reason="The attribute http.flavor is deprecated - Replaced by
// `network.protocol.name`")  # type: ignore DEBUG: {"brief": "Deprecated, use
// `network.protocol.name` instead.", "deprecated": "Replaced by `network.protocol.name`.", "name":
// "http.flavor", "requirement_level": "recommended", "root_namespace": "http", "stability":
// "experimental", "type": {"allow_custom_values": true, "members": [{"brief": "HTTP/1.0",
// "deprecated": none, "id": "http_1_0", "note": none, "stability": "experimental", "value": "1.0"},
// {"brief": "HTTP/1.1", "deprecated": none, "id": "http_1_1", "note": none, "stability":
// "experimental", "value": "1.1"}, {"brief": "HTTP/2", "deprecated": none, "id": "http_2_0",
// "note": none, "stability": "experimental", "value": "2.0"}, {"brief": "HTTP/3", "deprecated":
// none, "id": "http_3_0", "note": none, "stability": "experimental", "value": "3.0"}, {"brief":
// "SPDY protocol.", "deprecated": none, "id": "spdy", "note": none, "stability": "experimental",
// "value": "SPDY"}, {"brief": "QUIC protocol.", "deprecated": none, "id": "quic", "note": none,
// "stability": "experimental", "value": "QUIC"}]}}
namespace HttpFlavorValues
{
/**
 * HTTP/1.0.
 */
// DEBUG: {"brief": "HTTP/1.0", "deprecated": none, "id": "http_1_0", "note": none, "stability":
// "experimental", "value": "1.0"}
static constexpr const char *kHttp10 = "1.0";
/**
 * HTTP/1.1.
 */
// DEBUG: {"brief": "HTTP/1.1", "deprecated": none, "id": "http_1_1", "note": none, "stability":
// "experimental", "value": "1.1"}
static constexpr const char *kHttp11 = "1.1";
/**
 * HTTP/2.
 */
// DEBUG: {"brief": "HTTP/2", "deprecated": none, "id": "http_2_0", "note": none, "stability":
// "experimental", "value": "2.0"}
static constexpr const char *kHttp20 = "2.0";
/**
 * HTTP/3.
 */
// DEBUG: {"brief": "HTTP/3", "deprecated": none, "id": "http_3_0", "note": none, "stability":
// "experimental", "value": "3.0"}
static constexpr const char *kHttp30 = "3.0";
/**
 * SPDY protocol.
 */
// DEBUG: {"brief": "SPDY protocol.", "deprecated": none, "id": "spdy", "note": none, "stability":
// "experimental", "value": "SPDY"}
static constexpr const char *kSpdy = "SPDY";
/**
 * QUIC protocol.
 */
// DEBUG: {"brief": "QUIC protocol.", "deprecated": none, "id": "quic", "note": none, "stability":
// "experimental", "value": "QUIC"}
static constexpr const char *kQuic = "QUIC";
}  // namespace HttpFlavorValues

// @deprecated(reason="Deprecated in favor of stable
// `opentelemetry.semconv.attributes.http_attributes.HttpRequestMethodValues`.")  # type: ignore
// DEBUG: {"brief": "HTTP request method.", "examples": ["GET", "POST", "HEAD"], "name":
// "http.request.method", "note": "HTTP request method value SHOULD be \"known\" to the
// instrumentation.\nBy default, this convention defines \"known\" methods as the ones listed in
// [RFC9110](https://www.rfc-editor.org/rfc/rfc9110.html#name-methods)\nand the PATCH method defined
// in [RFC5789](https://www.rfc-editor.org/rfc/rfc5789.html).\n\nIf the HTTP request method is not
// known to instrumentation, it MUST set the `http.request.method` attribute to `_OTHER`.\n\nIf the
// HTTP instrumentation could end up converting valid HTTP request methods to `_OTHER`, then it MUST
// provide a way to override\nthe list of known HTTP methods. If this override is done via
// environment variable, then the environment variable MUST be
// named\nOTEL_INSTRUMENTATION_HTTP_KNOWN_METHODS and support a comma-separated list of
// case-sensitive known HTTP methods\n(this list MUST be a full override of the default known
// method, it is not a list of known methods in addition to the defaults).\n\nHTTP method names are
// case-sensitive and `http.request.method` attribute value MUST match a known HTTP method name
// exactly.\nInstrumentations for specific web frameworks that consider HTTP methods to be case
// insensitive, SHOULD populate a canonical equivalent.\nTracing instrumentations that do so, MUST
// also set `http.request.method_original` to the original value.\n", "requirement_level":
// "recommended", "root_namespace": "http", "stability": "stable", "type": {"allow_custom_values":
// true, "members": [{"brief": "CONNECT method.", "deprecated": none, "id": "connect", "note": none,
// "stability": "stable", "value": "CONNECT"}, {"brief": "DELETE method.", "deprecated": none, "id":
// "delete", "note": none, "stability": "stable", "value": "DELETE"}, {"brief": "GET method.",
// "deprecated": none, "id": "get", "note": none, "stability": "stable", "value": "GET"}, {"brief":
// "HEAD method.", "deprecated": none, "id": "head", "note": none, "stability": "stable", "value":
// "HEAD"}, {"brief": "OPTIONS method.", "deprecated": none, "id": "options", "note": none,
// "stability": "stable", "value": "OPTIONS"}, {"brief": "PATCH method.", "deprecated": none, "id":
// "patch", "note": none, "stability": "stable", "value": "PATCH"}, {"brief": "POST method.",
// "deprecated": none, "id": "post", "note": none, "stability": "stable", "value": "POST"},
// {"brief": "PUT method.", "deprecated": none, "id": "put", "note": none, "stability": "stable",
// "value": "PUT"}, {"brief": "TRACE method.", "deprecated": none, "id": "trace", "note": none,
// "stability": "stable", "value": "TRACE"}, {"brief": "Any HTTP method that the instrumentation has
// no prior knowledge of.", "deprecated": none, "id": "other", "note": none, "stability": "stable",
// "value": "_OTHER"}]}}
namespace HttpRequestMethodValues
{
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.http_attributes.HttpRequestMethodValues.kConnect @endcode.
 */
// DEBUG: {"brief": "CONNECT method.", "deprecated": none, "id": "connect", "note": none,
// "stability": "stable", "value": "CONNECT"}
static constexpr const char *kConnect = "CONNECT";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.http_attributes.HttpRequestMethodValues.kDelete @endcode.
 */
// DEBUG: {"brief": "DELETE method.", "deprecated": none, "id": "delete", "note": none, "stability":
// "stable", "value": "DELETE"}
static constexpr const char *kDelete = "DELETE";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.http_attributes.HttpRequestMethodValues.kGet @endcode.
 */
// DEBUG: {"brief": "GET method.", "deprecated": none, "id": "get", "note": none, "stability":
// "stable", "value": "GET"}
static constexpr const char *kGet = "GET";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.http_attributes.HttpRequestMethodValues.kHead @endcode.
 */
// DEBUG: {"brief": "HEAD method.", "deprecated": none, "id": "head", "note": none, "stability":
// "stable", "value": "HEAD"}
static constexpr const char *kHead = "HEAD";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.http_attributes.HttpRequestMethodValues.kOptions @endcode.
 */
// DEBUG: {"brief": "OPTIONS method.", "deprecated": none, "id": "options", "note": none,
// "stability": "stable", "value": "OPTIONS"}
static constexpr const char *kOptions = "OPTIONS";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.http_attributes.HttpRequestMethodValues.kPatch @endcode.
 */
// DEBUG: {"brief": "PATCH method.", "deprecated": none, "id": "patch", "note": none, "stability":
// "stable", "value": "PATCH"}
static constexpr const char *kPatch = "PATCH";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.http_attributes.HttpRequestMethodValues.kPost @endcode.
 */
// DEBUG: {"brief": "POST method.", "deprecated": none, "id": "post", "note": none, "stability":
// "stable", "value": "POST"}
static constexpr const char *kPost = "POST";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.http_attributes.HttpRequestMethodValues.kPut @endcode.
 */
// DEBUG: {"brief": "PUT method.", "deprecated": none, "id": "put", "note": none, "stability":
// "stable", "value": "PUT"}
static constexpr const char *kPut = "PUT";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.http_attributes.HttpRequestMethodValues.kTrace @endcode.
 */
// DEBUG: {"brief": "TRACE method.", "deprecated": none, "id": "trace", "note": none, "stability":
// "stable", "value": "TRACE"}
static constexpr const char *kTrace = "TRACE";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.http_attributes.HttpRequestMethodValues.kOther @endcode.
 */
// DEBUG: {"brief": "Any HTTP method that the instrumentation has no prior knowledge of.",
// "deprecated": none, "id": "other", "note": none, "stability": "stable", "value": "_OTHER"}
static constexpr const char *kOther = "_OTHER";
}  // namespace HttpRequestMethodValues

}  // namespace http
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
