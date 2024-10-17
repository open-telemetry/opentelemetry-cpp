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
 * The unique identifier of the feature flag.
 */
static constexpr const char *kFeatureFlagKey = "feature_flag.key";

/**
 * The name of the service provider that performs the flag evaluation.
 */
static constexpr const char *kFeatureFlagProviderName = "feature_flag.provider_name";

/**
 * SHOULD be a semantic identifier for a value. If one is unavailable, a stringified version of the
 * value can be used. <p> A semantic identifier, commonly referred to as a variant, provides a means
 * for referring to a value without including the value itself. This can
 * provide additional context for understanding the meaning behind a value.
 * For example, the variant @code red @endcode maybe be used for the value @code #c05543 @endcode.
 * <p>
 * A stringified version of the value can be used in situations where a
 * semantic identifier is unavailable. String representation of the value
 * should be determined by the implementer.
 */
static constexpr const char *kFeatureFlagVariant = "feature_flag.variant";

}  // namespace feature_flag
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
