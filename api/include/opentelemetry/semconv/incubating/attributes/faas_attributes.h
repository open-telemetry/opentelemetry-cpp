

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
namespace faas
{

/**
 * A boolean that is true if the serverless function is executed for the first time (aka
 * cold-start).
 */
static const char *kFaasColdstart = "faas.coldstart";

/**
 * A string containing the schedule period as <a
 * href="https://docs.oracle.com/cd/E12058_01/doc/doc.1014/e12030/cron_expressions.htm">Cron
 * Expression</a>.
 */
static const char *kFaasCron = "faas.cron";

/**
 * The name of the source on which the triggering operation was performed. For example, in Cloud
 * Storage or S3 corresponds to the bucket name, and in Cosmos DB to the database name.
 */
static const char *kFaasDocumentCollection = "faas.document.collection";

/**
 * The document name/table subjected to the operation. For example, in Cloud Storage or S3 is the
 * name of the file, and in Cosmos DB the table name.
 */
static const char *kFaasDocumentName = "faas.document.name";

/**
 * Describes the type of the operation that was performed on the data.
 */
static const char *kFaasDocumentOperation = "faas.document.operation";

/**
 * A string containing the time when the data was accessed in the <a
 * href="https://www.iso.org/iso-8601-date-and-time-format.html">ISO 8601</a> format expressed in <a
 * href="https://www.w3.org/TR/NOTE-datetime">UTC</a>.
 */
static const char *kFaasDocumentTime = "faas.document.time";

/**
 * The execution environment ID as a string, that will be potentially reused for other invocations
 * to the same function/function version. Note: * <strong>AWS Lambda:</strong> Use the (full) log
 * stream name.
 */
static const char *kFaasInstance = "faas.instance";

/**
 * The invocation ID of the current function invocation.
 */
static const char *kFaasInvocationId = "faas.invocation_id";

/**
 * The name of the invoked function.
 * Note: SHOULD be equal to the @code faas.name @endcode resource attribute of the invoked function.
 */
static const char *kFaasInvokedName = "faas.invoked_name";

/**
 * The cloud provider of the invoked function.
 * Note: SHOULD be equal to the @code cloud.provider @endcode resource attribute of the invoked
 * function.
 */
static const char *kFaasInvokedProvider = "faas.invoked_provider";

/**
 * The cloud region of the invoked function.
 * Note: SHOULD be equal to the @code cloud.region @endcode resource attribute of the invoked
 * function.
 */
static const char *kFaasInvokedRegion = "faas.invoked_region";

/**
 * The amount of memory available to the serverless function converted to Bytes.
 * Note: It's recommended to set this attribute since e.g. too little memory can easily stop a Java
 * AWS Lambda function from working correctly. On AWS Lambda, the environment variable @code
 * AWS_LAMBDA_FUNCTION_MEMORY_SIZE @endcode provides this information (which must be multiplied by
 * 1,048,576).
 */
static const char *kFaasMaxMemory = "faas.max_memory";

/**
 * The name of the single function that this runtime instance executes.
 * Note: This is the name of the function as configured/deployed on the FaaS
 * platform and is usually different from the name of the callback
 * function (which may be stored in the
 * <a href="/docs/general/attributes.md#source-code-attributes">@code code.namespace @endcode/@code
 * code.function @endcode</a> span attributes). <p> For some cloud providers, the above definition
 * is ambiguous. The following definition of function name MUST be used for this attribute (and
 * consequently the span name) for the listed cloud providers/products: <p> <ul>
 *   <li><strong>Azure:</strong>  The full name @code <FUNCAPP>/<FUNC> @endcode, i.e., function app
 * name followed by a forward slash followed by the function name (this form can also be seen in the
 * resource JSON for the function). This means that a span attribute MUST be used, as an Azure
 * function app can host multiple functions that would usually share a TracerProvider (see also the
 * @code cloud.resource_id @endcode attribute).</li>
 * </ul>
 */
static const char *kFaasName = "faas.name";

/**
 * A string containing the function invocation time in the <a
 * href="https://www.iso.org/iso-8601-date-and-time-format.html">ISO 8601</a> format expressed in <a
 * href="https://www.w3.org/TR/NOTE-datetime">UTC</a>.
 */
static const char *kFaasTime = "faas.time";

/**
 * Type of the trigger which caused this function invocation.
 */
static const char *kFaasTrigger = "faas.trigger";

/**
 * The immutable version of the function being executed.
 * Note: Depending on the cloud provider and platform, use:
 * <p>
 * <ul>
 *   <li><strong>AWS Lambda:</strong> The <a
 * href="https://docs.aws.amazon.com/lambda/latest/dg/configuration-versions.html">function
 * version</a> (an integer represented as a decimal string).</li> <li><strong>Google Cloud Run
 * (Services):</strong> The <a
 * href="https://cloud.google.com/run/docs/managing/revisions">revision</a> (i.e., the function name
 * plus the revision suffix).</li> <li><strong>Google Cloud Functions:</strong> The value of the <a
 * href="https://cloud.google.com/functions/docs/env-var#runtime_environment_variables_set_automatically">@code
 * K_REVISION @endcode environment variable</a>.</li> <li><strong>Azure Functions:</strong> Not
 * applicable. Do not set this attribute.</li>
 * </ul>
 */
static const char *kFaasVersion = "faas.version";

// DEBUG: {"brief": "Describes the type of the operation that was performed on the data.", "name":
// "faas.document.operation", "requirement_level": "recommended", "root_namespace": "faas",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": "When a
// new object is created.", "deprecated": none, "id": "insert", "note": none, "stability":
// "experimental", "value": "insert"}, {"brief": "When an object is modified.", "deprecated": none,
// "id": "edit", "note": none, "stability": "experimental", "value": "edit"}, {"brief": "When an
// object is deleted.", "deprecated": none, "id": "delete", "note": none, "stability":
// "experimental", "value": "delete"}]}}
namespace FaasDocumentOperationValues
{
/**
 * When a new object is created.
 */
// DEBUG: {"brief": "When a new object is created.", "deprecated": none, "id": "insert", "note":
// none, "stability": "experimental", "value": "insert"}
static constexpr const char *kInsert = "insert";
/**
 * When an object is modified.
 */
// DEBUG: {"brief": "When an object is modified.", "deprecated": none, "id": "edit", "note": none,
// "stability": "experimental", "value": "edit"}
static constexpr const char *kEdit = "edit";
/**
 * When an object is deleted.
 */
// DEBUG: {"brief": "When an object is deleted.", "deprecated": none, "id": "delete", "note": none,
// "stability": "experimental", "value": "delete"}
static constexpr const char *kDelete = "delete";
}  // namespace FaasDocumentOperationValues

// DEBUG: {"brief": "The cloud provider of the invoked function.\n", "name":
// "faas.invoked_provider", "note": "SHOULD be equal to the `cloud.provider` resource attribute of
// the invoked function.\n", "requirement_level": "recommended", "root_namespace": "faas",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": "Alibaba
// Cloud", "deprecated": none, "id": "alibaba_cloud", "note": none, "stability": "experimental",
// "value": "alibaba_cloud"}, {"brief": "Amazon Web Services", "deprecated": none, "id": "aws",
// "note": none, "stability": "experimental", "value": "aws"}, {"brief": "Microsoft Azure",
// "deprecated": none, "id": "azure", "note": none, "stability": "experimental", "value": "azure"},
// {"brief": "Google Cloud Platform", "deprecated": none, "id": "gcp", "note": none, "stability":
// "experimental", "value": "gcp"}, {"brief": "Tencent Cloud", "deprecated": none, "id":
// "tencent_cloud", "note": none, "stability": "experimental", "value": "tencent_cloud"}]}}
namespace FaasInvokedProviderValues
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
 * Tencent Cloud.
 */
// DEBUG: {"brief": "Tencent Cloud", "deprecated": none, "id": "tencent_cloud", "note": none,
// "stability": "experimental", "value": "tencent_cloud"}
static constexpr const char *kTencentCloud = "tencent_cloud";
}  // namespace FaasInvokedProviderValues

// DEBUG: {"brief": "Type of the trigger which caused this function invocation.\n", "name":
// "faas.trigger", "requirement_level": "recommended", "root_namespace": "faas", "stability":
// "experimental", "type": {"allow_custom_values": true, "members": [{"brief": "A response to some
// data source operation such as a database or filesystem read/write", "deprecated": none, "id":
// "datasource", "note": none, "stability": "experimental", "value": "datasource"}, {"brief": "To
// provide an answer to an inbound HTTP request", "deprecated": none, "id": "http", "note": none,
// "stability": "experimental", "value": "http"}, {"brief": "A function is set to be executed when
// messages are sent to a messaging system", "deprecated": none, "id": "pubsub", "note": none,
// "stability": "experimental", "value": "pubsub"}, {"brief": "A function is scheduled to be
// executed regularly", "deprecated": none, "id": "timer", "note": none, "stability":
// "experimental", "value": "timer"}, {"brief": "If none of the others apply", "deprecated": none,
// "id": "other", "note": none, "stability": "experimental", "value": "other"}]}}
namespace FaasTriggerValues
{
/**
 * A response to some data source operation such as a database or filesystem read/write.
 */
// DEBUG: {"brief": "A response to some data source operation such as a database or filesystem
// read/write", "deprecated": none, "id": "datasource", "note": none, "stability": "experimental",
// "value": "datasource"}
static constexpr const char *kDatasource = "datasource";
/**
 * To provide an answer to an inbound HTTP request.
 */
// DEBUG: {"brief": "To provide an answer to an inbound HTTP request", "deprecated": none, "id":
// "http", "note": none, "stability": "experimental", "value": "http"}
static constexpr const char *kHttp = "http";
/**
 * A function is set to be executed when messages are sent to a messaging system.
 */
// DEBUG: {"brief": "A function is set to be executed when messages are sent to a messaging system",
// "deprecated": none, "id": "pubsub", "note": none, "stability": "experimental", "value": "pubsub"}
static constexpr const char *kPubsub = "pubsub";
/**
 * A function is scheduled to be executed regularly.
 */
// DEBUG: {"brief": "A function is scheduled to be executed regularly", "deprecated": none, "id":
// "timer", "note": none, "stability": "experimental", "value": "timer"}
static constexpr const char *kTimer = "timer";
/**
 * If none of the others apply.
 */
// DEBUG: {"brief": "If none of the others apply", "deprecated": none, "id": "other", "note": none,
// "stability": "experimental", "value": "other"}
static constexpr const char *kOther = "other";
}  // namespace FaasTriggerValues

}  // namespace faas
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
