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
  Parent-child Reference type

  @deprecated
  {"note": "OpenTracing compatibility requirements are <a
  href="https://opentelemetry.io/docs/specs/otel/compatibility/opentracing/">deprecated in
  OpenTelemetry Specification</a>", "reason": "obsoleted"} <p> The causal relationship between a
  child Span and a parent Span.
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kOpentracingRefType = "opentracing.ref_type";

namespace OpentracingRefTypeValues
{
/**
  The parent Span depends on the child Span in some capacity
 */
static constexpr const char *kChildOf = "child_of";

/**
  The parent Span doesn't depend in any way on the result of the child Span
 */
static constexpr const char *kFollowsFrom = "follows_from";

}  // namespace OpentracingRefTypeValues

}  // namespace opentracing
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
