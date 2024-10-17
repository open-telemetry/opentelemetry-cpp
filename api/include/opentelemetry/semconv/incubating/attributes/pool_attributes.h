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
namespace pool
{

/**
 * Deprecated, use @code db.client.connection.pool.name @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.client.connection.pool.name @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kPoolName = "pool.name";

}  // namespace pool
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
