

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
namespace deployment
{

/**
 * @Deprecated: Deprecated, use @code deployment.environment.name @endcode instead.
 */
static const char *kDeploymentEnvironment = "deployment.environment";

/**
 * Name of the <a href="https://wikipedia.org/wiki/Deployment_environment">deployment
 * environment</a> (aka deployment tier). Note: @code deployment.environment.name @endcode does not
 * affect the uniqueness constraints defined through the @code service.namespace @endcode, @code
 * service.name @endcode and @code service.instance.id @endcode resource attributes. This implies
 * that resources carrying the following attribute combinations MUST be considered to be identifying
 * the same service: <p> <ul> <li>@code service.name=frontend @endcode, @code
 * deployment.environment.name=production @endcode</li> <li>@code service.name=frontend @endcode,
 * @code deployment.environment.name=staging @endcode.</li>
 * </ul>
 */
static const char *kDeploymentEnvironmentName = "deployment.environment.name";

/**
 * The id of the deployment.
 */
static const char *kDeploymentId = "deployment.id";

/**
 * The name of the deployment.
 */
static const char *kDeploymentName = "deployment.name";

/**
 * The status of the deployment.
 */
static const char *kDeploymentStatus = "deployment.status";

// DEBUG: {"brief": "The status of the deployment.\n", "name": "deployment.status",
// "requirement_level": "recommended", "root_namespace": "deployment", "stability": "experimental",
// "type": {"allow_custom_values": true, "members": [{"brief": "failed", "deprecated": none, "id":
// "failed", "note": none, "stability": "experimental", "value": "failed"}, {"brief": "succeeded",
// "deprecated": none, "id": "succeeded", "note": none, "stability": "experimental", "value":
// "succeeded"}]}}
namespace DeploymentStatusValues
{
/**
 * failed.
 */
// DEBUG: {"brief": "failed", "deprecated": none, "id": "failed", "note": none, "stability":
// "experimental", "value": "failed"}
static constexpr const char *kFailed = "failed";
/**
 * succeeded.
 */
// DEBUG: {"brief": "succeeded", "deprecated": none, "id": "succeeded", "note": none, "stability":
// "experimental", "value": "succeeded"}
static constexpr const char *kSucceeded = "succeeded";
}  // namespace DeploymentStatusValues

}  // namespace deployment
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
