

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
 * HTTP request headers, @code <key> @endcode being the normalized HTTP Header name (lowercase), the
 * value being the header values. Note: Instrumentations SHOULD require an explicit configuration of
 * which headers are to be captured. Including all request headers can be a security risk - explicit
 * configuration helps avoid leaking sensitive information. The @code User-Agent @endcode header is
 * already captured in the @code user_agent.original @endcode attribute. Users MAY explicitly
 * configure instrumentations to capture them even though it is not recommended. The attribute value
 * MUST consist of either multiple header values as an array of strings or a single-item array
 * containing a possibly comma-concatenated string, depending on the way the HTTP library provides
 * access to headers.
 */
static const char *kHttpRequestHeader = "http.request.header";

/**
 * HTTP request method.
 * Note: HTTP request method value SHOULD be "known" to the instrumentation.
 * By default, this convention defines "known" methods as the ones listed in <a
 * href="https://www.rfc-editor.org/rfc/rfc9110.html#name-methods">RFC9110</a> and the PATCH method
 * defined in <a href="https://www.rfc-editor.org/rfc/rfc5789.html">RFC5789</a>. <p> If the HTTP
 * request method is not known to instrumentation, it MUST set the @code http.request.method
 * @endcode attribute to @code _OTHER @endcode. <p> If the HTTP instrumentation could end up
 * converting valid HTTP request methods to @code _OTHER @endcode, then it MUST provide a way to
 * override the list of known HTTP methods. If this override is done via environment variable, then
 * the environment variable MUST be named OTEL_INSTRUMENTATION_HTTP_KNOWN_METHODS and support a
 * comma-separated list of case-sensitive known HTTP methods (this list MUST be a full override of
 * the default known method, it is not a list of known methods in addition to the defaults). <p>
 * HTTP method names are case-sensitive and @code http.request.method @endcode attribute value MUST
 * match a known HTTP method name exactly. Instrumentations for specific web frameworks that
 * consider HTTP methods to be case insensitive, SHOULD populate a canonical equivalent. Tracing
 * instrumentations that do so, MUST also set @code http.request.method_original @endcode to the
 * original value.
 */
static const char *kHttpRequestMethod = "http.request.method";

/**
 * Original HTTP method sent by the client in the request line.
 */
static const char *kHttpRequestMethodOriginal = "http.request.method_original";

/**
 * The ordinal number of request resending attempt (for any reason, including redirects).
 * Note: The resend count SHOULD be updated each time an HTTP request gets resent by the client,
 * regardless of what was the cause of the resending (e.g. redirection, authorization failure, 503
 * Server Unavailable, network issues, or any other).
 */
static const char *kHttpRequestResendCount = "http.request.resend_count";

/**
 * HTTP response headers, @code <key> @endcode being the normalized HTTP Header name (lowercase),
 * the value being the header values. Note: Instrumentations SHOULD require an explicit
 * configuration of which headers are to be captured. Including all response headers can be a
 * security risk - explicit configuration helps avoid leaking sensitive information. Users MAY
 * explicitly configure instrumentations to capture them even though it is not recommended. The
 * attribute value MUST consist of either multiple header values as an array of strings or a
 * single-item array containing a possibly comma-concatenated string, depending on the way the HTTP
 * library provides access to headers.
 */
static const char *kHttpResponseHeader = "http.response.header";

/**
 * <a href="https://tools.ietf.org/html/rfc7231#section-6">HTTP response status code</a>.
 */
static const char *kHttpResponseStatusCode = "http.response.status_code";

/**
 * The matched route, that is, the path template in the format used by the respective server
 * framework. Note: MUST NOT be populated when this is not supported by the HTTP server framework as
 * the route attribute should have low-cardinality and the URI path can NOT substitute it. SHOULD
 * include the <a href="/docs/http/http-spans.md#http-server-definitions">application root</a> if
 * there is one.
 */
static const char *kHttpRoute = "http.route";

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
 * CONNECT method.
 */
// DEBUG: {"brief": "CONNECT method.", "deprecated": none, "id": "connect", "note": none,
// "stability": "stable", "value": "CONNECT"}
static constexpr const char *kConnect = "CONNECT";
/**
 * DELETE method.
 */
// DEBUG: {"brief": "DELETE method.", "deprecated": none, "id": "delete", "note": none, "stability":
// "stable", "value": "DELETE"}
static constexpr const char *kDelete = "DELETE";
/**
 * GET method.
 */
// DEBUG: {"brief": "GET method.", "deprecated": none, "id": "get", "note": none, "stability":
// "stable", "value": "GET"}
static constexpr const char *kGet = "GET";
/**
 * HEAD method.
 */
// DEBUG: {"brief": "HEAD method.", "deprecated": none, "id": "head", "note": none, "stability":
// "stable", "value": "HEAD"}
static constexpr const char *kHead = "HEAD";
/**
 * OPTIONS method.
 */
// DEBUG: {"brief": "OPTIONS method.", "deprecated": none, "id": "options", "note": none,
// "stability": "stable", "value": "OPTIONS"}
static constexpr const char *kOptions = "OPTIONS";
/**
 * PATCH method.
 */
// DEBUG: {"brief": "PATCH method.", "deprecated": none, "id": "patch", "note": none, "stability":
// "stable", "value": "PATCH"}
static constexpr const char *kPatch = "PATCH";
/**
 * POST method.
 */
// DEBUG: {"brief": "POST method.", "deprecated": none, "id": "post", "note": none, "stability":
// "stable", "value": "POST"}
static constexpr const char *kPost = "POST";
/**
 * PUT method.
 */
// DEBUG: {"brief": "PUT method.", "deprecated": none, "id": "put", "note": none, "stability":
// "stable", "value": "PUT"}
static constexpr const char *kPut = "PUT";
/**
 * TRACE method.
 */
// DEBUG: {"brief": "TRACE method.", "deprecated": none, "id": "trace", "note": none, "stability":
// "stable", "value": "TRACE"}
static constexpr const char *kTrace = "TRACE";
/**
 * Any HTTP method that the instrumentation has no prior knowledge of.
 */
// DEBUG: {"brief": "Any HTTP method that the instrumentation has no prior knowledge of.",
// "deprecated": none, "id": "other", "note": none, "stability": "stable", "value": "_OTHER"}
static constexpr const char *kOther = "_OTHER";
}  // namespace HttpRequestMethodValues

}  // namespace http
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
