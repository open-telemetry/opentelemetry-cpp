

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
namespace url
{

/**
 * Domain extracted from the @code url.full @endcode, such as "opentelemetry.io".
 * Note: In some cases a URL may refer to an IP and/or port directly, without a domain name. In this
 * case, the IP address would go to the domain field. If the URL contains a <a
 * href="https://www.rfc-editor.org/rfc/rfc2732#section-2">literal IPv6 address</a> enclosed by
 * @code [ @endcode and @code ] @endcode, the @code [ @endcode and @code ] @endcode characters
 * should also be captured in the domain field.
 */
static const char *kUrlDomain = "url.domain";

/**
 * The file extension extracted from the @code url.full @endcode, excluding the leading dot.
 * Note: The file extension is only set if it exists, as not every url has a file extension. When
 * the file name has multiple extensions @code example.tar.gz @endcode, only the last one should be
 * captured @code gz @endcode, not @code tar.gz @endcode.
 */
static const char *kUrlExtension = "url.extension";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.url_attributes.
 * @endcode.
 */
static const char *kUrlFragment = "url.fragment";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.url_attributes.
 * @endcode.
 */
static const char *kUrlFull = "url.full";

/**
 * Unmodified original URL as seen in the event source.
 * Note: In network monitoring, the observed URL may be a full URL, whereas in access logs, the URL
 * is often just represented as a path. This field is meant to represent the URL as it was observed,
 * complete or not.
 * @code url.original @endcode might contain credentials passed via URL in form of @code
 * https://username:password@www.example.com/ @endcode. In such case password and username SHOULD
 * NOT be redacted and attribute's value SHOULD remain the same.
 */
static const char *kUrlOriginal = "url.original";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.url_attributes.
 * @endcode.
 */
static const char *kUrlPath = "url.path";

/**
 * Port extracted from the @code url.full @endcode.
 */
static const char *kUrlPort = "url.port";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.url_attributes.
 * @endcode.
 */
static const char *kUrlQuery = "url.query";

/**
 * The highest registered url domain, stripped of the subdomain.
 * Note: This value can be determined precisely with the <a href="http://publicsuffix.org">public
 * suffix list</a>. For example, the registered domain for @code foo.example.com @endcode is @code
 * example.com @endcode. Trying to approximate this by simply taking the last two labels will not
 * work well for TLDs such as @code co.uk @endcode.
 */
static const char *kUrlRegisteredDomain = "url.registered_domain";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.url_attributes.
 * @endcode.
 */
static const char *kUrlScheme = "url.scheme";

/**
 * The subdomain portion of a fully qualified domain name includes all of the names except the host
 * name under the registered_domain. In a partially qualified domain, or if the qualification level
 * of the full name cannot be determined, subdomain contains all of the names below the registered
 * domain. Note: The subdomain portion of @code www.east.mydomain.co.uk @endcode is @code east
 * @endcode. If the domain has multiple levels of subdomain, such as @code sub2.sub1.example.com
 * @endcode, the subdomain field should contain @code sub2.sub1 @endcode, with no trailing period.
 */
static const char *kUrlSubdomain = "url.subdomain";

/**
 * The low-cardinality template of an <a
 * href="https://www.rfc-editor.org/rfc/rfc3986#section-4.2">absolute path reference</a>.
 */
static const char *kUrlTemplate = "url.template";

/**
 * The effective top level domain (eTLD), also known as the domain suffix, is the last part of the
 * domain name. For example, the top level domain for example.com is @code com @endcode. Note: This
 * value can be determined precisely with the <a href="http://publicsuffix.org">public suffix
 * list</a>.
 */
static const char *kUrlTopLevelDomain = "url.top_level_domain";

}  // namespace url
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
