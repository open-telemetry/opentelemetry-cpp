

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
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.exception_attributes. @endcode.
 */
static const char *kExceptionEscaped = "exception.escaped";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.exception_attributes. @endcode.
 */
static const char *kExceptionMessage = "exception.message";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.exception_attributes. @endcode.
 */
static const char *kExceptionStacktrace = "exception.stacktrace";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.exception_attributes. @endcode.
 */
static const char *kExceptionType = "exception.type";

}  // namespace exception
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
