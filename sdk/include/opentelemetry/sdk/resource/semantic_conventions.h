/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

/*
  DO NOT EDIT, this is an Auto-generated file
  from buildscripts/semantic-convention/templates/SemanticAttributes.h.j2
*/

#pragma once

#include "opentelemetry/common/macros.h"
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
static constexpr const char *kSchemaUrl = "https://opentelemetry.io/schemas/1.25.0";

/**
 * The ID of a running ECS task. The ID MUST be extracted from {@code task.arn}.
 */
static constexpr const char *kAwsEcsTaskId = "aws.ecs.task.id";

/**
 * The ARN of an <a
 * href="https://docs.aws.amazon.com/AmazonECS/latest/developerguide/clusters.html">ECS cluster</a>.
 */
static constexpr const char *kAwsEcsClusterArn = "aws.ecs.cluster.arn";

/**
 * The Amazon Resource Name (ARN) of an <a
 * href="https://docs.aws.amazon.com/AmazonECS/latest/developerguide/ECS_instances.html">ECS
 * container instance</a>.
 */
static constexpr const char *kAwsEcsContainerArn = "aws.ecs.container.arn";

/**
 * The <a
 * href="https://docs.aws.amazon.com/AmazonECS/latest/developerguide/launch_types.html">launch
 * type</a> for an ECS task.
 */
static constexpr const char *kAwsEcsLaunchtype = "aws.ecs.launchtype";

/**
 * The ARN of a running <a
 * href="https://docs.aws.amazon.com/AmazonECS/latest/developerguide/ecs-account-settings.html#ecs-resource-ids">ECS
 * task</a>.
 */
static constexpr const char *kAwsEcsTaskArn = "aws.ecs.task.arn";

/**
 * The family name of the <a
 * href="https://docs.aws.amazon.com/AmazonECS/latest/developerguide/task_definitions.html">ECS task
 * definition</a> used to create the ECS task.
 */
static constexpr const char *kAwsEcsTaskFamily = "aws.ecs.task.family";

/**
 * The revision for the task definition used to create the ECS task.
 */
static constexpr const char *kAwsEcsTaskRevision = "aws.ecs.task.revision";

/**
 * The ARN of an EKS cluster.
 */
static constexpr const char *kAwsEksClusterArn = "aws.eks.cluster.arn";

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
 * The name(s) of the AWS log group(s) an application is writing to.
 *
 * <p>Notes:
  <ul> <li>Multiple log groups must be supported for cases like multi-container applications, where
 a single application has sidecar containers, and each write to their own log group.</li> </ul>
 */
static constexpr const char *kAwsLogGroupNames = "aws.log.group.names";

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
 * The name(s) of the AWS log stream(s) an application is writing to.
 */
static constexpr const char *kAwsLogStreamNames = "aws.log.stream.names";

/**
 * Unique identifier for the application
 */
static constexpr const char *kHerokuAppId = "heroku.app.id";

/**
 * Commit hash for the current release
 */
static constexpr const char *kHerokuReleaseCommit = "heroku.release.commit";

/**
 * Time and date the release was created
 */
static constexpr const char *kHerokuReleaseCreationTimestamp = "heroku.release.creation_timestamp";

/**
 * The name of the web engine.
 */
static constexpr const char *kWebengineName = "webengine.name";

/**
 * Additional description of the web engine (e.g. detailed version and edition information).
 */
static constexpr const char *kWebengineDescription = "webengine.description";

/**
 * The version of the web engine.
 */
static constexpr const char *kWebengineVersion = "webengine.version";

/**
 * The name of the instrumentation scope - ({@code InstrumentationScope.Name} in OTLP).
 */
static constexpr const char *kOtelScopeName = "otel.scope.name";

/**
 * The version of the instrumentation scope - ({@code InstrumentationScope.Version} in OTLP).
 */
static constexpr const char *kOtelScopeVersion = "otel.scope.version";

/**
 * None
 *
 * @deprecated None.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kOtelLibraryName = "otel.library.name";

/**
 * None
 *
 * @deprecated None.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kOtelLibraryVersion = "otel.library.version";

// Enum definitions
namespace AwsEcsLaunchtypeValues
{
/** ec2. */
static constexpr const char *kEc2 = "ec2";
/** fargate. */
static constexpr const char *kFargate = "fargate";
}  // namespace AwsEcsLaunchtypeValues

}  // namespace SemanticConventions
}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
