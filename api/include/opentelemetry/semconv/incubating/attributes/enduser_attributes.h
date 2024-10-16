

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
 * @Deprecated: Replaced by @code user.id @endcode attribute.
 */
static const char *kEnduserId = "enduser.id";

/**
 * @Deprecated: Replaced by @code user.roles @endcode attribute.
 */
static const char *kEnduserRole = "enduser.role";

/**
 * @Deprecated: Removed.
 */
static const char *kEnduserScope = "enduser.scope";

}  // namespace enduser
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
