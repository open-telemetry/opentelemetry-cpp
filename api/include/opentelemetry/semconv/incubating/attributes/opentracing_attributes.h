

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
namespace opentracing
{

/**
 * Parent-child Reference type.
 * Note: The causal relationship between a child Span and a parent Span.
 */
static const char *kOpentracingRefType = "opentracing.ref_type";

// DEBUG: {"brief": "Parent-child Reference type", "name": "opentracing.ref_type", "note": "The
// causal relationship between a child Span and a parent Span.\n", "requirement_level":
// "recommended", "root_namespace": "opentracing", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": "The parent Span depends on the child Span in
// some capacity", "deprecated": none, "id": "child_of", "note": none, "stability": "experimental",
// "value": "child_of"}, {"brief": "The parent Span doesn't depend in any way on the result of the
// child Span", "deprecated": none, "id": "follows_from", "note": none, "stability": "experimental",
// "value": "follows_from"}]}}
namespace OpentracingRefTypeValues
{
/**
 * The parent Span depends on the child Span in some capacity.
 */
// DEBUG: {"brief": "The parent Span depends on the child Span in some capacity", "deprecated":
// none, "id": "child_of", "note": none, "stability": "experimental", "value": "child_of"}
static constexpr const char *kChildOf = "child_of";
/**
 * The parent Span doesn't depend in any way on the result of the child Span.
 */
// DEBUG: {"brief": "The parent Span doesn't depend in any way on the result of the child Span",
// "deprecated": none, "id": "follows_from", "note": none, "stability": "experimental", "value":
// "follows_from"}
static constexpr const char *kFollowsFrom = "follows_from";
}  // namespace OpentracingRefTypeValues

}  // namespace opentracing
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
