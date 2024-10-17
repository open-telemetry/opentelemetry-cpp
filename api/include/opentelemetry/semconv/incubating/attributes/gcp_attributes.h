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
namespace gcp
{

/**
 * Identifies the Google Cloud service for which the official client library is intended.
 * <p>
 * Intended to be a stable identifier for Google Cloud client libraries that is uniform across
 * implementation languages. The value should be derived from the canonical service domain for the
 * service; for example, 'foo.googleapis.com' should result in a value of 'foo'.
 */
static constexpr const char *kGcpClientService = "gcp.client.service";

/**
 * The name of the Cloud Run <a
 * href="https://cloud.google.com/run/docs/managing/job-executions">execution</a> being run for the
 * Job, as set by the <a
 * href="https://cloud.google.com/run/docs/container-contract#jobs-env-vars">@code
 * CLOUD_RUN_EXECUTION @endcode</a> environment variable.
 */
static constexpr const char *kGcpCloudRunJobExecution = "gcp.cloud_run.job.execution";

/**
 * The index for a task within an execution as provided by the <a
 * href="https://cloud.google.com/run/docs/container-contract#jobs-env-vars">@code
 * CLOUD_RUN_TASK_INDEX @endcode</a> environment variable.
 */
static constexpr const char *kGcpCloudRunJobTaskIndex = "gcp.cloud_run.job.task_index";

/**
 * The hostname of a GCE instance. This is the full value of the default or <a
 * href="https://cloud.google.com/compute/docs/instances/custom-hostname-vm">custom hostname</a>.
 */
static constexpr const char *kGcpGceInstanceHostname = "gcp.gce.instance.hostname";

/**
 * The instance name of a GCE instance. This is the value provided by @code host.name @endcode, the
 * visible name of the instance in the Cloud Console UI, and the prefix for the default hostname of
 * the instance as defined by the <a
 * href="https://cloud.google.com/compute/docs/internal-dns#instance-fully-qualified-domain-names">default
 * internal DNS name</a>.
 */
static constexpr const char *kGcpGceInstanceName = "gcp.gce.instance.name";

}  // namespace gcp
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
