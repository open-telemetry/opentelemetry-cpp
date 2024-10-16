

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
namespace cloud
{

/**
 * The cloud account ID the resource is assigned to.
 */
static const char *kCloudAccountId = "cloud.account.id";

/**
 * Cloud regions often have multiple, isolated locations known as zones to increase availability.
 * Availability zone represents the zone where the resource is running. Note: Availability zones are
 * called "zones" on Alibaba Cloud and Google Cloud.
 */
static const char *kCloudAvailabilityZone = "cloud.availability_zone";

/**
 * The cloud platform in use.
 * Note: The prefix of the service SHOULD match the one specified in @code cloud.provider @endcode.
 */
static const char *kCloudPlatform = "cloud.platform";

/**
 * Name of the cloud provider.
 */
static const char *kCloudProvider = "cloud.provider";

/**
 * The geographical region the resource is running.
 * Note: Refer to your provider's docs to see the available regions, for example <a
 * href="https://www.alibabacloud.com/help/doc-detail/40654.htm">Alibaba Cloud regions</a>, <a
 * href="https://aws.amazon.com/about-aws/global-infrastructure/regions_az/">AWS regions</a>, <a
 * href="https://azure.microsoft.com/global-infrastructure/geographies/">Azure regions</a>, <a
 * href="https://cloud.google.com/about/locations">Google Cloud regions</a>, or <a
 * href="https://www.tencentcloud.com/document/product/213/6091">Tencent Cloud regions</a>.
 */
static const char *kCloudRegion = "cloud.region";

/**
 * Cloud provider-specific native identifier of the monitored cloud resource (e.g. an <a
 * href="https://docs.aws.amazon.com/general/latest/gr/aws-arns-and-namespaces.html">ARN</a> on AWS,
 * a <a href="https://learn.microsoft.com/rest/api/resources/resources/get-by-id">fully qualified
 * resource ID</a> on Azure, a <a
 * href="https://cloud.google.com/apis/design/resource_names#full_resource_name">full resource
 * name</a> on GCP). Note: On some cloud providers, it may not be possible to determine the full ID
 * at startup, so it may be necessary to set @code cloud.resource_id @endcode as a span attribute
 * instead. <p> The exact value to use for @code cloud.resource_id @endcode depends on the cloud
 * provider. The following well-known definitions MUST be used if you set this attribute and they
 * apply: <p> <ul> <li><strong>AWS Lambda:</strong> The function <a
 * href="https://docs.aws.amazon.com/general/latest/gr/aws-arns-and-namespaces.html">ARN</a>. Take
 * care not to use the "invoked ARN" directly but replace any <a
 * href="https://docs.aws.amazon.com/lambda/latest/dg/configuration-aliases.html">alias suffix</a>
 * with the resolved function version, as the same runtime instance may be invocable with
 * multiple different aliases.</li>
 *   <li><strong>GCP:</strong> The <a
 * href="https://cloud.google.com/iam/docs/full-resource-names">URI of the resource</a></li>
 *   <li><strong>Azure:</strong> The <a
 * href="https://docs.microsoft.com/rest/api/resources/resources/get-by-id">Fully Qualified Resource
 * ID</a> of the invoked function, <em>not</em> the function app, having the form
 * @code
 * /subscriptions/<SUBSCIPTION_GUID>/resourceGroups/<RG>/providers/Microsoft.Web/sites/<FUNCAPP>/functions/<FUNC>
 * @endcode. This means that a span attribute MUST be used, as an Azure function app can host
 * multiple functions that would usually share a TracerProvider.</li>
 * </ul>
 */
static const char *kCloudResourceId = "cloud.resource_id";

// DEBUG: {"brief": "The cloud platform in use.\n", "name": "cloud.platform", "note": "The prefix of
// the service SHOULD match the one specified in `cloud.provider`.\n", "requirement_level":
// "recommended", "root_namespace": "cloud", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": "Alibaba Cloud Elastic Compute Service",
// "deprecated": none, "id": "alibaba_cloud_ecs", "note": none, "stability": "experimental",
// "value": "alibaba_cloud_ecs"}, {"brief": "Alibaba Cloud Function Compute", "deprecated": none,
// "id": "alibaba_cloud_fc", "note": none, "stability": "experimental", "value":
// "alibaba_cloud_fc"}, {"brief": "Red Hat OpenShift on Alibaba Cloud", "deprecated": none, "id":
// "alibaba_cloud_openshift", "note": none, "stability": "experimental", "value":
// "alibaba_cloud_openshift"}, {"brief": "AWS Elastic Compute Cloud", "deprecated": none, "id":
// "aws_ec2", "note": none, "stability": "experimental", "value": "aws_ec2"}, {"brief": "AWS Elastic
// Container Service", "deprecated": none, "id": "aws_ecs", "note": none, "stability":
// "experimental", "value": "aws_ecs"}, {"brief": "AWS Elastic Kubernetes Service", "deprecated":
// none, "id": "aws_eks", "note": none, "stability": "experimental", "value": "aws_eks"}, {"brief":
// "AWS Lambda", "deprecated": none, "id": "aws_lambda", "note": none, "stability": "experimental",
// "value": "aws_lambda"}, {"brief": "AWS Elastic Beanstalk", "deprecated": none, "id":
// "aws_elastic_beanstalk", "note": none, "stability": "experimental", "value":
// "aws_elastic_beanstalk"}, {"brief": "AWS App Runner", "deprecated": none, "id": "aws_app_runner",
// "note": none, "stability": "experimental", "value": "aws_app_runner"}, {"brief": "Red Hat
// OpenShift on AWS (ROSA)", "deprecated": none, "id": "aws_openshift", "note": none, "stability":
// "experimental", "value": "aws_openshift"}, {"brief": "Azure Virtual Machines", "deprecated":
// none, "id": "azure_vm", "note": none, "stability": "experimental", "value": "azure_vm"},
// {"brief": "Azure Container Apps", "deprecated": none, "id": "azure_container_apps", "note": none,
// "stability": "experimental", "value": "azure_container_apps"}, {"brief": "Azure Container
// Instances", "deprecated": none, "id": "azure_container_instances", "note": none, "stability":
// "experimental", "value": "azure_container_instances"}, {"brief": "Azure Kubernetes Service",
// "deprecated": none, "id": "azure_aks", "note": none, "stability": "experimental", "value":
// "azure_aks"}, {"brief": "Azure Functions", "deprecated": none, "id": "azure_functions", "note":
// none, "stability": "experimental", "value": "azure_functions"}, {"brief": "Azure App Service",
// "deprecated": none, "id": "azure_app_service", "note": none, "stability": "experimental",
// "value": "azure_app_service"}, {"brief": "Azure Red Hat OpenShift", "deprecated": none, "id":
// "azure_openshift", "note": none, "stability": "experimental", "value": "azure_openshift"},
// {"brief": "Google Bare Metal Solution (BMS)", "deprecated": none, "id":
// "gcp_bare_metal_solution", "note": none, "stability": "experimental", "value":
// "gcp_bare_metal_solution"}, {"brief": "Google Cloud Compute Engine (GCE)", "deprecated": none,
// "id": "gcp_compute_engine", "note": none, "stability": "experimental", "value":
// "gcp_compute_engine"}, {"brief": "Google Cloud Run", "deprecated": none, "id": "gcp_cloud_run",
// "note": none, "stability": "experimental", "value": "gcp_cloud_run"}, {"brief": "Google Cloud
// Kubernetes Engine (GKE)", "deprecated": none, "id": "gcp_kubernetes_engine", "note": none,
// "stability": "experimental", "value": "gcp_kubernetes_engine"}, {"brief": "Google Cloud Functions
// (GCF)", "deprecated": none, "id": "gcp_cloud_functions", "note": none, "stability":
// "experimental", "value": "gcp_cloud_functions"}, {"brief": "Google Cloud App Engine (GAE)",
// "deprecated": none, "id": "gcp_app_engine", "note": none, "stability": "experimental", "value":
// "gcp_app_engine"}, {"brief": "Red Hat OpenShift on Google Cloud", "deprecated": none, "id":
// "gcp_openshift", "note": none, "stability": "experimental", "value": "gcp_openshift"}, {"brief":
// "Red Hat OpenShift on IBM Cloud", "deprecated": none, "id": "ibm_cloud_openshift", "note": none,
// "stability": "experimental", "value": "ibm_cloud_openshift"}, {"brief": "Tencent Cloud Cloud
// Virtual Machine (CVM)", "deprecated": none, "id": "tencent_cloud_cvm", "note": none, "stability":
// "experimental", "value": "tencent_cloud_cvm"}, {"brief": "Tencent Cloud Elastic Kubernetes
// Service (EKS)", "deprecated": none, "id": "tencent_cloud_eks", "note": none, "stability":
// "experimental", "value": "tencent_cloud_eks"}, {"brief": "Tencent Cloud Serverless Cloud Function
// (SCF)", "deprecated": none, "id": "tencent_cloud_scf", "note": none, "stability": "experimental",
// "value": "tencent_cloud_scf"}]}}
namespace CloudPlatformValues
{
/**
 * Alibaba Cloud Elastic Compute Service.
 */
// DEBUG: {"brief": "Alibaba Cloud Elastic Compute Service", "deprecated": none, "id":
// "alibaba_cloud_ecs", "note": none, "stability": "experimental", "value": "alibaba_cloud_ecs"}
static constexpr const char *kAlibabaCloudEcs = "alibaba_cloud_ecs";
/**
 * Alibaba Cloud Function Compute.
 */
// DEBUG: {"brief": "Alibaba Cloud Function Compute", "deprecated": none, "id": "alibaba_cloud_fc",
// "note": none, "stability": "experimental", "value": "alibaba_cloud_fc"}
static constexpr const char *kAlibabaCloudFc = "alibaba_cloud_fc";
/**
 * Red Hat OpenShift on Alibaba Cloud.
 */
// DEBUG: {"brief": "Red Hat OpenShift on Alibaba Cloud", "deprecated": none, "id":
// "alibaba_cloud_openshift", "note": none, "stability": "experimental", "value":
// "alibaba_cloud_openshift"}
static constexpr const char *kAlibabaCloudOpenshift = "alibaba_cloud_openshift";
/**
 * AWS Elastic Compute Cloud.
 */
// DEBUG: {"brief": "AWS Elastic Compute Cloud", "deprecated": none, "id": "aws_ec2", "note": none,
// "stability": "experimental", "value": "aws_ec2"}
static constexpr const char *kAwsEc2 = "aws_ec2";
/**
 * AWS Elastic Container Service.
 */
// DEBUG: {"brief": "AWS Elastic Container Service", "deprecated": none, "id": "aws_ecs", "note":
// none, "stability": "experimental", "value": "aws_ecs"}
static constexpr const char *kAwsEcs = "aws_ecs";
/**
 * AWS Elastic Kubernetes Service.
 */
// DEBUG: {"brief": "AWS Elastic Kubernetes Service", "deprecated": none, "id": "aws_eks", "note":
// none, "stability": "experimental", "value": "aws_eks"}
static constexpr const char *kAwsEks = "aws_eks";
/**
 * AWS Lambda.
 */
// DEBUG: {"brief": "AWS Lambda", "deprecated": none, "id": "aws_lambda", "note": none, "stability":
// "experimental", "value": "aws_lambda"}
static constexpr const char *kAwsLambda = "aws_lambda";
/**
 * AWS Elastic Beanstalk.
 */
// DEBUG: {"brief": "AWS Elastic Beanstalk", "deprecated": none, "id": "aws_elastic_beanstalk",
// "note": none, "stability": "experimental", "value": "aws_elastic_beanstalk"}
static constexpr const char *kAwsElasticBeanstalk = "aws_elastic_beanstalk";
/**
 * AWS App Runner.
 */
// DEBUG: {"brief": "AWS App Runner", "deprecated": none, "id": "aws_app_runner", "note": none,
// "stability": "experimental", "value": "aws_app_runner"}
static constexpr const char *kAwsAppRunner = "aws_app_runner";
/**
 * Red Hat OpenShift on AWS (ROSA).
 */
// DEBUG: {"brief": "Red Hat OpenShift on AWS (ROSA)", "deprecated": none, "id": "aws_openshift",
// "note": none, "stability": "experimental", "value": "aws_openshift"}
static constexpr const char *kAwsOpenshift = "aws_openshift";
/**
 * Azure Virtual Machines.
 */
// DEBUG: {"brief": "Azure Virtual Machines", "deprecated": none, "id": "azure_vm", "note": none,
// "stability": "experimental", "value": "azure_vm"}
static constexpr const char *kAzureVm = "azure_vm";
/**
 * Azure Container Apps.
 */
// DEBUG: {"brief": "Azure Container Apps", "deprecated": none, "id": "azure_container_apps",
// "note": none, "stability": "experimental", "value": "azure_container_apps"}
static constexpr const char *kAzureContainerApps = "azure_container_apps";
/**
 * Azure Container Instances.
 */
// DEBUG: {"brief": "Azure Container Instances", "deprecated": none, "id":
// "azure_container_instances", "note": none, "stability": "experimental", "value":
// "azure_container_instances"}
static constexpr const char *kAzureContainerInstances = "azure_container_instances";
/**
 * Azure Kubernetes Service.
 */
// DEBUG: {"brief": "Azure Kubernetes Service", "deprecated": none, "id": "azure_aks", "note": none,
// "stability": "experimental", "value": "azure_aks"}
static constexpr const char *kAzureAks = "azure_aks";
/**
 * Azure Functions.
 */
// DEBUG: {"brief": "Azure Functions", "deprecated": none, "id": "azure_functions", "note": none,
// "stability": "experimental", "value": "azure_functions"}
static constexpr const char *kAzureFunctions = "azure_functions";
/**
 * Azure App Service.
 */
// DEBUG: {"brief": "Azure App Service", "deprecated": none, "id": "azure_app_service", "note":
// none, "stability": "experimental", "value": "azure_app_service"}
static constexpr const char *kAzureAppService = "azure_app_service";
/**
 * Azure Red Hat OpenShift.
 */
// DEBUG: {"brief": "Azure Red Hat OpenShift", "deprecated": none, "id": "azure_openshift", "note":
// none, "stability": "experimental", "value": "azure_openshift"}
static constexpr const char *kAzureOpenshift = "azure_openshift";
/**
 * Google Bare Metal Solution (BMS).
 */
// DEBUG: {"brief": "Google Bare Metal Solution (BMS)", "deprecated": none, "id":
// "gcp_bare_metal_solution", "note": none, "stability": "experimental", "value":
// "gcp_bare_metal_solution"}
static constexpr const char *kGcpBareMetalSolution = "gcp_bare_metal_solution";
/**
 * Google Cloud Compute Engine (GCE).
 */
// DEBUG: {"brief": "Google Cloud Compute Engine (GCE)", "deprecated": none, "id":
// "gcp_compute_engine", "note": none, "stability": "experimental", "value": "gcp_compute_engine"}
static constexpr const char *kGcpComputeEngine = "gcp_compute_engine";
/**
 * Google Cloud Run.
 */
// DEBUG: {"brief": "Google Cloud Run", "deprecated": none, "id": "gcp_cloud_run", "note": none,
// "stability": "experimental", "value": "gcp_cloud_run"}
static constexpr const char *kGcpCloudRun = "gcp_cloud_run";
/**
 * Google Cloud Kubernetes Engine (GKE).
 */
// DEBUG: {"brief": "Google Cloud Kubernetes Engine (GKE)", "deprecated": none, "id":
// "gcp_kubernetes_engine", "note": none, "stability": "experimental", "value":
// "gcp_kubernetes_engine"}
static constexpr const char *kGcpKubernetesEngine = "gcp_kubernetes_engine";
/**
 * Google Cloud Functions (GCF).
 */
// DEBUG: {"brief": "Google Cloud Functions (GCF)", "deprecated": none, "id": "gcp_cloud_functions",
// "note": none, "stability": "experimental", "value": "gcp_cloud_functions"}
static constexpr const char *kGcpCloudFunctions = "gcp_cloud_functions";
/**
 * Google Cloud App Engine (GAE).
 */
// DEBUG: {"brief": "Google Cloud App Engine (GAE)", "deprecated": none, "id": "gcp_app_engine",
// "note": none, "stability": "experimental", "value": "gcp_app_engine"}
static constexpr const char *kGcpAppEngine = "gcp_app_engine";
/**
 * Red Hat OpenShift on Google Cloud.
 */
// DEBUG: {"brief": "Red Hat OpenShift on Google Cloud", "deprecated": none, "id": "gcp_openshift",
// "note": none, "stability": "experimental", "value": "gcp_openshift"}
static constexpr const char *kGcpOpenshift = "gcp_openshift";
/**
 * Red Hat OpenShift on IBM Cloud.
 */
// DEBUG: {"brief": "Red Hat OpenShift on IBM Cloud", "deprecated": none, "id":
// "ibm_cloud_openshift", "note": none, "stability": "experimental", "value": "ibm_cloud_openshift"}
static constexpr const char *kIbmCloudOpenshift = "ibm_cloud_openshift";
/**
 * Tencent Cloud Cloud Virtual Machine (CVM).
 */
// DEBUG: {"brief": "Tencent Cloud Cloud Virtual Machine (CVM)", "deprecated": none, "id":
// "tencent_cloud_cvm", "note": none, "stability": "experimental", "value": "tencent_cloud_cvm"}
static constexpr const char *kTencentCloudCvm = "tencent_cloud_cvm";
/**
 * Tencent Cloud Elastic Kubernetes Service (EKS).
 */
// DEBUG: {"brief": "Tencent Cloud Elastic Kubernetes Service (EKS)", "deprecated": none, "id":
// "tencent_cloud_eks", "note": none, "stability": "experimental", "value": "tencent_cloud_eks"}
static constexpr const char *kTencentCloudEks = "tencent_cloud_eks";
/**
 * Tencent Cloud Serverless Cloud Function (SCF).
 */
// DEBUG: {"brief": "Tencent Cloud Serverless Cloud Function (SCF)", "deprecated": none, "id":
// "tencent_cloud_scf", "note": none, "stability": "experimental", "value": "tencent_cloud_scf"}
static constexpr const char *kTencentCloudScf = "tencent_cloud_scf";
}  // namespace CloudPlatformValues

// DEBUG: {"brief": "Name of the cloud provider.\n", "name": "cloud.provider", "requirement_level":
// "recommended", "root_namespace": "cloud", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": "Alibaba Cloud", "deprecated": none, "id":
// "alibaba_cloud", "note": none, "stability": "experimental", "value": "alibaba_cloud"}, {"brief":
// "Amazon Web Services", "deprecated": none, "id": "aws", "note": none, "stability":
// "experimental", "value": "aws"}, {"brief": "Microsoft Azure", "deprecated": none, "id": "azure",
// "note": none, "stability": "experimental", "value": "azure"}, {"brief": "Google Cloud Platform",
// "deprecated": none, "id": "gcp", "note": none, "stability": "experimental", "value": "gcp"},
// {"brief": "Heroku Platform as a Service", "deprecated": none, "id": "heroku", "note": none,
// "stability": "experimental", "value": "heroku"}, {"brief": "IBM Cloud", "deprecated": none, "id":
// "ibm_cloud", "note": none, "stability": "experimental", "value": "ibm_cloud"}, {"brief": "Tencent
// Cloud", "deprecated": none, "id": "tencent_cloud", "note": none, "stability": "experimental",
// "value": "tencent_cloud"}]}}
namespace CloudProviderValues
{
/**
 * Alibaba Cloud.
 */
// DEBUG: {"brief": "Alibaba Cloud", "deprecated": none, "id": "alibaba_cloud", "note": none,
// "stability": "experimental", "value": "alibaba_cloud"}
static constexpr const char *kAlibabaCloud = "alibaba_cloud";
/**
 * Amazon Web Services.
 */
// DEBUG: {"brief": "Amazon Web Services", "deprecated": none, "id": "aws", "note": none,
// "stability": "experimental", "value": "aws"}
static constexpr const char *kAws = "aws";
/**
 * Microsoft Azure.
 */
// DEBUG: {"brief": "Microsoft Azure", "deprecated": none, "id": "azure", "note": none, "stability":
// "experimental", "value": "azure"}
static constexpr const char *kAzure = "azure";
/**
 * Google Cloud Platform.
 */
// DEBUG: {"brief": "Google Cloud Platform", "deprecated": none, "id": "gcp", "note": none,
// "stability": "experimental", "value": "gcp"}
static constexpr const char *kGcp = "gcp";
/**
 * Heroku Platform as a Service.
 */
// DEBUG: {"brief": "Heroku Platform as a Service", "deprecated": none, "id": "heroku", "note":
// none, "stability": "experimental", "value": "heroku"}
static constexpr const char *kHeroku = "heroku";
/**
 * IBM Cloud.
 */
// DEBUG: {"brief": "IBM Cloud", "deprecated": none, "id": "ibm_cloud", "note": none, "stability":
// "experimental", "value": "ibm_cloud"}
static constexpr const char *kIbmCloud = "ibm_cloud";
/**
 * Tencent Cloud.
 */
// DEBUG: {"brief": "Tencent Cloud", "deprecated": none, "id": "tencent_cloud", "note": none,
// "stability": "experimental", "value": "tencent_cloud"}
static constexpr const char *kTencentCloud = "tencent_cloud";
}  // namespace CloudProviderValues

}  // namespace cloud
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
