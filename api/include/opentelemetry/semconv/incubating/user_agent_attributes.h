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
namespace user_agent
{

/**
 * Name of the user-agent extracted from original. Usually refers to the browser's name.
 * <p>
 * <a href="https://www.whatsmyua.info">Example</a> of extracting browser's name from original
 * string. In the case of using a user-agent for non-browser products, such as microservices with
 * multiple names/versions inside the @code user_agent.original @endcode, the most significant name
 * SHOULD be selected. In such a scenario it should align with @code user_agent.version @endcode
 */
static constexpr const char *kUserAgentName = "user_agent.name";

/**
 * Value of the <a href="https://www.rfc-editor.org/rfc/rfc9110.html#field.user-agent">HTTP
 * User-Agent</a> header sent by the client.
 */
static constexpr const char *kUserAgentOriginal = "user_agent.original";

/**
 * Version of the user-agent extracted from original. Usually refers to the browser's version
 * <p>
 * <a href="https://www.whatsmyua.info">Example</a> of extracting browser's version from original
 * string. In the case of using a user-agent for non-browser products, such as microservices with
 * multiple names/versions inside the @code user_agent.original @endcode, the most significant
 * version SHOULD be selected. In such a scenario it should align with @code user_agent.name
 * @endcode
 */
static constexpr const char *kUserAgentVersion = "user_agent.version";

}  // namespace user_agent
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
