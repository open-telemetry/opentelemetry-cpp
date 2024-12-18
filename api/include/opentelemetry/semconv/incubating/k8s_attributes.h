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
namespace k8s
{

/**
 * The name of the cluster.
 */
static constexpr const char *kK8sClusterName = "k8s.cluster.name";

/**
 * A pseudo-ID for the cluster, set to the UID of the @code kube-system @endcode namespace.
 * <p>
 * K8s doesn't have support for obtaining a cluster ID. If this is ever
 * added, we will recommend collecting the @code k8s.cluster.uid @endcode through the
 * official APIs. In the meantime, we are able to use the @code uid @endcode of the
 * @code kube-system @endcode namespace as a proxy for cluster ID. Read on for the
 * rationale.
 * <p>
 * Every object created in a K8s cluster is assigned a distinct UID. The
 * @code kube-system @endcode namespace is used by Kubernetes itself and will exist
 * for the lifetime of the cluster. Using the @code uid @endcode of the @code kube-system @endcode
 * namespace is a reasonable proxy for the K8s ClusterID as it will only
 * change if the cluster is rebuilt. Furthermore, Kubernetes UIDs are
 * UUIDs as standardized by
 * <a href="https://www.itu.int/ITU-T/studygroups/com17/oid.html">ISO/IEC 9834-8 and ITU-T
 * X.667</a>. Which states: <p> <blockquote> If generated according to one of the mechanisms defined
 * in Rec. ITU-T X.667 | ISO/IEC 9834-8, a UUID is either guaranteed to be different from all other
 * UUIDs generated before 3603 A.D., or is extremely likely to be different (depending on the
 * mechanism chosen).</blockquote>
 *
 * <p>
 * Therefore, UIDs between clusters should be extremely unlikely to
 * conflict.
 */
static constexpr const char *kK8sClusterUid = "k8s.cluster.uid";

/**
 * The name of the Container from Pod specification, must be unique within a Pod. Container runtime
 * usually uses different globally unique name (@code container.name @endcode).
 */
static constexpr const char *kK8sContainerName = "k8s.container.name";

/**
 * Number of times the container was restarted. This attribute can be used to identify a particular
 * container (running or stopped) within a container spec.
 */
static constexpr const char *kK8sContainerRestartCount = "k8s.container.restart_count";

/**
 * Last terminated reason of the Container.
 */
static constexpr const char *kK8sContainerStatusLastTerminatedReason =
    "k8s.container.status.last_terminated_reason";

/**
 * The name of the CronJob.
 */
static constexpr const char *kK8sCronjobName = "k8s.cronjob.name";

/**
 * The UID of the CronJob.
 */
static constexpr const char *kK8sCronjobUid = "k8s.cronjob.uid";

/**
 * The name of the DaemonSet.
 */
static constexpr const char *kK8sDaemonsetName = "k8s.daemonset.name";

/**
 * The UID of the DaemonSet.
 */
static constexpr const char *kK8sDaemonsetUid = "k8s.daemonset.uid";

/**
 * The name of the Deployment.
 */
static constexpr const char *kK8sDeploymentName = "k8s.deployment.name";

/**
 * The UID of the Deployment.
 */
static constexpr const char *kK8sDeploymentUid = "k8s.deployment.uid";

/**
 * The name of the Job.
 */
static constexpr const char *kK8sJobName = "k8s.job.name";

/**
 * The UID of the Job.
 */
static constexpr const char *kK8sJobUid = "k8s.job.uid";

/**
 * The name of the namespace that the pod is running in.
 */
static constexpr const char *kK8sNamespaceName = "k8s.namespace.name";

/**
 * The name of the Node.
 */
static constexpr const char *kK8sNodeName = "k8s.node.name";

/**
 * The UID of the Node.
 */
static constexpr const char *kK8sNodeUid = "k8s.node.uid";

/**
 * The annotation key-value pairs placed on the Pod, the @code <key> @endcode being the annotation
 * name, the value being the annotation value.
 */
static constexpr const char *kK8sPodAnnotation = "k8s.pod.annotation";

/**
 * The label key-value pairs placed on the Pod, the @code <key> @endcode being the label name, the
 * value being the label value.
 */
static constexpr const char *kK8sPodLabel = "k8s.pod.label";

/**
 * Deprecated, use @code k8s.pod.label @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code k8s.pod.label @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kK8sPodLabels = "k8s.pod.labels";

/**
 * The name of the Pod.
 */
static constexpr const char *kK8sPodName = "k8s.pod.name";

/**
 * The UID of the Pod.
 */
static constexpr const char *kK8sPodUid = "k8s.pod.uid";

/**
 * The name of the ReplicaSet.
 */
static constexpr const char *kK8sReplicasetName = "k8s.replicaset.name";

/**
 * The UID of the ReplicaSet.
 */
static constexpr const char *kK8sReplicasetUid = "k8s.replicaset.uid";

/**
 * The name of the StatefulSet.
 */
static constexpr const char *kK8sStatefulsetName = "k8s.statefulset.name";

/**
 * The UID of the StatefulSet.
 */
static constexpr const char *kK8sStatefulsetUid = "k8s.statefulset.uid";

/**
 * The name of the K8s volume.
 */
static constexpr const char *kK8sVolumeName = "k8s.volume.name";

/**
 * The type of the K8s volume.
 */
static constexpr const char *kK8sVolumeType = "k8s.volume.type";

namespace K8sVolumeTypeValues
{
/**
 * A <a
 * href="https://v1-29.docs.kubernetes.io/docs/concepts/storage/volumes/#persistentvolumeclaim">persistentVolumeClaim</a>
 * volume
 */
static constexpr const char *kPersistentVolumeClaim = "persistentVolumeClaim";

/**
 * A <a
 * href="https://v1-29.docs.kubernetes.io/docs/concepts/storage/volumes/#configmap">configMap</a>
 * volume
 */
static constexpr const char *kConfigMap = "configMap";

/**
 * A <a
 * href="https://v1-29.docs.kubernetes.io/docs/concepts/storage/volumes/#downwardapi">downwardAPI</a>
 * volume
 */
static constexpr const char *kDownwardApi = "downwardAPI";

/**
 * An <a
 * href="https://v1-29.docs.kubernetes.io/docs/concepts/storage/volumes/#emptydir">emptyDir</a>
 * volume
 */
static constexpr const char *kEmptyDir = "emptyDir";

/**
 * A <a href="https://v1-29.docs.kubernetes.io/docs/concepts/storage/volumes/#secret">secret</a>
 * volume
 */
static constexpr const char *kSecret = "secret";

/**
 * A <a href="https://v1-29.docs.kubernetes.io/docs/concepts/storage/volumes/#local">local</a>
 * volume
 */
static constexpr const char *kLocal = "local";

}  // namespace K8sVolumeTypeValues

}  // namespace k8s
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
