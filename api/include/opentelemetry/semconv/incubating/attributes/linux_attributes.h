

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
namespace linux
{

/**
 * The Linux Slab memory state.
 */
static const char *kLinuxMemorySlabState = "linux.memory.slab.state";

// DEBUG: {"brief": "The Linux Slab memory state", "examples": ["reclaimable", "unreclaimable"],
// "name": "linux.memory.slab.state", "requirement_level": "recommended", "root_namespace": "linux",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": none,
// "deprecated": none, "id": "reclaimable", "note": none, "stability": "experimental", "value":
// "reclaimable"}, {"brief": none, "deprecated": none, "id": "unreclaimable", "note": none,
// "stability": "experimental", "value": "unreclaimable"}]}}
namespace LinuxMemorySlabStateValues
{
/**
 * reclaimable.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "reclaimable", "note": none, "stability":
// "experimental", "value": "reclaimable"}
static constexpr const char *kReclaimable = "reclaimable";
/**
 * unreclaimable.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "unreclaimable", "note": none, "stability":
// "experimental", "value": "unreclaimable"}
static constexpr const char *kUnreclaimable = "unreclaimable";
}  // namespace LinuxMemorySlabStateValues

}  // namespace linux
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
