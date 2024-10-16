

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
namespace graphql
{

/**
 * The GraphQL document being executed.
 * Note: The value may be sanitized to exclude sensitive information.
 */
static const char *kGraphqlDocument = "graphql.document";

/**
 * The name of the operation being executed.
 */
static const char *kGraphqlOperationName = "graphql.operation.name";

/**
 * The type of the operation being executed.
 */
static const char *kGraphqlOperationType = "graphql.operation.type";

// DEBUG: {"brief": "The type of the operation being executed.", "examples": ["query", "mutation",
// "subscription"], "name": "graphql.operation.type", "requirement_level": "recommended",
// "root_namespace": "graphql", "stability": "experimental", "type": {"allow_custom_values": true,
// "members": [{"brief": "GraphQL query", "deprecated": none, "id": "query", "note": none,
// "stability": "experimental", "value": "query"}, {"brief": "GraphQL mutation", "deprecated": none,
// "id": "mutation", "note": none, "stability": "experimental", "value": "mutation"}, {"brief":
// "GraphQL subscription", "deprecated": none, "id": "subscription", "note": none, "stability":
// "experimental", "value": "subscription"}]}}
namespace GraphqlOperationTypeValues
{
/**
 * GraphQL query.
 */
// DEBUG: {"brief": "GraphQL query", "deprecated": none, "id": "query", "note": none, "stability":
// "experimental", "value": "query"}
static constexpr const char *kQuery = "query";
/**
 * GraphQL mutation.
 */
// DEBUG: {"brief": "GraphQL mutation", "deprecated": none, "id": "mutation", "note": none,
// "stability": "experimental", "value": "mutation"}
static constexpr const char *kMutation = "mutation";
/**
 * GraphQL subscription.
 */
// DEBUG: {"brief": "GraphQL subscription", "deprecated": none, "id": "subscription", "note": none,
// "stability": "experimental", "value": "subscription"}
static constexpr const char *kSubscription = "subscription";
}  // namespace GraphqlOperationTypeValues

}  // namespace graphql
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
