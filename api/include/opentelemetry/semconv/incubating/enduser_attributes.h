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
namespace enduser
{

/**
 * Deprecated, use @code user.id @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code user.id @endcode attribute.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kEnduserId = "enduser.id";

/**
 * Deprecated, use @code user.roles @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code user.roles @endcode attribute.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kEnduserRole = "enduser.role";

/**
 * Deprecated, no replacement at this time.
 * <p>
 * @deprecated
 * Removed.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kEnduserScope = "enduser.scope";

}  // namespace enduser
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
