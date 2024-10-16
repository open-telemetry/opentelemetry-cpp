

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
namespace test
{

/**
 * The fully qualified human readable name of the <a
 * href="https://en.wikipedia.org/wiki/Test_case">test case</a>.
 */
static const char *kTestCaseName = "test.case.name";

/**
 * The status of the actual test case result from test execution.
 */
static const char *kTestCaseResultStatus = "test.case.result.status";

/**
 * The human readable name of a <a href="https://en.wikipedia.org/wiki/Test_suite">test suite</a>.
 */
static const char *kTestSuiteName = "test.suite.name";

/**
 * The status of the test suite run.
 */
static const char *kTestSuiteRunStatus = "test.suite.run.status";

// DEBUG: {"brief": "The status of the actual test case result from test execution.\n", "examples":
// ["pass", "fail"], "name": "test.case.result.status", "requirement_level": "recommended",
// "root_namespace": "test", "stability": "experimental", "type": {"allow_custom_values": true,
// "members": [{"brief": "pass", "deprecated": none, "id": "pass", "note": none, "stability":
// "experimental", "value": "pass"}, {"brief": "fail", "deprecated": none, "id": "fail", "note":
// none, "stability": "experimental", "value": "fail"}]}}
namespace TestCaseResultStatusValues
{
/**
 * pass.
 */
// DEBUG: {"brief": "pass", "deprecated": none, "id": "pass", "note": none, "stability":
// "experimental", "value": "pass"}
static constexpr const char *kPass = "pass";
/**
 * fail.
 */
// DEBUG: {"brief": "fail", "deprecated": none, "id": "fail", "note": none, "stability":
// "experimental", "value": "fail"}
static constexpr const char *kFail = "fail";
}  // namespace TestCaseResultStatusValues

// DEBUG: {"brief": "The status of the test suite run.\n", "examples": ["success", "failure",
// "skipped", "aborted", "timed_out", "in_progress"], "name": "test.suite.run.status",
// "requirement_level": "recommended", "root_namespace": "test", "stability": "experimental",
// "type": {"allow_custom_values": true, "members": [{"brief": "success", "deprecated": none, "id":
// "success", "note": none, "stability": "experimental", "value": "success"}, {"brief": "failure",
// "deprecated": none, "id": "failure", "note": none, "stability": "experimental", "value":
// "failure"}, {"brief": "skipped", "deprecated": none, "id": "skipped", "note": none, "stability":
// "experimental", "value": "skipped"}, {"brief": "aborted", "deprecated": none, "id": "aborted",
// "note": none, "stability": "experimental", "value": "aborted"}, {"brief": "timed_out",
// "deprecated": none, "id": "timed_out", "note": none, "stability": "experimental", "value":
// "timed_out"}, {"brief": "in_progress", "deprecated": none, "id": "in_progress", "note": none,
// "stability": "experimental", "value": "in_progress"}]}}
namespace TestSuiteRunStatusValues
{
/**
 * success.
 */
// DEBUG: {"brief": "success", "deprecated": none, "id": "success", "note": none, "stability":
// "experimental", "value": "success"}
static constexpr const char *kSuccess = "success";
/**
 * failure.
 */
// DEBUG: {"brief": "failure", "deprecated": none, "id": "failure", "note": none, "stability":
// "experimental", "value": "failure"}
static constexpr const char *kFailure = "failure";
/**
 * skipped.
 */
// DEBUG: {"brief": "skipped", "deprecated": none, "id": "skipped", "note": none, "stability":
// "experimental", "value": "skipped"}
static constexpr const char *kSkipped = "skipped";
/**
 * aborted.
 */
// DEBUG: {"brief": "aborted", "deprecated": none, "id": "aborted", "note": none, "stability":
// "experimental", "value": "aborted"}
static constexpr const char *kAborted = "aborted";
/**
 * timed_out.
 */
// DEBUG: {"brief": "timed_out", "deprecated": none, "id": "timed_out", "note": none, "stability":
// "experimental", "value": "timed_out"}
static constexpr const char *kTimedOut = "timed_out";
/**
 * in_progress.
 */
// DEBUG: {"brief": "in_progress", "deprecated": none, "id": "in_progress", "note": none,
// "stability": "experimental", "value": "in_progress"}
static constexpr const char *kInProgress = "in_progress";
}  // namespace TestSuiteRunStatusValues

}  // namespace test
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
