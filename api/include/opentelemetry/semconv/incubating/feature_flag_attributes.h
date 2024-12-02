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
namespace feature_flag
{

/**
 * The unique identifier for the flag evaluation context. For example, the targeting key.
 */
static constexpr const char *kFeatureFlagContextId = "feature_flag.context.id";

/**
 * A message explaining the nature of an error occurring during flag evaluation.
 */
static constexpr const char *kFeatureFlagEvaluationErrorMessage =
    "feature_flag.evaluation.error.message";

/**
 * The reason code which shows how a feature flag value was determined.
 */
static constexpr const char *kFeatureFlagEvaluationReason = "feature_flag.evaluation.reason";

/**
 * The lookup key of the feature flag.
 */
static constexpr const char *kFeatureFlagKey = "feature_flag.key";

/**
 * Identifies the feature flag provider.
 */
static constexpr const char *kFeatureFlagProviderName = "feature_flag.provider_name";

/**
 * The identifier of the <a href="https://openfeature.dev/specification/glossary/#flag-set">flag
 * set</a> to which the feature flag belongs.
 */
static constexpr const char *kFeatureFlagSetId = "feature_flag.set.id";

/**
 * A semantic identifier for an evaluated flag value.
 * <p>
 * A semantic identifier, commonly referred to as a variant, provides a means
 * for referring to a value without including the value itself. This can
 * provide additional context for understanding the meaning behind a value.
 * For example, the variant @code red @endcode maybe be used for the value @code #c05543 @endcode.
 */
static constexpr const char *kFeatureFlagVariant = "feature_flag.variant";

/**
 * The version of the ruleset used during the evaluation. This may be any stable value which
 * uniquely identifies the ruleset.
 */
static constexpr const char *kFeatureFlagVersion = "feature_flag.version";

namespace FeatureFlagEvaluationReasonValues
{
/**
 * The resolved value is static (no dynamic evaluation).
 */
static constexpr const char *kStatic = "static";

/**
 * The resolved value fell back to a pre-configured value (no dynamic evaluation occurred or dynamic
 * evaluation yielded no result).
 */
static constexpr const char *kDefault = "default";

/**
 * The resolved value was the result of a dynamic evaluation, such as a rule or specific
 * user-targeting.
 */
static constexpr const char *kTargetingMatch = "targeting_match";

/**
 * The resolved value was the result of pseudorandom assignment.
 */
static constexpr const char *kSplit = "split";

/**
 * The resolved value was retrieved from cache.
 */
static constexpr const char *kCached = "cached";

/**
 * The resolved value was the result of the flag being disabled in the management system.
 */
static constexpr const char *kDisabled = "disabled";

/**
 * The reason for the resolved value could not be determined.
 */
static constexpr const char *kUnknown = "unknown";

/**
 * The resolved value is non-authoritative or possibly out of date
 */
static constexpr const char *kStale = "stale";

/**
 * The resolved value was the result of an error.
 */
static constexpr const char *kError = "error";

}  // namespace FeatureFlagEvaluationReasonValues

}  // namespace feature_flag
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
