// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

#define OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(attribute_name, attribute_value) \
  static constexpr const char *GetAttribute##attribute_name() noexcept { return attribute_value; }

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{
/**
 * Stores the Constants for semantic Attribute names for resources outlined by the OpenTelemetry
 * specifications. <see
 * href,"https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/resource/semantic_conventions/README.md"/>.
 */

class SemanticConventions final
{

public:
  // service attributes
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ServiceName, "service.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ServiceNamespace, "service.namespace");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ServiceInstance, "service.instance.id");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ServiceVersion, "service.version	");

  // telemetry attributes
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(TelemetrySdkName, "telemetry.sdk.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(TelemetrySdkLanguage, "telemetry.sdk.language");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(TelemetrySdkVersion, "telemetry.sdk.version");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(TelemetryAutoVersion, "telemetry.auto.version");

  // compute unit: container attributes
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ContainerName, "container.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ContainerId, "container.id");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ContainerRuntime, "container.runtime");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ContainerImageName, "container.image.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ContainerImageTag, "container.image.tag");

  // compute unit: faas attributes
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(FaasName, "faas.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(FaasId, "faas.id");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(FaasVersion, "faas.version");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(FaasInstance, "faas.instance");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(FaasMaxMemory, "faas.max_memory");

  // compute unit : process attributes
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ProcessId, "process.pid");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ProcessExecutableName, "process.executable.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ProcessExecutablePath, "process.executable.path");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ProcessCommand, "process.command");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ProcessCommandLine, "process.command_line");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ProcessCommandArgs, "process.command_args");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ProcessOwner, "process.owner");

  // compute : process runtimes
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ProcessRuntimeName, "process.runtime.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ProcessRuntimeVersion, "process.runtime.version");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(ProcessRuntimeDescription, "process.runtime.description");

  // compute unit : WebEngine
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(WebEngineName, "webengine.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(WebEngineVersion, "webengine.version");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(WebEngineDescription, "webengine.description");

  // compute instance : host
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(HostId, "host.id");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(HostName, "host.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(HostType, "host.type");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(HostArch, "host.arch");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(HostImageName, "host.image.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(HostImageId, "host.image.id");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(HostImageVersion, "host.image.version");

  // env os attributes
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(OsType, "os.type");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(OsDescription, "os.description");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(OsName, "os.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(OsVersion, "os.version");

  // env device attributes
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(DeviceId, "device.id");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(DeviceModelIdentifier, "device.model.identifier");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(DeviceModelName, "device.model.name");

  // env cloud
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(CloudProvider, "cloud.provider");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(CloudAccountId, "cloud.account.id");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(CloudRegion, "cloud.region");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(CloudAvailabilityZone, "cloud.availability_zone");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(CloudPlatform, "cloud.platform");

  // env deployment
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(DeploymentEnvironment, "deployment.environment");

  // env kubernetes
  //   - cluster
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sClusterName, "k8s.cluster.name");
  //   - node
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sNodeName, "k8s.node.name");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sNodeUid, "k8s.node.uid");
  //  - namespace
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sNamespaceName, "k8s.namespace.name");
  //  - pod
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sPodUid, "k8s.pod.uid");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sPodName, "k8s.pod.name");
  //  - container
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sContainerName, "k8s.container.name");
  //  - replicaset
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sReplicaSetUid, "k8s.replicaset.uid");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sReplicaSetName, "k8s.replicaset.name");
  //  - deployment
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sDeploymentUid, "k8s.deployment.uid");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sDeploymentName, "k8s.deployment.name");
  //  - stateful-set
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sStatefulSetUid, "k8s.statefulset.uid");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sStatefulSetName, "k8s.statefulset.name");
  //  - daemon set
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sDaemonSetUid, "k8s.daemonset.uid");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sDaemonSetName, "k8s.daemonset.name");
  // - job
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sJobUid, "k8s.job.uid");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(K8sJobName, "k8s.job.name");
  // - cronjob
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(CronjobUid, "k8s.cronjob.id");
  OTEL_RESOURCE_GENERATE_SEMCONV_METHOD(CronjobName, "k8s.cronjob.name");
};

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
