

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
 * @Deprecated: use the @code otel.scope.name @endcode attribute.
 */
static const char *kOtelLibraryName = "otel.library.name";

/**
 * @Deprecated: use the @code otel.scope.version @endcode attribute.
 */
static const char *kOtelLibraryVersion = "otel.library.version";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.otel_attributes.
 * @endcode.
 */
static const char *kOtelScopeName = "otel.scope.name";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.otel_attributes.
 * @endcode.
 */
static const char *kOtelScopeVersion = "otel.scope.version";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.otel_attributes.
 * @endcode.
 */
static const char *kOtelStatusCode = "otel.status_code";

/**
 * @Deprecated in favor of stable :py:const:@code opentelemetry.semconv.attributes.otel_attributes.
 * @endcode.
 */
static const char *kOtelStatusDescription = "otel.status_description";

// @deprecated(reason="Deprecated in favor of stable
// `opentelemetry.semconv.attributes.otel_attributes.OtelStatusCodeValues`.")  # type: ignore DEBUG:
// {"brief": "Name of the code, either \"OK\" or \"ERROR\". MUST NOT be set if the status code is
// UNSET.", "name": "otel.status_code", "requirement_level": "recommended", "root_namespace":
// "otel", "stability": "stable", "type": {"allow_custom_values": true, "members": [{"brief": "The
// operation has been validated by an Application developer or Operator to have completed
// successfully.", "deprecated": none, "id": "ok", "note": none, "stability": "stable", "value":
// "OK"}, {"brief": "The operation contains an error.", "deprecated": none, "id": "error", "note":
// none, "stability": "stable", "value": "ERROR"}]}}
namespace OtelStatusCodeValues
{
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.otel_attributes.OtelStatusCodeValues.kOk @endcode.
 */
// DEBUG: {"brief": "The operation has been validated by an Application developer or Operator to
// have completed successfully.", "deprecated": none, "id": "ok", "note": none, "stability":
// "stable", "value": "OK"}
static constexpr const char *kOk = "OK";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.otel_attributes.OtelStatusCodeValues.kError @endcode.
 */
// DEBUG: {"brief": "The operation contains an error.", "deprecated": none, "id": "error", "note":
// none, "stability": "stable", "value": "ERROR"}
static constexpr const char *kError = "ERROR";
}  // namespace OtelStatusCodeValues

}  // namespace otel
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
