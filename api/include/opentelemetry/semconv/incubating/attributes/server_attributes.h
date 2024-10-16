

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
namespace server
{

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.server_attributes. @endcode.
 */
static const char *kServerAddress = "server.address";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.server_attributes. @endcode.
 */
static const char *kServerPort = "server.port";

}  // namespace server
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
