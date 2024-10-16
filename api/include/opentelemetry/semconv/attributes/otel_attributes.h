

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
 * The name of the instrumentation scope - (@code InstrumentationScope.Name @endcode in OTLP).
 */
static const char *kOtelScopeName = "otel.scope.name";

/**
 * The version of the instrumentation scope - (@code InstrumentationScope.Version @endcode in OTLP).
 */
static const char *kOtelScopeVersion = "otel.scope.version";

/**
 * Name of the code, either "OK" or "ERROR". MUST NOT be set if the status code is UNSET.
 */
static const char *kOtelStatusCode = "otel.status_code";

/**
 * Description of the Status if it has a value, otherwise not set.
 */
static const char *kOtelStatusDescription = "otel.status_description";

// DEBUG: {"brief": "Name of the code, either \"OK\" or \"ERROR\". MUST NOT be set if the status
// code is UNSET.", "name": "otel.status_code", "requirement_level": "recommended",
// "root_namespace": "otel", "stability": "stable", "type": {"allow_custom_values": true, "members":
// [{"brief": "The operation has been validated by an Application developer or Operator to have
// completed successfully.", "deprecated": none, "id": "ok", "note": none, "stability": "stable",
// "value": "OK"}, {"brief": "The operation contains an error.", "deprecated": none, "id": "error",
// "note": none, "stability": "stable", "value": "ERROR"}]}}
namespace OtelStatusCodeValues
{
/**
 * The operation has been validated by an Application developer or Operator to have completed
 * successfully.
 */
// DEBUG: {"brief": "The operation has been validated by an Application developer or Operator to
// have completed successfully.", "deprecated": none, "id": "ok", "note": none, "stability":
// "stable", "value": "OK"}
static constexpr const char *kOk = "OK";
/**
 * The operation contains an error.
 */
// DEBUG: {"brief": "The operation contains an error.", "deprecated": none, "id": "error", "note":
// none, "stability": "stable", "value": "ERROR"}
static constexpr const char *kError = "ERROR";
}  // namespace OtelStatusCodeValues

}  // namespace otel
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
