/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

/*
  DO NOT EDIT, this is an Auto-generated file
  from buildscripts/semantic-convention/templates/SemanticAttributes.h.j2
*/

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

namespace SemanticConventions
{
/**
 * The URL of the OpenTelemetry schema for these keys and values.
 */
static constexpr const char *kSchemaUrl = "https://opentelemetry.io/schemas/1.17.0";

/**
 * Array of brand name and version separated by a space
 *
 * <p>Notes:
  <ul> <li>This value is intended to be taken from the <a
 href="https://wicg.github.io/ua-client-hints/#interface">UA client hints API</a> ({@code
 navigator.userAgentData.brands}).</li> </ul>
 */
static constexpr const char *kBrowserBrands = "browser.brands";

/**
 * The platform on which the browser is running
 *
 * <p>Notes:
  <ul> <li>This value is intended to be taken from the <a
href="https://wicg.github.io/ua-client-hints/#interface">UA client hints API</a> ({@code
navigator.userAgentData.platform}). If unavailable, the legacy {@code navigator.platform} API SHOULD
NOT be used instead and this attribute SHOULD be left unset in order for the values to be
consistent. The list of possible values is defined in the <a
href="https://wicg.github.io/ua-client-hints/#sec-ch-ua-platform">W3C User-Agent Client Hints
specification</a>. Note that some (but not all) of these values can overlap with values in the <a
href="./os.md">{@code os.type} and {@code os.name} attributes</a>. However, for consistency, the
values in the {@code browser.platform} attribute should capture the exact value that the user agent
provides.</li> </ul>
 */
static constexpr const char *kBrowserPlatform = "browser.platform";

/**
 * A boolean that is true if the browser is running on a mobile device
 *
 * <p>Notes:
  <ul> <li>This value is intended to be taken from the <a
 href="https://wicg.github.io/ua-client-hints/#interface">UA client hints API</a> ({@code
 navigator.userAgentData.mobile}). If unavailable, this attribute SHOULD be left unset.</li> </ul>
 */
static constexpr const char *kBrowserMobile = "browser.mobile";

/**
 * Full user-agent string provided by the browser
 *
 * <p>Notes:
  <ul> <li>The user-agent value SHOULD be provided only from browsers that do not have a mechanism
 to retrieve brands and platform individually from the User-Agent Client Hints API. To retrieve the
 value, the legacy {@code navigator.userAgent} API can be used.</li> </ul>
 */
static constexpr const char *kBrowserUserAgent = "browser.user_agent";

/**
 * Preferred language of the user using the browser
 *
 * <p>Notes:
  <ul> <li>This value is intended to be taken from the Navigator API {@code
 navigator.language}.</li> </ul>
 */
static constexpr const char *kBrowserLanguage = "browser.language";

/**
 * Name of the cloud provider.
 */
static constexpr const char *kCloudProvider = "cloud.provider";

/**
 * The cloud account ID the resource is assigned to.
 */
static constexpr const char *kCloudAccountId = "cloud.account.id";

/**
 * The geographical region the resource is running.
 *
 * <p>Notes:
  <ul> <li>Refer to your provider's docs to see the available regions, for example <a
 href="https://www.alibabacloud.com/help/doc-detail/40654.htm">Alibaba Cloud regions</a>, <a
 href="https://aws.amazon.com/about-aws/global-infrastructure/regions_az/">AWS regions</a>, <a
 href="https://azure.microsoft.com/en-us/global-infrastructure/geographies/">Azure regions</a>, <a
 href="https://cloud.google.com/about/locations">Google Cloud regions</a>, or <a
 href="https://intl.cloud.tencent.com/document/product/213/6091">Tencent Cloud regions</a>.</li>
 </ul>
 */
static constexpr const char *kCloudRegion = "cloud.region";

/**
 * Cloud regions often have multiple, isolated locations known as zones to increase availability.
 Availability zone represents the zone where the resource is running.
 *
 * <p>Notes:
  <ul> <li>Availability zones are called &quot;zones&quot; on Alibaba Cloud and Google Cloud.</li>
 </ul>
 */
static constexpr const char *kCloudAvailabilityZone = "cloud.availability_zone";

/**
 * The cloud platform in use.
 *
 * <p>Notes:
  <ul> <li>The prefix of the service SHOULD match the one specified in {@code cloud.provider}.</li>
 </ul>
 */
static constexpr const char *kCloudPlatform = "cloud.platform";

/**
 * The Amazon Resource Name (ARN) of an <a
 * href="https://docs.aws.amazon.com/AmazonECS/latest/developerguide/ECS_instances.html">ECS
 * container instance</a>.
 */
static constexpr const char *kAwsEcsContainerArn = "aws.ecs.container.arn";

/**
 * The ARN of an <a
 * href="https://docs.aws.amazon.com/AmazonECS/latest/developerguide/clusters.html">ECS cluster</a>.
 */
static constexpr const char *kAwsEcsClusterArn = "aws.ecs.cluster.arn";

/**
 * The <a
 * href="https://docs.aws.amazon.com/AmazonECS/latest/developerguide/launch_types.html">launch
 * type</a> for an ECS task.
 */
static constexpr const char *kAwsEcsLaunchtype = "aws.ecs.launchtype";

/**
 * The ARN of an <a
 * href="https://docs.aws.amazon.com/AmazonECS/latest/developerguide/task_definitions.html">ECS task
 * definition</a>.
 */
static constexpr const char *kAwsEcsTaskArn = "aws.ecs.task.arn";

/**
 * The task definition family this task definition is a member of.
 */
static constexpr const char *kAwsEcsTaskFamily = "aws.ecs.task.family";

/**
 * The revision for this task definition.
 */
static constexpr const char *kAwsEcsTaskRevision = "aws.ecs.task.revision";

/**
 * The ARN of an EKS cluster.
 */
static constexpr const char *kAwsEksClusterArn = "aws.eks.cluster.arn";

/**
 * The name(s) of the AWS log group(s) an application is writing to.
 *
 * <p>Notes:
  <ul> <li>Multiple log groups must be supported for cases like multi-container applications, where
 a single application has sidecar containers, and each write to their own log group.</li> </ul>
 */
static constexpr const char *kAwsLogGroupNames = "aws.log.group.names";

/**
 * The Amazon Resource Name(s) (ARN) of the AWS log group(s).
 *
 * <p>Notes:
  <ul> <li>See the <a
 href="https://docs.aws.amazon.com/AmazonCloudWatch/latest/logs/iam-access-control-overview-cwl.html#CWL_ARN_Format">log
 group ARN format documentation</a>.</li> </ul>
 */
static constexpr const char *kAwsLogGroupArns = "aws.log.group.arns";

/**
 * The name(s) of the AWS log stream(s) an application is writing to.
 */
static constexpr const char *kAwsLogStreamNames = "aws.log.stream.names";

/**
 * The ARN(s) of the AWS log stream(s).
 *
 * <p>Notes:
  <ul> <li>See the <a
 href="https://docs.aws.amazon.com/AmazonCloudWatch/latest/logs/iam-access-control-overview-cwl.html#CWL_ARN_Format">log
 stream ARN format documentation</a>. One log group can contain several log streams, so these ARNs
 necessarily identify both a log group and a log stream.</li> </ul>
 */
static constexpr const char *kAwsLogStreamArns = "aws.log.stream.arns";

/**
 * Container name used by container runtime.
 */
static constexpr const char *kContainerName = "container.name";

/**
 * Container ID. Usually a UUID, as for example used to <a
 * href="https://docs.docker.com/engine/reference/run/#container-identification">identify Docker
 * containers</a>. The UUID might be abbreviated.
 */
static constexpr const char *kContainerId = "container.id";

/**
 * The container runtime managing this container.
 */
static constexpr const char *kContainerRuntime = "container.runtime";

/**
 * Name of the image the container was built on.
 */
static constexpr const char *kContainerImageName = "container.image.name";

/**
 * Container image tag.
 */
static constexpr const char *kContainerImageTag = "container.image.tag";

/**
 * Name of the <a href="https://en.wikipedia.org/wiki/Deployment_environment">deployment
 * environment</a> (aka deployment tier).
 */
static constexpr const char *kDeploymentEnvironment = "deployment.environment";

/**
 * A unique identifier representing the device
 *
 * <p>Notes:
  <ul> <li>The device identifier MUST only be defined using the values outlined below. This value is
 not an advertising identifier and MUST NOT be used as such. On iOS (Swift or Objective-C), this
 value MUST be equal to the <a
 href="https://developer.apple.com/documentation/uikit/uidevice/1620059-identifierforvendor">vendor
 identifier</a>. On Android (Java or Kotlin), this value MUST be equal to the Firebase Installation
 ID or a globally unique UUID which is persisted across sessions in your application. More
 information can be found <a
 href="https://developer.android.com/training/articles/user-data-ids">here</a> on best practices and
 exact implementation details. Caution should be taken when storing personal data or anything which
 can identify a user. GDPR and data protection laws may apply, ensure you do your own due
 diligence.</li> </ul>
 */
static constexpr const char *kDeviceId = "device.id";

/**
 * The model identifier for the device
 *
 * <p>Notes:
  <ul> <li>It's recommended this value represents a machine readable version of the model identifier
 rather than the market or consumer-friendly name of the device.</li> </ul>
 */
static constexpr const char *kDeviceModelIdentifier = "device.model.identifier";

/**
 * The marketing name for the device model
 *
 * <p>Notes:
  <ul> <li>It's recommended this value represents a human readable version of the device model
 rather than a machine readable alternative.</li> </ul>
 */
static constexpr const char *kDeviceModelName = "device.model.name";

/**
 * The name of the device manufacturer
 *
 * <p>Notes:
  <ul> <li>The Android OS provides this field via <a
 href="https://developer.android.com/reference/android/os/Build#MANUFACTURER">Build</a>. iOS apps
 SHOULD hardcode the value {@code Apple}.</li> </ul>
 */
static constexpr const char *kDeviceManufacturer = "device.manufacturer";

/**
 * The name of the single function that this runtime instance executes.
 *
 * <p>Notes:
  <ul> <li>This is the name of the function as configured/deployed on the FaaS
platform and is usually different from the name of the callback
function (which may be stored in the
<a href="../../trace/semantic_conventions/span-general.md#source-code-attributes">{@code
code.namespace}/{@code code.function}</a> span attributes).</li><li>For some cloud providers, the
above definition is ambiguous. The following definition of function name MUST be used for this
attribute (and consequently the span name) for the listed cloud
providers/products:</li><li><strong>Azure:</strong>  The full name {@code <FUNCAPP>/<FUNC>}, i.e.,
function app name followed by a forward slash followed by the function name (this form can also be
seen in the resource JSON for the function). This means that a span attribute MUST be used, as an
Azure function app can host multiple functions that would usually share a TracerProvider (see also
the {@code faas.id} attribute).</li>
 </ul>
 */
static constexpr const char *kFaasName = "faas.name";

/**
 * The unique ID of the single function that this runtime instance executes.
 *
 * <p>Notes:
  <ul> <li>On some cloud providers, it may not be possible to determine the full ID at startup,
so consider setting {@code faas.id} as a span attribute instead.</li><li>The exact value to use for
{@code faas.id} depends on the cloud provider:</li><li><strong>AWS Lambda:</strong> The function <a
href="https://docs.aws.amazon.com/general/latest/gr/aws-arns-and-namespaces.html">ARN</a>. Take care
not to use the &quot;invoked ARN&quot; directly but replace any <a
href="https://docs.aws.amazon.com/lambda/latest/dg/configuration-aliases.html">alias suffix</a> with
the resolved function version, as the same runtime instance may be invokable with multiple different
aliases.</li> <li><strong>GCP:</strong> The <a
href="https://cloud.google.com/iam/docs/full-resource-names">URI of the resource</a></li>
<li><strong>Azure:</strong> The <a
href="https://docs.microsoft.com/en-us/rest/api/resources/resources/get-by-id">Fully Qualified
Resource ID</a> of the invoked function, <em>not</em> the function app, having the form
{@code
/subscriptions/<SUBSCIPTION_GUID>/resourceGroups/<RG>/providers/Microsoft.Web/sites/<FUNCAPP>/functions/<FUNC>}.
This means that a span attribute MUST be used, as an Azure function app can host multiple functions
that would usually share a TracerProvider.</li>
 </ul>
 */
static constexpr const char *kFaasId = "faas.id";

/**
 * The immutable version of the function being executed.
 *
 * <p>Notes:
  <ul> <li>Depending on the cloud provider and platform, use:</li><li><strong>AWS Lambda:</strong>
The <a href="https://docs.aws.amazon.com/lambda/latest/dg/configuration-versions.html">function
version</a> (an integer represented as a decimal string).</li> <li><strong>Google Cloud
Run:</strong> The <a href="https://cloud.google.com/run/docs/managing/revisions">revision</a> (i.e.,
the function name plus the revision suffix).</li> <li><strong>Google Cloud Functions:</strong> The
value of the <a
href="https://cloud.google.com/functions/docs/env-var#runtime_environment_variables_set_automatically">{@code
K_REVISION} environment variable</a>.</li> <li><strong>Azure Functions:</strong> Not applicable. Do
not set this attribute.</li>
 </ul>
 */
static constexpr const char *kFaasVersion = "faas.version";

/**
 * The execution environment ID as a string, that will be potentially reused for other invocations
 to the same function/function version.
 *
 * <p>Notes:
  <ul> <li><strong>AWS Lambda:</strong> Use the (full) log stream name.</li>
 </ul>
 */
static constexpr const char *kFaasInstance = "faas.instance";

/**
 * The amount of memory available to the serverless function in MiB.
 *
 * <p>Notes:
  <ul> <li>It's recommended to set this attribute since e.g. too little memory can easily stop a
 Java AWS Lambda function from working correctly. On AWS Lambda, the environment variable {@code
 AWS_LAMBDA_FUNCTION_MEMORY_SIZE} provides this information.</li> </ul>
 */
static constexpr const char *kFaasMaxMemory = "faas.max_memory";

/**
 * Unique host ID. For Cloud, this must be the instance_id assigned by the cloud provider. For
 * non-containerized Linux systems, the {@code machine-id} located in {@code /etc/machine-id} or
 * {@code /var/lib/dbus/machine-id} may be used.
 */
static constexpr const char *kHostId = "host.id";

/**
 * Name of the host. On Unix systems, it may contain what the hostname command returns, or the fully
 * qualified hostname, or another name specified by the user.
 */
static constexpr const char *kHostName = "host.name";

/**
 * Type of host. For Cloud, this must be the machine type.
 */
static constexpr const char *kHostType = "host.type";

/**
 * The CPU architecture the host system is running on.
 */
static constexpr const char *kHostArch = "host.arch";

/**
 * Name of the VM image or OS install the host was instantiated from.
 */
static constexpr const char *kHostImageName = "host.image.name";

/**
 * VM image ID. For Cloud, this value is from the provider.
 */
static constexpr const char *kHostImageId = "host.image.id";

/**
 * The version string of the VM image as defined in <a href="README.md#version-attributes">Version
 * Attributes</a>.
 */
static constexpr const char *kHostImageVersion = "host.image.version";

/**
 * The name of the cluster.
 */
static constexpr const char *kK8sClusterName = "k8s.cluster.name";

/**
 * The name of the Node.
 */
static constexpr const char *kK8sNodeName = "k8s.node.name";

/**
 * The UID of the Node.
 */
static constexpr const char *kK8sNodeUid = "k8s.node.uid";

/**
 * The name of the namespace that the pod is running in.
 */
static constexpr const char *kK8sNamespaceName = "k8s.namespace.name";

/**
 * The UID of the Pod.
 */
static constexpr const char *kK8sPodUid = "k8s.pod.uid";

/**
 * The name of the Pod.
 */
static constexpr const char *kK8sPodName = "k8s.pod.name";

/**
 * The name of the Container from Pod specification, must be unique within a Pod. Container runtime
 * usually uses different globally unique name ({@code container.name}).
 */
static constexpr const char *kK8sContainerName = "k8s.container.name";

/**
 * Number of times the container was restarted. This attribute can be used to identify a particular
 * container (running or stopped) within a container spec.
 */
static constexpr const char *kK8sContainerRestartCount = "k8s.container.restart_count";

/**
 * The UID of the ReplicaSet.
 */
static constexpr const char *kK8sReplicasetUid = "k8s.replicaset.uid";

/**
 * The name of the ReplicaSet.
 */
static constexpr const char *kK8sReplicasetName = "k8s.replicaset.name";

/**
 * The UID of the Deployment.
 */
static constexpr const char *kK8sDeploymentUid = "k8s.deployment.uid";

/**
 * The name of the Deployment.
 */
static constexpr const char *kK8sDeploymentName = "k8s.deployment.name";

/**
 * The UID of the StatefulSet.
 */
static constexpr const char *kK8sStatefulsetUid = "k8s.statefulset.uid";

/**
 * The name of the StatefulSet.
 */
static constexpr const char *kK8sStatefulsetName = "k8s.statefulset.name";

/**
 * The UID of the DaemonSet.
 */
static constexpr const char *kK8sDaemonsetUid = "k8s.daemonset.uid";

/**
 * The name of the DaemonSet.
 */
static constexpr const char *kK8sDaemonsetName = "k8s.daemonset.name";

/**
 * The UID of the Job.
 */
static constexpr const char *kK8sJobUid = "k8s.job.uid";

/**
 * The name of the Job.
 */
static constexpr const char *kK8sJobName = "k8s.job.name";

/**
 * The UID of the CronJob.
 */
static constexpr const char *kK8sCronjobUid = "k8s.cronjob.uid";

/**
 * The name of the CronJob.
 */
static constexpr const char *kK8sCronjobName = "k8s.cronjob.name";

/**
 * The operating system type.
 */
static constexpr const char *kOsType = "os.type";

/**
 * Human readable (not intended to be parsed) OS version information, like e.g. reported by {@code
 * ver} or {@code lsb_release -a} commands.
 */
static constexpr const char *kOsDescription = "os.description";

/**
 * Human readable operating system name.
 */
static constexpr const char *kOsName = "os.name";

/**
 * The version string of the operating system as defined in <a
 * href="../../resource/semantic_conventions/README.md#version-attributes">Version Attributes</a>.
 */
static constexpr const char *kOsVersion = "os.version";

/**
 * Process identifier (PID).
 */
static constexpr const char *kProcessPid = "process.pid";

/**
 * Parent Process identifier (PID).
 */
static constexpr const char *kProcessParentPid = "process.parent_pid";

/**
 * The name of the process executable. On Linux based systems, can be set to the {@code Name} in
 * {@code proc/[pid]/status}. On Windows, can be set to the base name of {@code
 * GetProcessImageFileNameW}.
 */
static constexpr const char *kProcessExecutableName = "process.executable.name";

/**
 * The full path to the process executable. On Linux based systems, can be set to the target of
 * {@code proc/[pid]/exe}. On Windows, can be set to the result of {@code GetProcessImageFileNameW}.
 */
static constexpr const char *kProcessExecutablePath = "process.executable.path";

/**
 * The command used to launch the process (i.e. the command name). On Linux based systems, can be
 * set to the zeroth string in {@code proc/[pid]/cmdline}. On Windows, can be set to the first
 * parameter extracted from {@code GetCommandLineW}.
 */
static constexpr const char *kProcessCommand = "process.command";

/**
 * The full command used to launch the process as a single string representing the full command. On
 * Windows, can be set to the result of {@code GetCommandLineW}. Do not set this if you have to
 * assemble it just for monitoring; use {@code process.command_args} instead.
 */
static constexpr const char *kProcessCommandLine = "process.command_line";

/**
 * All the command arguments (including the command/executable itself) as received by the process.
 * On Linux-based systems (and some other Unixoid systems supporting procfs), can be set according
 * to the list of null-delimited strings extracted from {@code proc/[pid]/cmdline}. For libc-based
 * executables, this would be the full argv vector passed to {@code main}.
 */
static constexpr const char *kProcessCommandArgs = "process.command_args";

/**
 * The username of the user that owns the process.
 */
static constexpr const char *kProcessOwner = "process.owner";

/**
 * The name of the runtime of this process. For compiled native binaries, this SHOULD be the name of
 * the compiler.
 */
static constexpr const char *kProcessRuntimeName = "process.runtime.name";

/**
 * The version of the runtime of this process, as returned by the runtime without modification.
 */
static constexpr const char *kProcessRuntimeVersion = "process.runtime.version";

/**
 * An additional description about the runtime of the process, for example a specific vendor
 * customization of the runtime environment.
 */
static constexpr const char *kProcessRuntimeDescription = "process.runtime.description";

/**
 * Logical name of the service.
 *
 * <p>Notes:
  <ul> <li>MUST be the same for all instances of horizontally scaled services. If the value was not
 specified, SDKs MUST fallback to {@code unknown_service:} concatenated with <a
 href="process.md#process">{@code process.executable.name}</a>, e.g. {@code unknown_service:bash}.
 If {@code process.executable.name} is not available, the value MUST be set to {@code
 unknown_service}.</li> </ul>
 */
static constexpr const char *kServiceName = "service.name";

/**
 * A namespace for {@code service.name}.
 *
 * <p>Notes:
  <ul> <li>A string value having a meaning that helps to distinguish a group of services, for
 example the team name that owns a group of services. {@code service.name} is expected to be unique
 within the same namespace. If {@code service.namespace} is not specified in the Resource then
 {@code service.name} is expected to be unique for all services that have no explicit namespace
 defined (so the empty/unspecified namespace is simply one more valid namespace). Zero-length
 namespace string is assumed equal to unspecified namespace.</li> </ul>
 */
static constexpr const char *kServiceNamespace = "service.namespace";

/**
 * The string ID of the service instance.
 *
 * <p>Notes:
  <ul> <li>MUST be unique for each instance of the same {@code service.namespace,service.name} pair
 (in other words {@code service.namespace,service.name,service.instance.id} triplet MUST be globally
 unique). The ID helps to distinguish instances of the same service that exist at the same time
 (e.g. instances of a horizontally scaled service). It is preferable for the ID to be persistent and
 stay the same for the lifetime of the service instance, however it is acceptable that the ID is
 ephemeral and changes during important lifetime events for the service (e.g. service restarts). If
 the service has no inherent unique ID that can be used as the value of this attribute it is
 recommended to generate a random Version 1 or Version 4 RFC 4122 UUID (services aiming for
 reproducible UUIDs may also use Version 5, see RFC 4122 for more recommendations).</li> </ul>
 */
static constexpr const char *kServiceInstanceId = "service.instance.id";

/**
 * The version string of the service API or implementation.
 */
static constexpr const char *kServiceVersion = "service.version";

/**
 * The name of the telemetry SDK as defined above.
 */
static constexpr const char *kTelemetrySdkName = "telemetry.sdk.name";

/**
 * The language of the telemetry SDK.
 */
static constexpr const char *kTelemetrySdkLanguage = "telemetry.sdk.language";

/**
 * The version string of the telemetry SDK.
 */
static constexpr const char *kTelemetrySdkVersion = "telemetry.sdk.version";

/**
 * The version string of the auto instrumentation agent, if used.
 */
static constexpr const char *kTelemetryAutoVersion = "telemetry.auto.version";

/**
 * The name of the web engine.
 */
static constexpr const char *kWebengineName = "webengine.name";

/**
 * The version of the web engine.
 */
static constexpr const char *kWebengineVersion = "webengine.version";

/**
 * Additional description of the web engine (e.g. detailed version and edition information).
 */
static constexpr const char *kWebengineDescription = "webengine.description";

/**
 * The name of the instrumentation scope - ({@code InstrumentationScope.Name} in OTLP).
 */
static constexpr const char *kOtelScopeName = "otel.scope.name";

/**
 * The version of the instrumentation scope - ({@code InstrumentationScope.Version} in OTLP).
 */
static constexpr const char *kOtelScopeVersion = "otel.scope.version";

/**
 * Deprecated, use the {@code otel.scope.name} attribute.
 */
static constexpr const char *kOtelLibraryName = "otel.library.name";

/**
 * Deprecated, use the {@code otel.scope.version} attribute.
 */
static constexpr const char *kOtelLibraryVersion = "otel.library.version";

// Enum definitions
namespace CloudProviderValues
{
/** Alibaba Cloud. */
static constexpr const char *kAlibabaCloud = "alibaba_cloud";
/** Amazon Web Services. */
static constexpr const char *kAws = "aws";
/** Microsoft Azure. */
static constexpr const char *kAzure = "azure";
/** Google Cloud Platform. */
static constexpr const char *kGcp = "gcp";
/** IBM Cloud. */
static constexpr const char *kIbmCloud = "ibm_cloud";
/** Tencent Cloud. */
static constexpr const char *kTencentCloud = "tencent_cloud";
}  // namespace CloudProviderValues

namespace CloudPlatformValues
{
/** Alibaba Cloud Elastic Compute Service. */
static constexpr const char *kAlibabaCloudEcs = "alibaba_cloud_ecs";
/** Alibaba Cloud Function Compute. */
static constexpr const char *kAlibabaCloudFc = "alibaba_cloud_fc";
/** Red Hat OpenShift on Alibaba Cloud. */
static constexpr const char *kAlibabaCloudOpenshift = "alibaba_cloud_openshift";
/** AWS Elastic Compute Cloud. */
static constexpr const char *kAwsEc2 = "aws_ec2";
/** AWS Elastic Container Service. */
static constexpr const char *kAwsEcs = "aws_ecs";
/** AWS Elastic Kubernetes Service. */
static constexpr const char *kAwsEks = "aws_eks";
/** AWS Lambda. */
static constexpr const char *kAwsLambda = "aws_lambda";
/** AWS Elastic Beanstalk. */
static constexpr const char *kAwsElasticBeanstalk = "aws_elastic_beanstalk";
/** AWS App Runner. */
static constexpr const char *kAwsAppRunner = "aws_app_runner";
/** Red Hat OpenShift on AWS (ROSA). */
static constexpr const char *kAwsOpenshift = "aws_openshift";
/** Azure Virtual Machines. */
static constexpr const char *kAzureVm = "azure_vm";
/** Azure Container Instances. */
static constexpr const char *kAzureContainerInstances = "azure_container_instances";
/** Azure Kubernetes Service. */
static constexpr const char *kAzureAks = "azure_aks";
/** Azure Functions. */
static constexpr const char *kAzureFunctions = "azure_functions";
/** Azure App Service. */
static constexpr const char *kAzureAppService = "azure_app_service";
/** Azure Red Hat OpenShift. */
static constexpr const char *kAzureOpenshift = "azure_openshift";
/** Google Cloud Compute Engine (GCE). */
static constexpr const char *kGcpComputeEngine = "gcp_compute_engine";
/** Google Cloud Run. */
static constexpr const char *kGcpCloudRun = "gcp_cloud_run";
/** Google Cloud Kubernetes Engine (GKE). */
static constexpr const char *kGcpKubernetesEngine = "gcp_kubernetes_engine";
/** Google Cloud Functions (GCF). */
static constexpr const char *kGcpCloudFunctions = "gcp_cloud_functions";
/** Google Cloud App Engine (GAE). */
static constexpr const char *kGcpAppEngine = "gcp_app_engine";
/** Red Hat OpenShift on Google Cloud. */
static constexpr const char *kGoogleCloudOpenshift = "google_cloud_openshift";
/** Red Hat OpenShift on IBM Cloud. */
static constexpr const char *kIbmCloudOpenshift = "ibm_cloud_openshift";
/** Tencent Cloud Cloud Virtual Machine (CVM). */
static constexpr const char *kTencentCloudCvm = "tencent_cloud_cvm";
/** Tencent Cloud Elastic Kubernetes Service (EKS). */
static constexpr const char *kTencentCloudEks = "tencent_cloud_eks";
/** Tencent Cloud Serverless Cloud Function (SCF). */
static constexpr const char *kTencentCloudScf = "tencent_cloud_scf";
}  // namespace CloudPlatformValues

namespace AwsEcsLaunchtypeValues
{
/** ec2. */
static constexpr const char *kEc2 = "ec2";
/** fargate. */
static constexpr const char *kFargate = "fargate";
}  // namespace AwsEcsLaunchtypeValues

namespace HostArchValues
{
/** AMD64. */
static constexpr const char *kAmd64 = "amd64";
/** ARM32. */
static constexpr const char *kArm32 = "arm32";
/** ARM64. */
static constexpr const char *kArm64 = "arm64";
/** Itanium. */
static constexpr const char *kIa64 = "ia64";
/** 32-bit PowerPC. */
static constexpr const char *kPpc32 = "ppc32";
/** 64-bit PowerPC. */
static constexpr const char *kPpc64 = "ppc64";
/** IBM z/Architecture. */
static constexpr const char *kS390x = "s390x";
/** 32-bit x86. */
static constexpr const char *kX86 = "x86";
}  // namespace HostArchValues

namespace OsTypeValues
{
/** Microsoft Windows. */
static constexpr const char *kWindows = "windows";
/** Linux. */
static constexpr const char *kLinux = "linux";
/** Apple Darwin. */
static constexpr const char *kDarwin = "darwin";
/** FreeBSD. */
static constexpr const char *kFreebsd = "freebsd";
/** NetBSD. */
static constexpr const char *kNetbsd = "netbsd";
/** OpenBSD. */
static constexpr const char *kOpenbsd = "openbsd";
/** DragonFly BSD. */
static constexpr const char *kDragonflybsd = "dragonflybsd";
/** HP-UX (Hewlett Packard Unix). */
static constexpr const char *kHpux = "hpux";
/** AIX (Advanced Interactive eXecutive). */
static constexpr const char *kAix = "aix";
/** SunOS, Oracle Solaris. */
static constexpr const char *kSolaris = "solaris";
/** IBM z/OS. */
static constexpr const char *kZOs = "z_os";
}  // namespace OsTypeValues

namespace TelemetrySdkLanguageValues
{
/** cpp. */
static constexpr const char *kCpp = "cpp";
/** dotnet. */
static constexpr const char *kDotnet = "dotnet";
/** erlang. */
static constexpr const char *kErlang = "erlang";
/** go. */
static constexpr const char *kGo = "go";
/** java. */
static constexpr const char *kJava = "java";
/** nodejs. */
static constexpr const char *kNodejs = "nodejs";
/** php. */
static constexpr const char *kPhp = "php";
/** python. */
static constexpr const char *kPython = "python";
/** ruby. */
static constexpr const char *kRuby = "ruby";
/** webjs. */
static constexpr const char *kWebjs = "webjs";
/** swift. */
static constexpr const char *kSwift = "swift";
}  // namespace TelemetrySdkLanguageValues

}  // namespace SemanticConventions
}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
