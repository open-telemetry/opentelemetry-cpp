// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// NOTE:
// This implementation is based on the experimental specs for resource semantic convention as
// defined here:
// https://github.com/open-telemetry/opentelemetry-specification/tree/v1.0.0/specification/resource/semantic_conventions
// and MAY will change in future.

#pragma once

#include <type_traits>
#include <unordered_map>

#include "opentelemetry/common/string_util.h"
#include "opentelemetry/version.h"

#define OTEL_GET_RESOURCE_ATTR(name) \
  opentelemetry::sdk::resource::attr(OTEL_CPP_CONST_HASHCODE(name))

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

static const std::unordered_map<uint32_t, const char *> attribute_ids = {
    {OTEL_CPP_CONST_HASHCODE(AttrServiceName), "service.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrServiceNamespace), "service.namespace"},
    {OTEL_CPP_CONST_HASHCODE(AttrServiceInstance), "service.instance.id"},
    {OTEL_CPP_CONST_HASHCODE(AttrServiceVersion), "service.version"},

    // telemetry attributes
    {OTEL_CPP_CONST_HASHCODE(AttrTelemetrySdkName), "telemetry.sdk.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrTelemetrySdkLanguage), "telemetry.sdk.language"},
    {OTEL_CPP_CONST_HASHCODE(AttrTelemetrySdkVersion), "telemetry.sdk.version"},
    {OTEL_CPP_CONST_HASHCODE(AttrTelemetryAutoVersion), "telemetry.auto.version"},

    // compute unit: container attributes
    {OTEL_CPP_CONST_HASHCODE(AttrContainerName), "container.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrContainerId), "container.id"},
    {OTEL_CPP_CONST_HASHCODE(AttrContainerRuntime), "container.runtime"},
    {OTEL_CPP_CONST_HASHCODE(AttrContainerImageName), "container.image.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrContainerImageTag), "container.image.tag"},

    // compute unit: faas attributes
    {OTEL_CPP_CONST_HASHCODE(AttrFaasName), "faas.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrFaasId), "faas.id"},
    {OTEL_CPP_CONST_HASHCODE(AttrFaasVersion), "faas.version"},
    {OTEL_CPP_CONST_HASHCODE(AttrFaasInstance), "faas.instance"},
    {OTEL_CPP_CONST_HASHCODE(AttrFaasMaxMemory), "faas.max_memory"},

    // compute unit : process attributes
    {OTEL_CPP_CONST_HASHCODE(AttrProcessId), "process.pid"},
    {OTEL_CPP_CONST_HASHCODE(AttrProcessExecutableName), "process.executable.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrProcessExecutablePath), "process.executable.path"},
    {OTEL_CPP_CONST_HASHCODE(AttrProcessCommand), "process.command"},
    {OTEL_CPP_CONST_HASHCODE(AttrProcessCommandLine), "process.command_line"},
    {OTEL_CPP_CONST_HASHCODE(AttrProcessCommandArgs), "process.command_args"},
    {OTEL_CPP_CONST_HASHCODE(AttrProcessOwner), "process.owner"},

    // compute : process runtimes
    {OTEL_CPP_CONST_HASHCODE(AttrProcessRuntimeName), "process.runtime.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrProcessRuntimeVersion), "process.runtime.version"},
    {OTEL_CPP_CONST_HASHCODE(AttrProcessRuntimeDescription), "process.runtime.description"},

    // compute unit : WebEngine
    {OTEL_CPP_CONST_HASHCODE(AttrWebEngineName), "webengine.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrWebEngineVersion), "webengine.version"},
    {OTEL_CPP_CONST_HASHCODE(AttrWebEngineDescription), "webengine.description"},

    // compute instance : host
    {OTEL_CPP_CONST_HASHCODE(AttrHostId), "host.id"},
    {OTEL_CPP_CONST_HASHCODE(AttrHostName), "host.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrHostType), "host.type"},
    {OTEL_CPP_CONST_HASHCODE(AttrHostArch), "host.arch"},
    {OTEL_CPP_CONST_HASHCODE(AttrHostImageName), "host.image.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrHostImageId), "host.image.id"},
    {OTEL_CPP_CONST_HASHCODE(AttrHostImageVersion), "host.image.version"},

    // env os attributes
    {OTEL_CPP_CONST_HASHCODE(AttrOsType), "os.type"},
    {OTEL_CPP_CONST_HASHCODE(AttrOsDescription), "os.description"},
    {OTEL_CPP_CONST_HASHCODE(AttrOsName), "os.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrOsVersion), "os.version"},

    // env device attributes
    {OTEL_CPP_CONST_HASHCODE(AttrDeviceId), "device.id"},
    {OTEL_CPP_CONST_HASHCODE(AttrDeviceModelIdentifier), "device.model.identifier"},
    {OTEL_CPP_CONST_HASHCODE(AttrDeviceModelName), "device.model.name"},

    // env cloud
    {OTEL_CPP_CONST_HASHCODE(AttrCloudProvider), "cloud.provider"},
    {OTEL_CPP_CONST_HASHCODE(AttrCloudAccountId), "cloud.account.id"},
    {OTEL_CPP_CONST_HASHCODE(AttrCloudRegion), "cloud.region"},
    {OTEL_CPP_CONST_HASHCODE(AttrCloudAvailabilityZone), "cloud.availability_zone"},
    {OTEL_CPP_CONST_HASHCODE(AttrCloudPlatform), "cloud.platform"},

    // env deployment
    {OTEL_CPP_CONST_HASHCODE(AttrDeploymentEnvironment), "deployment.environment"},

    // env kubernetes
    //   - cluster
    {OTEL_CPP_CONST_HASHCODE(AttrK8sClusterName), "k8s.cluster.name"},
    //   - node
    {OTEL_CPP_CONST_HASHCODE(AttrK8sNodeName), "k8s.node.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrK8sNodeUid), "k8s.node.uid"},
    //  - namespace
    {OTEL_CPP_CONST_HASHCODE(AttrK8sNamespaceName), "k8s.namespace.name"},
    //  - pod
    {OTEL_CPP_CONST_HASHCODE(AttrK8sPodUid), "k8s.pod.uid"},
    {OTEL_CPP_CONST_HASHCODE(AttrK8sPodName), "k8s.pod.name"},
    //  - container
    {OTEL_CPP_CONST_HASHCODE(AttrK8sContainerName), "k8s.container.name"},
    //  - replicaset
    {OTEL_CPP_CONST_HASHCODE(AttrK8sReplicaSetUid), "k8s.replicaset.uid"},
    {OTEL_CPP_CONST_HASHCODE(AttrK8sReplicaSetName), "k8s.replicaset.name"},
    //  - deployment
    {OTEL_CPP_CONST_HASHCODE(AttrK8sDeploymentUid), "k8s.deployment.uid"},
    {OTEL_CPP_CONST_HASHCODE(AttrK8sDeploymentName), "k8s.deployment.name"},
    //  - stateful-set
    {OTEL_CPP_CONST_HASHCODE(AttrK8sStatefulSetUid), "k8s.statefulset.uid"},
    {OTEL_CPP_CONST_HASHCODE(AttrK8sStatefulSetName), "k8s.statefulset.name"},
    //  - daemon set
    {OTEL_CPP_CONST_HASHCODE(AttrK8sDaemonSetUid), "k8s.daemonset.uid"},
    {OTEL_CPP_CONST_HASHCODE(AttrK8sDaemonSetName), "k8s.daemonset.name"},
    // - job
    {OTEL_CPP_CONST_HASHCODE(AttrK8sJobUid), "k8s.job.uid"},
    {OTEL_CPP_CONST_HASHCODE(AttrK8sJobName), "k8s.job.name"},
    // - cronjob
    {OTEL_CPP_CONST_HASHCODE(AttrCronjobUid), "k8s.cronjob.id"},
    {OTEL_CPP_CONST_HASHCODE(AttrCronjobName), "k8s.cronjob.name"}};

// function to generate hash code for semantic conventions attributes.
inline const char *attr(uint32_t attr)
{
  return (attribute_ids.find(attr) != attribute_ids.end()) ? attribute_ids.at(attr) : "";
}
}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE