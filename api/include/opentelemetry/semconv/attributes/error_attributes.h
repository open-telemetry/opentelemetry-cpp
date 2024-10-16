

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
namespace error
{

/**
 * Describes a class of error the operation ended with.
 * Note: The @code error.type @endcode SHOULD be predictable, and SHOULD have low cardinality.
 * <p>
 * When @code error.type @endcode is set to a type (e.g., an exception type), its
 * canonical class name identifying the type within the artifact SHOULD be used.
 * <p>
 * Instrumentations SHOULD document the list of errors they report.
 * <p>
 * The cardinality of @code error.type @endcode within one instrumentation library SHOULD be low.
 * Telemetry consumers that aggregate data from multiple instrumentation libraries and applications
 * should be prepared for @code error.type @endcode to have high cardinality at query time when no
 * additional filters are applied.
 * <p>
 * If the operation has completed successfully, instrumentations SHOULD NOT set @code error.type
 * @endcode. <p> If a specific domain defines its own set of error identifiers (such as HTTP or gRPC
 * status codes), it's RECOMMENDED to: <p> <ul> <li>Use a domain-specific attribute</li> <li>Set
 * @code error.type @endcode to capture all errors, regardless of whether they are defined within
 * the domain-specific set or not.</li>
 * </ul>
 */
static const char *kErrorType = "error.type";

// DEBUG: {"brief": "Describes a class of error the operation ended with.\n", "examples":
// ["timeout", "java.net.UnknownHostException", "server_certificate_invalid", "500"], "name":
// "error.type", "note": "The `error.type` SHOULD be predictable, and SHOULD have low
// cardinality.\n\nWhen `error.type` is set to a type (e.g., an exception type), its\ncanonical
// class name identifying the type within the artifact SHOULD be used.\n\nInstrumentations SHOULD
// document the list of errors they report.\n\nThe cardinality of `error.type` within one
// instrumentation library SHOULD be low.\nTelemetry consumers that aggregate data from multiple
// instrumentation libraries and applications\nshould be prepared for `error.type` to have high
// cardinality at query time when no\nadditional filters are applied.\n\nIf the operation has
// completed successfully, instrumentations SHOULD NOT set `error.type`.\n\nIf a specific domain
// defines its own set of error identifiers (such as HTTP or gRPC status codes),\nit's RECOMMENDED
// to:\n\n* Use a domain-specific attribute\n* Set `error.type` to capture all errors, regardless of
// whether they are defined within the domain-specific set or not.\n", "requirement_level":
// "recommended", "root_namespace": "error", "stability": "stable", "type": {"allow_custom_values":
// true, "members": [{"brief": "A fallback error value to be used when the instrumentation doesn't
// define a custom value.\n", "deprecated": none, "id": "other", "note": none, "stability":
// "stable", "value": "_OTHER"}]}}
namespace ErrorTypeValues
{
/**
 * A fallback error value to be used when the instrumentation doesn't define a custom value.
 */
// DEBUG: {"brief": "A fallback error value to be used when the instrumentation doesn't define a
// custom value.\n", "deprecated": none, "id": "other", "note": none, "stability": "stable",
// "value": "_OTHER"}
static constexpr const char *kOther = "_OTHER";
}  // namespace ErrorTypeValues

}  // namespace error
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
