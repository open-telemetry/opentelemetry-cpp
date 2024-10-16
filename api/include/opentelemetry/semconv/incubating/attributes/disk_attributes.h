

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
namespace disk
{

/**
 * The disk IO operation direction.
 */
static const char *kDiskIoDirection = "disk.io.direction";

// DEBUG: {"brief": "The disk IO operation direction.", "examples": ["read"], "name":
// "disk.io.direction", "requirement_level": "recommended", "root_namespace": "disk", "stability":
// "experimental", "type": {"allow_custom_values": true, "members": [{"brief": none, "deprecated":
// none, "id": "read", "note": none, "stability": "experimental", "value": "read"}, {"brief": none,
// "deprecated": none, "id": "write", "note": none, "stability": "experimental", "value":
// "write"}]}}
namespace DiskIoDirectionValues
{
/**
 * read.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "read", "note": none, "stability":
// "experimental", "value": "read"}
static constexpr const char *kRead = "read";
/**
 * write.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "write", "note": none, "stability":
// "experimental", "value": "write"}
static constexpr const char *kWrite = "write";
}  // namespace DiskIoDirectionValues

}  // namespace disk
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
