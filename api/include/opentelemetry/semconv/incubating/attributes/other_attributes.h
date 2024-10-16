

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
namespace other
{

/**
 * @Deprecated: Replaced by @code db.client.connection.state @endcode.
 */
static const char *kState = "state";

// @deprecated(reason="The attribute state is deprecated - Replaced by
// `db.client.connection.state`")  # type: ignore DEBUG: {"brief": "Deprecated, use
// `db.client.connection.state` instead.", "deprecated": "Replaced by
// `db.client.connection.state`.", "examples": ["idle"], "name": "state", "requirement_level":
// "recommended", "root_namespace": "other", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id": "idle",
// "note": none, "stability": "experimental", "value": "idle"}, {"brief": none, "deprecated": none,
// "id": "used", "note": none, "stability": "experimental", "value": "used"}]}}
namespace StateValues
{
/**
 * idle.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "idle", "note": none, "stability":
// "experimental", "value": "idle"}
static constexpr const char *kIdle = "idle";
/**
 * used.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "used", "note": none, "stability":
// "experimental", "value": "used"}
static constexpr const char *kUsed = "used";
}  // namespace StateValues

}  // namespace other
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
