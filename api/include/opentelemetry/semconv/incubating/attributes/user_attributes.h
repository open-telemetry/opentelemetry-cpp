

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
namespace user
{

/**
 * User email address.
 */
static const char *kUserEmail = "user.email";

/**
 * User's full name.
 */
static const char *kUserFullName = "user.full_name";

/**
 * Unique user hash to correlate information for a user in anonymized form.
 * Note: Useful if @code user.id @endcode or @code user.name @endcode contain confidential
 * information and cannot be used.
 */
static const char *kUserHash = "user.hash";

/**
 * Unique identifier of the user.
 */
static const char *kUserId = "user.id";

/**
 * Short name or login/username of the user.
 */
static const char *kUserName = "user.name";

/**
 * Array of user roles at the time of the event.
 */
static const char *kUserRoles = "user.roles";

}  // namespace user
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
