

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
namespace container
{

/**
 * The command used to run the container (i.e. the command name).
 * Note: If using embedded credentials or sensitive data, it is recommended to remove them to
 * prevent potential leakage.
 */
static const char *kContainerCommand = "container.command";

/**
 * All the command arguments (including the command/executable itself) run by the container. [2].
 */
static const char *kContainerCommandArgs = "container.command_args";

/**
 * The full command run by the container as a single string representing the full command. [2].
 */
static const char *kContainerCommandLine = "container.command_line";

/**
 * @Deprecated: Replaced by @code cpu.mode @endcode.
 */
static const char *kContainerCpuState = "container.cpu.state";

/**
 * Container ID. Usually a UUID, as for example used to <a
 * href="https://docs.docker.com/engine/reference/run/#container-identification">identify Docker
 * containers</a>. The UUID might be abbreviated.
 */
static const char *kContainerId = "container.id";

/**
 * Runtime specific image identifier. Usually a hash algorithm followed by a UUID.
 * Note: Docker defines a sha256 of the image id; @code container.image.id @endcode corresponds to
 * the @code Image @endcode field from the Docker container inspect <a
 * href="https://docs.docker.com/engine/api/v1.43/#tag/Container/operation/ContainerInspect">API</a>
 * endpoint. K8s defines a link to the container registry repository with digest @code "imageID":
 * "registry.azurecr.io
 * /namespace/service/dockerfile@sha256:bdeabd40c3a8a492eaf9e8e44d0ebbb84bac7ee25ac0cf8a7159d25f62555625"
 * @endcode. The ID is assigned by the container runtime and can vary in different environments.
 * Consider using @code oci.manifest.digest @endcode if it is important to identify the same image
 * in different environments/runtimes.
 */
static const char *kContainerImageId = "container.image.id";

/**
 * Name of the image the container was built on.
 */
static const char *kContainerImageName = "container.image.name";

/**
 * Repo digests of the container image as provided by the container runtime.
 * Note: <a
 * href="https://docs.docker.com/engine/api/v1.43/#tag/Image/operation/ImageInspect">Docker</a> and
 * <a
 * href="https://github.com/kubernetes/cri-api/blob/c75ef5b473bbe2d0a4fc92f82235efd665ea8e9f/pkg/apis/runtime/v1/api.proto#L1237-L1238">CRI</a>
 * report those under the @code RepoDigests @endcode field.
 */
static const char *kContainerImageRepoDigests = "container.image.repo_digests";

/**
 * Container image tags. An example can be found in <a
 * href="https://docs.docker.com/engine/api/v1.43/#tag/Image/operation/ImageInspect">Docker Image
 * Inspect</a>. Should be only the @code <tag> @endcode section of the full name for example from
 * @code registry.example.com/my-org/my-image:<tag> @endcode.
 */
static const char *kContainerImageTags = "container.image.tags";

/**
 * Container labels, @code <key> @endcode being the label name, the value being the label value.
 */
static const char *kContainerLabel = "container.label";

/**
 * @Deprecated: Replaced by @code container.label @endcode.
 */
static const char *kContainerLabels = "container.labels";

/**
 * Container name used by container runtime.
 */
static const char *kContainerName = "container.name";

/**
 * The container runtime managing this container.
 */
static const char *kContainerRuntime = "container.runtime";

// @deprecated(reason="The attribute container.cpu.state is deprecated - Replaced by `cpu.mode`")  #
// type: ignore DEBUG: {"brief": "Deprecated, use `cpu.mode` instead.", "deprecated": "Replaced by
// `cpu.mode`", "examples": ["user", "kernel"], "name": "container.cpu.state", "requirement_level":
// "recommended", "root_namespace": "container", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": "When tasks of the cgroup are in user mode
// (Linux). When all container processes are in user mode (Windows).", "deprecated": none, "id":
// "user", "note": none, "stability": "experimental", "value": "user"}, {"brief": "When CPU is used
// by the system (host OS)", "deprecated": none, "id": "system", "note": none, "stability":
// "experimental", "value": "system"}, {"brief": "When tasks of the cgroup are in kernel mode
// (Linux). When all container processes are in kernel mode (Windows).", "deprecated": none, "id":
// "kernel", "note": none, "stability": "experimental", "value": "kernel"}]}}
namespace ContainerCpuStateValues
{
/**
 * When tasks of the cgroup are in user mode (Linux). When all container processes are in user mode
 * (Windows).
 */
// DEBUG: {"brief": "When tasks of the cgroup are in user mode (Linux). When all container processes
// are in user mode (Windows).", "deprecated": none, "id": "user", "note": none, "stability":
// "experimental", "value": "user"}
static constexpr const char *kUser = "user";
/**
 * When CPU is used by the system (host OS).
 */
// DEBUG: {"brief": "When CPU is used by the system (host OS)", "deprecated": none, "id": "system",
// "note": none, "stability": "experimental", "value": "system"}
static constexpr const char *kSystem = "system";
/**
 * When tasks of the cgroup are in kernel mode (Linux). When all container processes are in kernel
 * mode (Windows).
 */
// DEBUG: {"brief": "When tasks of the cgroup are in kernel mode (Linux). When all container
// processes are in kernel mode (Windows).", "deprecated": none, "id": "kernel", "note": none,
// "stability": "experimental", "value": "kernel"}
static constexpr const char *kKernel = "kernel";
}  // namespace ContainerCpuStateValues

}  // namespace container
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
