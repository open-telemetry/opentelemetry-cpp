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
namespace exception
{

/**
 * SHOULD be set to true if the exception event is recorded at a point where it is known that the
 * exception is escaping the scope of the span. <p> An exception is considered to have escaped (or
 * left) the scope of a span, if that span is ended while the exception is still logically "in
 * flight". This may be actually "in flight" in some languages (e.g. if the exception is passed to a
 * Context manager's @code __exit__ @endcode method in Python) but will usually be caught at the
 * point of recording the exception in most languages. <p> It is usually not possible to determine
 * at the point where an exception is thrown whether it will escape the scope of a span. However, it
 * is trivial to know that an exception will escape, if one checks for an active exception just
 * before ending the span, as done in the <a
 * href="https://opentelemetry.io/docs/specs/semconv/exceptions/exceptions-spans/#recording-an-exception">example
 * for recording span exceptions</a>. <p> It follows that an exception may still escape the scope of
 * the span even if the @code exception.escaped @endcode attribute was not set or set to false,
 * since the event might have been recorded at a time where it was not
 * clear whether the exception will escape.
 */
static constexpr const char *kExceptionEscaped = "exception.escaped";

/**
 * The exception message.
 */
static constexpr const char *kExceptionMessage = "exception.message";

/**
 * A stacktrace as a string in the natural representation for the language runtime. The
 * representation is to be determined and documented by each language SIG.
 */
static constexpr const char *kExceptionStacktrace = "exception.stacktrace";

/**
 * The type of the exception (its fully-qualified class name, if applicable). The dynamic type of
 * the exception should be preferred over the static type in languages that support it.
 */
static constexpr const char *kExceptionType = "exception.type";

}  // namespace exception
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
