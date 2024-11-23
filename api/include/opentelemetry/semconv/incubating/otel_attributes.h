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
namespace otel
{

/**
 * Deprecated. Use the @code otel.scope.name @endcode attribute
 * <p>
 * @deprecated
 * Use the @code otel.scope.name @endcode attribute.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kOtelLibraryName = "otel.library.name";

/**
 * Deprecated. Use the @code otel.scope.version @endcode attribute.
 * <p>
 * @deprecated
 * Use the @code otel.scope.version @endcode attribute.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kOtelLibraryVersion = "otel.library.version";

/**
 * The name of the instrumentation scope - (@code InstrumentationScope.Name @endcode in OTLP).
 */
static constexpr const char *kOtelScopeName = "otel.scope.name";

/**
 * The version of the instrumentation scope - (@code InstrumentationScope.Version @endcode in OTLP).
 */
static constexpr const char *kOtelScopeVersion = "otel.scope.version";

/**
 * Name of the code, either "OK" or "ERROR". MUST NOT be set if the status code is UNSET.
 */
static constexpr const char *kOtelStatusCode = "otel.status_code";

/**
 * Description of the Status if it has a value, otherwise not set.
 */
static constexpr const char *kOtelStatusDescription = "otel.status_description";

namespace OtelStatusCodeValues
{
/**
 * The operation has been validated by an Application developer or Operator to have completed
 * successfully.
 */
static constexpr const char *kOk = "OK";

/**
 * The operation contains an error.
 */
static constexpr const char *kError = "ERROR";

}  // namespace OtelStatusCodeValues

}  // namespace otel
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
