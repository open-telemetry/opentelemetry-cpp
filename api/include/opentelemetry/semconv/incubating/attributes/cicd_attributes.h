

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
namespace cicd
{

/**
 * The human readable name of the pipeline within a CI/CD system.
 */
static const char *kCicdPipelineName = "cicd.pipeline.name";

/**
 * The unique identifier of a pipeline run within a CI/CD system.
 */
static const char *kCicdPipelineRunId = "cicd.pipeline.run.id";

/**
 * The human readable name of a task within a pipeline. Task here most closely aligns with a <a
 * href="https://en.wikipedia.org/wiki/Pipeline_(computing)">computing process</a> in a pipeline.
 * Other terms for tasks include commands, steps, and procedures.
 */
static const char *kCicdPipelineTaskName = "cicd.pipeline.task.name";

/**
 * The unique identifier of a task run within a pipeline.
 */
static const char *kCicdPipelineTaskRunId = "cicd.pipeline.task.run.id";

/**
 * The <a href="https://en.wikipedia.org/wiki/URL">URL</a> of the pipeline run providing the
 * complete address in order to locate and identify the pipeline run.
 */
static const char *kCicdPipelineTaskRunUrlFull = "cicd.pipeline.task.run.url.full";

/**
 * The type of the task within a pipeline.
 */
static const char *kCicdPipelineTaskType = "cicd.pipeline.task.type";

// DEBUG: {"brief": "The type of the task within a pipeline.\n", "examples": ["build", "test",
// "deploy"], "name": "cicd.pipeline.task.type", "requirement_level": "recommended",
// "root_namespace": "cicd", "stability": "experimental", "type": {"allow_custom_values": true,
// "members": [{"brief": "build", "deprecated": none, "id": "build", "note": none, "stability":
// "experimental", "value": "build"}, {"brief": "test", "deprecated": none, "id": "test", "note":
// none, "stability": "experimental", "value": "test"}, {"brief": "deploy", "deprecated": none,
// "id": "deploy", "note": none, "stability": "experimental", "value": "deploy"}]}}
namespace CicdPipelineTaskTypeValues
{
/**
 * build.
 */
// DEBUG: {"brief": "build", "deprecated": none, "id": "build", "note": none, "stability":
// "experimental", "value": "build"}
static constexpr const char *kBuild = "build";
/**
 * test.
 */
// DEBUG: {"brief": "test", "deprecated": none, "id": "test", "note": none, "stability":
// "experimental", "value": "test"}
static constexpr const char *kTest = "test";
/**
 * deploy.
 */
// DEBUG: {"brief": "deploy", "deprecated": none, "id": "deploy", "note": none, "stability":
// "experimental", "value": "deploy"}
static constexpr const char *kDeploy = "deploy";
}  // namespace CicdPipelineTaskTypeValues

}  // namespace cicd
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
